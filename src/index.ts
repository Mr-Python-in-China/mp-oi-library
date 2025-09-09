import path from 'path';
import fs from 'fs';
import * as vscode from 'vscode';
import { promisify } from 'util';

const includeRegex = /^#include "mrpython\/(.+)"$/;
const extensionFileNoteRegex = /^\/\/ mp-oi-library extension note: file (.+)$/;
const extensionStartNoteStr =
  '// mp-oi-library extension note: start. Do not modify this part.';
const extensionEndNoteStr = '// mp-oi-library extension note: end';
const extensionHeaderStr = '// mp-oi-library extension header: ';

interface HeaderItem extends vscode.QuickPickItem {
  data: { content: string; dependencies: string[] };
}

export async function activate(context: vscode.ExtensionContext) {
  const packageJson = context.extension
    .packageJSON as typeof import('../package.json');
  const headMessage = `library version: @${packageJson.publisher}-${packageJson.version}`;

  const headers = await getProcessedFileContent(context.extensionPath);

  const items: HeaderItem[] = Array.from(headers.entries(), ([name, dat]) => ({
    label: name,
    data: {
      content: dat.content,
      dependencies: Array.from(dat.dependencies).map(x => {
        const res = headers.get(x);
        if (res === undefined) throw new Error(`File ${x} not found`);
        return x;
      })
    }
  }));
  const mapperItems = new Map<string, HeaderItem>(items.map(x => [x.label, x]));

  context.subscriptions.push(
    vscode.commands.registerCommand('mp-oi-library.chooseInclude', async () => {
      const editor = vscode.window.activeTextEditor;
      if (editor === undefined) {
        vscode.window.showErrorMessage('No active file');
        return;
      }
      const fileContent = editor.document.getText();
      const lastSelection: HeaderItem[] = getIncludeFromFile(fileContent)
        .map(file => mapperItems.get(file))
        .filter(x => x !== undefined);

      const pick = vscode.window.createQuickPick<HeaderItem>();
      pick.canSelectMany = true;
      pick.items = items;
      pick.selectedItems = Array.from(lastSelection);
      pick.show();
      pick.onDidChangeSelection(
        () =>
          (pick.selectedItems = Array.from(
            new Set([
              ...pick.selectedItems,
              ...pick.selectedItems.flatMap(item =>
                item.data.dependencies.map(
                  dependency => mapperItems.get(dependency)!
                )
              )
            ])
          ))
      );
      pick.onDidAccept(() => {
        pick.dispose();
        editor
          .edit(editBulder => {
            const v = writeIncludeToFile(
              fileContent,
              headMessage,
              pick.selectedItems.map(x => ({
                file: x.label,
                content: x.data.content
              }))
            );
            editBulder.replace(
              new vscode.Range(v.start, 0, v.end, 0),
              v.content
            );
          })
          .then(
            x =>
              void (x || vscode.window.showErrorMessage('Failed to write file'))
          );
      });
      pick.onDidHide(() => pick.dispose());
    })
  );
}

/**
 * @returns 存放了文件内容和依赖关系的 Map。保证插入某个文件时依赖的每个文件都已经插入。
 */
async function getProcessedFileContent(dir: string) {
  const fileContent = new Map(
    (await getFilesUnderDir(path.join(dir, 'library', 'mrpython'))).map(
      ([fileName, content]) => {
        const lines = content.split('\n');
        let i = 0;
        const includes = new Array<string>();
        while (i < lines.length) {
          const matchs = includeRegex.exec(lines[i]);
          if (!matchs) break;
          includes.push(matchs[1]);
          ++i;
        }
        return [
          fileName,
          { includes, content: lines.slice(i).join('\n') }
        ] as const;
      }
    )
  );

  const solved = new Map<
    string,
    { content: string; dependencies: Set<string> }
  >();
  function solve(name: string) {
    let dependencies = solved.get(name)?.dependencies;
    if (dependencies) return dependencies;
    const file = fileContent.get(name);
    if (file === undefined) throw new Error(`File ${name} not found`);
    dependencies = new Set<string>();
    file.includes.forEach(
      s => (dependencies.add(s), solve(s).forEach(x => dependencies.add(x)))
    );
    solved.set(name, { content: file.content, dependencies });
    return dependencies;
  }
  fileContent.forEach((_, name) => solve(name));
  return solved;
}
function getFilesUnderDir(dir: string) {
  return Promise.all(
    fs
      .readdirSync(dir)
      .map(s =>
        promisify(fs.readFile)(path.join(dir, s)).then(
          x => [s, x.toString('utf-8')] as const
        )
      )
  );
}

function getIncludeFromFile(content: string) {
  const lines = content.split('\n');
  let start = lines.findIndex(x => x === extensionStartNoteStr),
    end = lines.findIndex(x => x === extensionEndNoteStr);
  if (start === -1 || end === -1) return [];
  const files = new Array<string>();
  for (let i = start + 1; i < end; i++) {
    const matchs = extensionFileNoteRegex.exec(lines[i]);
    if (matchs) files.push(matchs[1]);
  }
  return files;
}

function writeIncludeToFile(
  content: string,
  headMessage: string,
  includes: { file: string; content: string }[]
) {
  const lines = content.split('\n');
  let start = lines.findIndex(x => x === extensionStartNoteStr),
    end = lines.findIndex(x => x === extensionEndNoteStr);
  if (start === -1 || end === -1) ((start = 0), (end = -1));
  ++end;
  return {
    start,
    end,
    content: [
      extensionStartNoteStr,
      extensionHeaderStr + headMessage,
      ...includes.map(
        x => `// mp-oi-library extension note: file ${x.file}\n${x.content}`
      ),
      extensionEndNoteStr,
      ''
    ].join('\n')
  };
}
