// mp-oi-library extension note: start. Do not modify this part.
// mp-oi-library extension header: library version: @MrPython-0.2.6
// mp-oi-library extension note: file bits.hpp
#ifndef MP_LIBRARY_BITS
#define MP_LIBRARY_BITS
#include <limits>
namespace mrpython {
unsigned countl_zero(unsigned x) { return __builtin_clz(x); }
unsigned countl_zero(unsigned long x) { return __builtin_clzl(x); }
unsigned countl_zero(unsigned long long x) { return __builtin_clzll(x); }
unsigned countr_zero(unsigned x) { return __builtin_ctz(x); }
unsigned countr_zero(unsigned long x) { return __builtin_ctzl(x); }
unsigned countr_zero(unsigned long long x) { return __builtin_ctzll(x); }
unsigned int_log2(unsigned n) {
  return std::numeric_limits<unsigned>::digits - 1 - countl_zero(n);
}
unsigned long int_log2(unsigned long n) {
  return std::numeric_limits<unsigned long>::digits - 1 - countl_zero(n);
}
unsigned long long int_log2(unsigned long long n) {
  return std::numeric_limits<unsigned long long>::digits - 1 - countl_zero(n);
}
unsigned lowbit(unsigned x) { return x & -x; }
unsigned long lowbit(unsigned long x) { return x & -x; }
unsigned long long lowbit(unsigned long long x) { return x & -x; }
unsigned highbit(unsigned x) { return (unsigned)1 << int_log2(x); }
unsigned long highbit(unsigned long x) {
  return (unsigned long)1 << int_log2(x);
}
unsigned long long highbit(unsigned long long x) {
  return (unsigned long long)1 << int_log2(x);
}
}  // namespace mrpython
#endif
// mp-oi-library extension note: file lazy_segment_tree.hpp

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
  void build(void) {
    data.reserve(2 * n - 1), size.reserve(2 * n - 1);
    for (size_t i = n; i < 2 * n - 1; ++i) {
      size_t d = 2 * n - 1 - i;
      size_t l = d * 2, r = d * 2 + 1;
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

template <typename NodeStruct> class lazy_segment_tree_from_node {
  using T = typename NodeStruct::T;
  using Lazy = typename NodeStruct::Lazy;
  struct MergeFunction {
    T operator()(T const& a, T const& b) const {
      return NodeStruct::merge_data(a, b);
    }
  };
  struct OperateFunction {
    T operator()(Lazy const& lazy, T const& data, size_t size) const {
      return NodeStruct::operate(lazy, data, size);
    }
  };
  struct MergeLazyFunction {
    Lazy operator()(Lazy const& a, Lazy const& b) const {
      return NodeStruct::merge_lazy(a, b);
    }
  };
  lazy_segment_tree_from_node() = delete;

 public:
  using type = lazy_segment_tree<T, MergeFunction, Lazy, OperateFunction,
                                 MergeLazyFunction>;
};
}  // namespace mrpython
#endif

// mp-oi-library extension note: end
#include <bits/stdc++.h>
using namespace std;
istream& fin = cin;
ostream& fout = cout;
using ui = unsigned int;
using uli = unsigned long long int;
using li = long long int;
int main(void) {
  ios::sync_with_stdio(false), cin.tie(nullptr), cout.tie(nullptr);
  size_t n, m;
  fin >> n >> m;
  vector<li> a(n);
  for (li& i : a) fin >> i;
  mrpython::lazy_segment_tree_add_add<li> tree(a.begin(), a.end(), 0);
  while (m--) {
    char op;
    size_t x, y;
    fin >> op >> x >> y;
    --x;
    if (op == '1') {
      size_t k;
      fin >> k;
      tree.set(x, y, k);
    }
    if (op == '2') { fout << tree.get(x, y) << '\n'; }
  }
  return 0;
}