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
