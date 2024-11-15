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
// mp-oi-library extension note: file utility.hpp
#ifndef MP_LIBRARY_UTILITY_HPP
#define MP_LIBRARY_UTILITY_HPP
#include <algorithm>
#include <limits>

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
  Comp const comp;
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

// mp-oi-library extension note: file sparse_table.hpp

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
               MergeFunction mergeFun = MergeFunction())
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
                               Comp comp = Comp()) {
  std::vector<Iterator> dat(std::distance(first, last));
  std::iota(dat.begin(), dat.end(), first);
  return sparse_table<Iterator, compare_iterator<Comp>>(
      dat.begin(), dat.end(), compare_iterator<Comp>(comp));
}
}  // namespace mrpython
#endif  // MP_LIBRARY_SPARSE_TABLE_HPP

// mp-oi-library extension note: end
#include <bits/stdc++.h>
using namespace std;
istream& fin = cin;
ostream& fout = cout;
using ui = unsigned int;
int main(void) {
  ios::sync_with_stdio(false), cin.tie(nullptr), cout.tie(nullptr);
  size_t n, m;
  fin >> n >> m;
  vector<ui> a(n);
  for (ui& i : a) fin >> i;
  mrpython::sparse_table_max<ui> st(a.begin(), a.end());
  while (m--) {
    size_t l, r;
    fin >> l >> r;
    --l;
    fout << st.get(l, r) << '\n';
  }
}
