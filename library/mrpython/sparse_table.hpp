#include "mrpython/bits.hpp"
#include "mrpython/utility.hpp"

#ifndef MP_LIBRARY_SPARSE_TABLE_HPP
#define MP_LIBRARY_SPARSE_TABLE_HPP
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <vector>

namespace mrpython {
using std::size_t;
/**
 * [ST 表](https://oi-wiki.org/ds/sparse-table/) 基础模板。
 * @tparam T 元素类型
 * @tparam MergeFunction 合并运算函数，对于函数 `F` 应满足 `F(x,x)==x`
 */
template <typename T, typename MergeFunction> class sparse_table {
  std::vector<std::vector<T>> table;
  MergeFunction merge;

 public:
  template <typename InputIterator>
  sparse_table(InputIterator first, InputIterator last,
               MergeFunction const& mergeFun = MergeFunction())
      : merge(mergeFun) {
    table.emplace_back(std::vector<T>(first, last));
    size_t n = table[0].size();
    for (size_t i = 1; ((size_t)1 << i) <= n; ++i) {
      table.emplace_back();
      table[i].reserve(n - (1 << i) + 1);
      for (size_t j = 0; j < n - (1 << i) + 1; ++j)
        table[i].emplace_back(
            merge(table[i - 1][j], table[i - 1][j + (1 << (i - 1))]));
    }
    table.shrink_to_fit();
  }
  T get(size_t l, size_t r) const {
    size_t k = int_log2(r - l);
    return merge(table[k][l], table[k][r - ((size_t)1 << k)]);
  }
};

template <typename T> using sparse_table_min = sparse_table<T, mrpython::min>;
template <typename T> using sparse_table_max = sparse_table<T, mrpython::max>;
template <typename T> using sparse_table_gcd = sparse_table<T, mrpython::gcd>;
template <typename Iterator, typename Comp>
auto gen_iterator_sparse_table(Iterator first, Iterator last,
                               Comp const& comp = Comp()) {
  std::vector<Iterator> dat(std::distance(first, last));
  std::iota(dat.begin(), dat.end(), first);
  return sparse_table<Iterator, compare_iterator<Comp>>(
      dat.begin(), dat.end(), compare_iterator<Comp>(comp));
}
}  // namespace mrpython
#endif  // MP_LIBRARY_SPARSE_TABLE_HPP
