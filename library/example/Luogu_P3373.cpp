// mp-oi-library extension note: start. Do not modify this part.
// mp-oi-library extension header: library version: @MrPython-0.5.1
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
// mp-oi-library extension note: file segment_tree_utility.hpp
#ifndef MP_LIBRARY_SEGMENT_TREE_UTILITY_HPP
#define MP_LIBRARY_SEGMENT_TREE_UTILITY_HPP
#include <cstddef>
#include <utility>
namespace mrpython {
template <typename F, typename T>
auto call_merge_impl(F const& f, T const& lv, std::size_t ls, T const& rv,
                     std::size_t rs, int) -> decltype(f(lv, ls, rv, rs)) {
  return f(lv, ls, rv, rs);
}
template <typename F, typename T>
auto call_merge_impl(F const& f, T const& lv, std::size_t, T const& rv,
                     std::size_t, long) -> decltype(f(lv, rv)) {
  return f(lv, rv);
}
template <typename F, typename T>
decltype(auto) call_merge(F const& f, T const& lv, std::size_t ls, T const& rv,
                          std::size_t rs) {
  return call_merge_impl(f, lv, ls, rv, rs, 0);
}
}  // namespace mrpython
#endif  // MP_LIBRARY_SEGMENT_TREE_UTILITY_HPP

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

// mp-oi-library extension note: end
#include <bits/stdc++.h>
using namespace std;
istream& fin = cin;
ostream& fout = cout;
using ui = unsigned int;
using uli = unsigned long long int;
using li = long long int;
template <ui P> class modint {
  ui v;

 public:
  modint(uli x): v(x % P) {}
  modint(void): v() {}
  modint& operator+=(modint x) {
    if ((this->v += x.v) >= P) this->v -= P;
    return *this;
  }
  modint& operator-=(modint x) {
    if (((this->v += P) -= x.v) >= P) this->v -= P;
    return *this;
  }
  modint& operator*=(ui x) {
    this->v = (uli)this->v * x % P;
    return *this;
  }
  modint& operator*=(modint x) {
    this->v = (uli)this->v * x.v % P;
    return *this;
  }
  friend modint operator+(modint a, modint b) {
    a += b;
    return a;
  }
  friend modint operator-(modint a, modint b) {
    a -= b;
    return a;
  }
  friend modint operator*(modint a, ui b) {
    a *= b;
    return a;
  }
  friend modint operator*(modint a, modint b) {
    a *= b;
    return a;
  }
  friend ostream& operator<<(ostream& out, modint x) { return out << x.v; }
};
using mi = modint<571373>;
struct Node {
  using T = mi;
  struct Lazy {
    mi time, add;
  };
  static plus<T> merge_data;
  static Lazy merge_lazy(Lazy const& x, Lazy const& y) {
    return {x.time * y.time, x.add * y.time + y.add};
  }
  static T operate(Lazy const& x, T const& v, size_t sz) {
    return v * x.time + x.add * sz;
  }
};
plus<mi> Node::merge_data;
int main(void) {
  ios::sync_with_stdio(false), cin.tie(nullptr), cout.tie(nullptr);
  size_t n, q;
  ui _m;
  fin >> n >> q >> _m;
  vector<ui> a(n);
  for (ui& i : a) fin >> i;
  mrpython::lazy_segment_tree_from_node<Node>::type tree(a.begin(), a.end(),
                                                         {1, 0});
  while (q--) {
    char c;
    size_t l, r;
    fin >> c >> l >> r;
    --l;
    if (c == '1') {
      ui k;
      fin >> k;
      tree.set(l, r, {k, 0});
    }
    if (c == '2') {
      ui k;
      fin >> k;
      tree.set(l, r, {1, k});
    }
    if (c == '3') fout << tree.get(l, r) << '\n';
  }
  return 0;
}