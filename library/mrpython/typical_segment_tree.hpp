#include "mrpython/bits.hpp"
#include "mrpython/utility.hpp"

#ifndef MP_LIBRARY_SEGMENT_TREE_HPP
#define MP_LIBRARY_SEGMENT_TREE_HPP
#include <algorithm>
#include <functional>
#include <vector>

namespace mrpython {
using std::size_t;
/**
 * 一颗 无标记线段树 模板。
 * @tparam T 元素类型
 * @tparam MergeFunction 合并运算函数
 */
template <typename T, typename MergeFunction> class typical_segment_tree {
  std::vector<T> data;
  std::vector<size_t> size;
  size_t n;
  MergeFunction merge;
  void build(void) {
    data.reserve(2 * n - 1), size.reserve(2 * n - 1);
    for (size_t i = n; i < 2 * n - 1; ++i) {
      size_t d = 2 * n - 1 - i;
      size_t l = d * 2, r = d * 2 + 1;
      data.emplace_back(merge(data[2 * n - 1 - l], data[2 * n - 1 - r]));
      size.emplace_back(size[2 * n - 1 - l] + size[2 * n - 1 - r]);
    }
    std::reverse(data.begin(), data.end());
    std::reverse(size.begin(), size.end());
  }
  template <typename Operate>
  void set_impl(size_t c, Operate const& operate, size_t pos) {
    if (size[pos] == 1) {
      data[pos] = operate((T const&)data[pos]);
      return;
    }
    size_t m = size[pos * 2 + 1];
    if (c < m)
      set_impl(c, operate, pos * 2 + 1);
    else
      set_impl(c - m, operate, pos * 2 + 2);
    data[pos] = merge(data[pos * 2 + 1], data[pos * 2 + 2]);
  }
  T get_impl(size_t l, size_t r, size_t pos) {
    if (l == 0 && r == size[pos]) return data[pos];
    size_t m = size[pos * 2 + 1];
    if (l < m && r > m)
      return merge(get_impl(l, m, pos * 2 + 1),
                   get_impl(0, r - m, pos * 2 + 2));
    else if (l < m)
      return get_impl(l, r, pos * 2 + 1);
    else if (r > m)
      return get_impl(l - m, r - m, pos * 2 + 2);
    else
      __builtin_unreachable();
  }

 public:
  template <typename InputIterator>
  typical_segment_tree(InputIterator first, InputIterator last,
                       MergeFunction const& mergeFun = MergeFunction())
      : data(first, last),
        size(data.size(), 1),
        n(data.size()),
        merge(mergeFun) {
    rotate(data.begin(), data.begin() + (2 * n - 1) - (highbit(2 * n - 1) - 1),
           data.end());
    reverse(data.begin(), data.end());
    build();
  }
  typical_segment_tree(size_t len, T const& init,
                       MergeFunction const& mergeFun = MergeFunction())
      : data(len, init), size(len, 1), n(len), merge(mergeFun) {
    build();
  }
  /**
   * 单点修改操作
   * @param target 修改的位置
   * @param operate 更新该点的函数
   */
  template <typename Fun> void set(size_t target, Fun const& operate) {
    set_impl(target, operate, 0);
  }
  T get(size_t l, size_t r) { return get_impl(l, r, 0); }
};
template <typename T>
using typical_segment_tree_add = typical_segment_tree<T, std::plus<T>>;
template <typename T>
using typical_segment_tree_max = typical_segment_tree<T, max>;
template <typename T>
using typical_segment_tree_min = typical_segment_tree<T, min>;

template <typename NodeStruct> class typical_segment_tree_from_node {
  using T = typename NodeStruct::T;
  struct MergeFunction {
    T operator()(T const& a, T const& b) const {
      return NodeStruct::merge_data(a, b);
    }
  };
  typical_segment_tree_from_node() = delete;

 public:
  using type = typical_segment_tree<T, MergeFunction>;
};
}  // namespace mrpython
#endif  // MP_LIBRARY_SEGMENT_TREE_HPP
