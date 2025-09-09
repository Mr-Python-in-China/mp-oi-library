// mp-oi-library extension note: start. Do not modify this part.
// mp-oi-library extension header: library version: @MrPython-0.4.2
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

// mp-oi-library extension note: file utility.hpp
#ifndef MP_LIBRARY_UTILITY_HPP
#define MP_LIBRARY_UTILITY_HPP
#include <algorithm>
#include <functional>

namespace mrpython {
struct max {
  template <typename T> T const& operator()(T const& a, T const& b) const {
    return std::max(a, b);
  }
  template <typename T, typename Cmp>
  T const& operator()(T const& a, T const& b, Cmp cmp) const {
    return std::max(a, b, cmp);
  }
};
struct min {
  template <typename T> T const& operator()(T const& a, T const& b) const {
    return std::min(a, b);
  }
  template <typename T, typename Cmp>
  T const& operator()(T const& a, T const& b, Cmp cmp) const {
    return std::min(a, b, cmp);
  }
};
struct gcd {
  template <typename T> T operator()(T a, T b) const {
    while (b != 0) {
      T t = b;
      b = a % b;
      a = t;
    }
    return a;
  }
};
template <typename Comp> struct compare_iterator {
  Comp comp;
  compare_iterator(Comp const& c): comp(c) {}
  template <typename Iterator>
  Iterator operator()(Iterator a, Iterator b) const {
    return comp(*a, *b) ? a : b;
  }
};
template <typename T> struct const_function {
  T const value;
  const_function(T const& v): value(v) {}
  template <typename... Args> T operator()(Args...) const { return value; }
};
}  // namespace mrpython
#endif  // MP_LIBRARY_UTILITY_HPP

// mp-oi-library extension note: file typical_segment_tree.hpp

#ifndef MP_LIBRARY_SEGMENT_TREE_HPP
#define MP_LIBRARY_SEGMENT_TREE_HPP
#include <algorithm>
#include <functional>
#include <utility>
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
  auto doMerge(std::pair<T const&, size_t> l, std::pair<T const&, size_t> r) {
    return std::make_pair(
        call_merge(merge, l.first, l.second, r.first, r.second),
        l.second + r.second);
  }
  decltype(auto) doMerge(T const& lv, size_t ls, T const& rv, size_t rs) {
    return call_merge(merge, lv, ls, rv, rs);
  }
  void build(void) {
    data.reserve(2 * n - 1), size.reserve(2 * n - 1);
    for (size_t i = n; i < 2 * n - 1; ++i) {
      size_t d = 2 * n - 1 - i;
      size_t l = d * 2, r = d * 2 + 1;
      data.emplace_back(doMerge(data[2 * n - 1 - l], size[2 * n - 1 - l],
                                 data[2 * n - 1 - r], size[2 * n - 1 - r]));
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
    data[pos] = doMerge(data[pos * 2 + 1], size[pos * 2 + 1],
                         data[pos * 2 + 2], size[pos * 2 + 2]);
  }
  std::pair<T, size_t> get_impl(size_t l, size_t r, size_t pos) {
    if (l == 0 && r == size[pos]) return {data[pos], size[pos]};
    size_t m = size[pos * 2 + 1];
    if (l < m && r > m)
      return doMerge(get_impl(l, m, pos * 2 + 1),
                      get_impl(0, r - m, pos * 2 + 2));
    else if (l < m)
      return get_impl(l, r, pos * 2 + 1);
    else if (r > m)
      return get_impl(l - m, r - m, pos * 2 + 2);
    else
      __builtin_unreachable();
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
  typical_segment_tree(InputIterator first, InputIterator last,
                       MergeFunction const& mergeFun = MergeFunction())
      : data(first, last),
        size(data.size(), 1),
        n(data.size()),
        merge(mergeFun) {
    rotate(data.begin(),
           data.begin() + ((2 * n - 1) - (highbit(2 * n - 1) - 1)), data.end());
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
  T get(size_t l, size_t r) { return get_impl(l, r, 0).first; }
  T getd(size_t l, size_t r, T const& e = {}) { return l == r ? e : get(l, r); }
  template <typename Check>
  size_t find_first_right(size_t l, Check const& check) {
    if (l >= n) return l;
    l = data_id_to_node_id(l);
    while (l % 2 == 1) l /= 2;
    while (l < 2 * n - 1 && check(data[l])) l = l * 2 + 1;
    if (l >= 2 * n - 1) return node_id_to_data_id(l / 2);
    std::pair<T, size_t> v = {data[l], size[l]};
    while (true) {
      ++l;
      if (!(l & (l + 1))) return n;
      while (l % 2 == 1) l /= 2;
      std::pair<T, size_t> vl = doMerge(v, {data[l], size[l]});
      if (check(vl.first)) break;
      v = std::move(vl);
    }
    while (l < n - 1) {
      std::pair<T, size_t> vl = doMerge(v, {data[l * 2 + 1], size[l * 2 + 1]});
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
    r = data_id_to_node_id(r);
    while (r && r % 2 == 0) r = (r - 1) / 2;
    while (r < 2 * n - 1 && check(data[r])) r = r * 2 + 2;
    if (r >= 2 * n - 1) return node_id_to_data_id((r - 1) / 2);
    std::pair<T, size_t> v = {data[r], size[r]};
    while (true) {
      if (!(r & (r + 1))) return -1;
      --r;
      while (r % 2 == 0) r = (r - 1) / 2;
      std::pair<T, size_t> vl = doMerge(v, {data[r], size[r]});
      if (check(vl.first)) break;
      v = std::move(vl);
    }
    while (r < n - 1) {
      std::pair<T, size_t> vr = doMerge(v, {data[r * 2 + 2], size[r * 2 + 2]});
      if (!check(vr.first))
        r = r * 2 + 1, v = std::move(vr);
      else
        r = r * 2 + 2;
    }
    return node_id_to_data_id(r);
  }
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
  mrpython::typical_segment_tree_add<li> tree(n, 0);
  for (size_t i = 0; i < n; ++i) {
    ui x;
    fin >> x;
    tree.set(i, [x](li) { return x; });
  }
  while (m--) {
    char c;
    fin >> c;
    if (c == '1') {
      size_t x;
      li k;
      fin >> x >> k;
      tree.set(x - 1, [k](li x) { return x + k; });
    } else if (c == '2') {
      size_t l, r;
      fin >> l >> r;
      fout << tree.get(--l, r) << '\n';
    }
  }
  return 0;
}