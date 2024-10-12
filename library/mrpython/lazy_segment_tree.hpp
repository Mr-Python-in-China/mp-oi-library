#include "mrpython/bits.hpp"

#ifndef MP_LIBRARY_LAZY_SEGMENT_TREE_HPP
#define MP_LIBRARY_LAZY_SEGMENT_TREE_HPP
#include <algorithm>
#include <functional>
#include <vector>

namespace mrpython {
using std::size_t;
template <typename T, typename MergeFunction, typename Lazy,
          typename OperateFunction, typename MergeLazyFunction>
class lazy_segment_tree {
  std::vector<T> data;
  std::vector<Lazy> lazy;
  std::vector<size_t> size;
  size_t n;
  Lazy const lazyInit;
  MergeFunction const mergeData;
  OperateFunction const operate;
  MergeLazyFunction const mergeLazy;
  void build(void) {
    data.reserve(2 * n - 1), size.reserve(2 * n - 1);
    for (size_t i = n; i < 2 * n - 1; ++i) {
      size_t d = 2 * n - 1 - i;
      size_t l = d * 2 + 1, r = d * 2;
      data.emplace_back(mergeData(data[2 * n - 1 - l], data[2 * n - 1 - r]));
      size.emplace_back(size[2 * n - 1 - l] + size[2 * n - 1 - r]);
    }
    std::reverse(data.begin(), data.end());
    std::reverse(size.begin(), size.end());
  }
  void pushdown(size_t pos) {
    if (size[pos] == 1) {
      lazy[pos] = lazyInit;
      return;
    }
    add_tag_for_node(2 * pos + 1, lazy[pos]);
    add_tag_for_node(2 * pos + 2, lazy[pos]);
    lazy[pos] = lazyInit;
  }
  void add_tag_for_node(size_t pos, Lazy const& lazyVal) {
    data[pos] = operate(lazyVal, data[pos], size[pos]);
    lazy[pos] = mergeLazy(lazy[pos], lazyVal);
  }
  T get_impl(size_t l, size_t r, size_t pos) {
    if (l == 0 && r == size[pos]) return data[pos];
    pushdown(pos);
    size_t m = size[pos * 2 + 1];
    if (l < m && r > m)
      return mergeData(get_impl(l, m, pos * 2 + 1),
                       get_impl(0, r - m, pos * 2 + 2));
    else if (l < m)
      return get_impl(l, r, pos * 2 + 1);
    else if (r > m)
      return get_impl(l - m, r - m, pos * 2 + 2);
    else
      __builtin_unreachable();
  }
  void set_impl(size_t l, size_t r, Lazy const& operateVal, size_t pos) {
    if (l == 0 && r == size[pos]) return add_tag_for_node(pos, operateVal);
    pushdown(pos);
    size_t m = size[pos * 2 + 1];
    if (l < m && r > m)
      set_impl(l, m, operateVal, pos * 2 + 1),
          set_impl(0, r - m, operateVal, pos * 2 + 2);
    else if (l < m)
      set_impl(l, r, operateVal, pos * 2 + 1);
    else if (r > m)
      set_impl(l - m, r - m, operateVal, pos * 2 + 2);
    else
      __builtin_unreachable();
    data[pos] = mergeData(data[pos * 2 + 1], data[pos * 2 + 2]);
  }

 public:
  template <typename InputIterator>
  lazy_segment_tree(InputIterator first, InputIterator last,
                    Lazy const& lazyInitVal,
                    MergeFunction const& mergeDataFun = MergeFunction(),
                    OperateFunction const& OperateFun = OperateFunction(),
                    MergeLazyFunction const& mergeTagFun = MergeLazyFunction())
      : data(first, last),
        lazy(2 * data.size() - 1, lazyInitVal),
        size(data.size(), 1),
        n(data.size()),
        lazyInit(lazyInitVal),
        mergeData(mergeDataFun),
        operate(OperateFun),
        mergeLazy(mergeTagFun) {
    rotate(data.begin(), data.begin() + (2 * n - 1) - (highbit(2 * n - 1) - 1),
           data.end());
    reverse(data.begin(), data.end());
    build();
  }
  lazy_segment_tree(size_t len, T const& init, Lazy const& lazyInitVal,
                    MergeFunction const& mergeDataFun = MergeFunction(),
                    OperateFunction const& OperateFun = OperateFunction(),
                    MergeLazyFunction const& mergeTagFun = MergeLazyFunction())
      : data(len, init),
        lazy(2 * len - 1, lazyInitVal),
        size(len, 1),
        n(len),
        lazyInit(lazyInitVal),
        mergeData(mergeDataFun),
        operate(OperateFun),
        mergeLazy(mergeTagFun) {
    build();
  }
  T get(size_t l, size_t r) { return get_impl(l, r, 0); }
  void set(size_t l, size_t r, Lazy const& operateVal) {
    set_impl(l, r, operateVal, 0);
  }
};
template <typename T> struct lazy_segment_tree_add_add_operate_function {
  T operator()(T const& lazy, T const& data, size_t size) const {
    return data + lazy * size;
  }
};
template <typename T>
using lazy_segment_tree_add_add =
    lazy_segment_tree<T, std::plus<T>, T,
                      lazy_segment_tree_add_add_operate_function<T>,
                      std::plus<T>>;
}  // namespace mrpython
#endif
