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
  Comp comp;
  compare_iterator(Comp const& c): comp(c) {}
  template <typename Iterator>
  Iterator operator()(Iterator a, Iterator b) const {
    return comp(*a, *b) ? a : b;
  }
};
}  // namespace mrpython
#endif  // MP_LIBRARY_UTILITY_HPP
