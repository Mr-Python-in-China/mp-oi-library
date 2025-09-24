#include "mrpython/bits.hpp"
#include "mrpython/segment_tree_utility.hpp"

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
  Lazy lazyInit;
  MergeFunction mergeData;
  OperateFunction operate;
  MergeLazyFunction mergeLazy;
  auto doMergeData(std::pair<T const&, size_t> l,
                   std::pair<T const&, size_t> r) {
    return std::make_pair(
        call_merge(mergeData, l.first, l.second, r.first, r.second),
        l.second + r.second);
  }
  decltype(auto) doMergeData(T const& lv, size_t ls, T const& rv, size_t rs) {
    return call_merge(mergeData, lv, ls, rv, rs);
  }
  void build(void) {
    data.reserve(2 * n - 1), size.reserve(2 * n - 1);
    for (size_t i = n; i < 2 * n - 1; ++i) {
      size_t d = 2 * n - 1 - i;
      size_t l = d * 2, r = d * 2 + 1;
      data.emplace_back(doMergeData(data[2 * n - 1 - l], size[2 * n - 1 - l],
                                    data[2 * n - 1 - r], size[2 * n - 1 - r]));
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
  std::pair<T, size_t> get_impl(size_t l, size_t r, size_t pos) {
    if (l == 0 && r == size[pos]) return {data[pos], size[pos]};
    pushdown(pos);
    size_t m = size[pos * 2 + 1];
    if (l < m && r > m)
      return doMergeData(get_impl(l, m, pos * 2 + 1),
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
    data[pos] = doMergeData(data[pos * 2 + 1], size[pos * 2 + 1],
                            data[pos * 2 + 2], size[pos * 2 + 2]);
  }
  template <typename Operate>
  void set_impl(size_t c, Operate const& opv, size_t pos) {
    if (size[pos] == 1) {
      data[pos] = opv((T const&)data[pos]);
      return;
    }
    pushdown(pos);
    size_t m = size[pos * 2 + 1];
    if (c < m)
      set_impl(c, opv, pos * 2 + 1);
    else
      set_impl(c - m, opv, pos * 2 + 2);
    data[pos] = doMergeData(data[pos * 2 + 1], size[pos * 2 + 1],
                            data[pos * 2 + 2], size[pos * 2 + 2]);
  }
  size_t data_id_to_node_id(size_t x) {
    x += n - ((2 * n - 1) - (highbit(2 * n - 1) - 1));
    if (x >= n) x -= n;
    x += n - 1;
    return x;
  }
  size_t node_id_to_data_id(size_t x) {
    x -= n - 1;
    x += ((2 * n - 1) - (highbit(2 * n - 1) - 1));
    if (x >= n) x -= n;
    return x;
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
    rotate(data.begin(),
           data.begin() + ((2 * n - 1) - (highbit(2 * n - 1) - 1)), data.end());
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
  T get(size_t l, size_t r) { return get_impl(l, r, 0).first; }
  T getd(size_t l, size_t r, T const& e = {}) { return l == r ? e : get(l, r); }
  void set(size_t l, size_t r, Lazy const& operateVal) {
    if (l == r) return;
    set_impl(l, r, operateVal, 0);
  }
  template <typename Operate> void set(size_t p, Operate const& opv) {
    set_impl(p, opv, 0);
  }
  template <typename Check>
  size_t find_first_right(size_t l, Check const& check) {
    if (l >= n) return l;
    set(l, [](T const& _) { return _; });  // 将一路上的节点都 pushdown 下去
    l = data_id_to_node_id(l);
    while (l % 2 == 1) l /= 2;
    while (l < 2 * n - 1 && check(data[l])) l = l * 2 + 1;
    if (l >= 2 * n - 1) return node_id_to_data_id(l / 2);
    std::pair<T, size_t> v = {data[l], size[l]};
    while (true) {
      ++l;
      if (!(l & (l + 1))) return n;
      while (l % 2 == 1) l /= 2;
      std::pair<T, size_t> vl = doMergeData(v, {data[l], size[l]});
      if (check(vl.first)) break;
      v = std::move(vl);
    }
    while (l < n - 1) {
      pushdown(l);
      std::pair<T, size_t> vl =
          doMergeData(v, {data[l * 2 + 1], size[l * 2 + 1]});
      if (!check(vl.first))
        l = l * 2 + 2, v = std::move(vl);
      else
        l = l * 2 + 1;
    }
    return node_id_to_data_id(l);
  }
  template <typename Check>
  size_t find_last_left(size_t r, Check const& check) {
    if (r >= n) return r;
    set(r, [](T const& _) { return _; });  // 将一路上的节点都 pushdown 下去
    r = data_id_to_node_id(r);
    while (r && r % 2 == 0) r = (r - 1) / 2;
    while (r < 2 * n - 1 && check(data[r])) r = r * 2 + 2;
    if (r >= 2 * n - 1) return node_id_to_data_id((r - 1) / 2);
    std::pair<T, size_t> v = {data[r], size[r]};
    while (true) {
      if (!(r & (r + 1))) return -1;
      --r;
      while (r % 2 == 0) r = (r - 1) / 2;
      std::pair<T, size_t> vl = doMergeData(v, {data[r], size[r]});
      if (check(vl.first)) break;
      v = std::move(vl);
    }
    while (r < n - 1) {
      pushdown(r);
      std::pair<T, size_t> vr =
          doMergeData(v, {data[r * 2 + 2], size[r * 2 + 2]});
      if (!check(vr.first))
        r = r * 2 + 1, v = std::move(vr);
      else
        r = r * 2 + 2;
    }
    return node_id_to_data_id(r);
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

template <typename NodeStruct> class lazy_segment_tree_from_node {
  using T = typename NodeStruct::T;
  using Lazy = typename NodeStruct::Lazy;
  struct MergeFunction {
    template <typename... Args>
    auto operator()(Args&&... args) const
        -> decltype(NodeStruct::merge_data(std::forward<Args>(args)...)) {
      return NodeStruct::merge_data(std::forward<Args>(args)...);
    }
  };
  struct OperateFunction {
    template <typename... Args>
    auto operator()(Args&&... args) const
        -> decltype(NodeStruct::operate(std::forward<Args>(args)...)) {
      return NodeStruct::operate(std::forward<Args>(args)...);
    }
  };
  struct MergeLazyFunction {
    template <typename... Args>
    auto operator()(Args&&... args) const
        -> decltype(NodeStruct::merge_lazy(std::forward<Args>(args)...)) {
      return NodeStruct::merge_lazy(std::forward<Args>(args)...);
    }
  };
  lazy_segment_tree_from_node() = delete;

 public:
  using type = lazy_segment_tree<T, MergeFunction, Lazy, OperateFunction,
                                 MergeLazyFunction>;
};
}  // namespace mrpython
#endif
