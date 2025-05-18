#include "mrpython/numeric.hpp"

#ifndef MP_LIBRARY_MODINT_HPP
#define MP_LIBRARY_MODINT_HPP
#include <iostream>
#include <type_traits>

namespace mrpython {
template <unsigned P> class modint {
  static_assert(P > 0, "modint: P must be positive");
  static_assert(P < (1U << 31), "modint: P must be less than 2^31");

  unsigned int v;

 public:
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  constexpr modint(T x)
      : v((std::is_unsigned<T>::value
               ? static_cast<unsigned int>(x % static_cast<T>(P))
               : static_cast<unsigned int>((x % static_cast<T>(P) + P) % P))) {}
  constexpr modint(void): v(0) {}
  constexpr modint& operator+=(modint x) {
    if ((this->v += x.v) >= P) this->v -= P;
    return *this;
  }
  constexpr modint& operator-=(modint x) {
    if (this->v < x.v) this->v += P;
    this->v -= x.v;
    return *this;
  }
  constexpr modint& operator*=(unsigned int x) {
    this->v = (unsigned long long int)this->v * x % P;
    return *this;
  }
  template <typename T,
            typename = std::enable_if_t<std::is_integral<T>::value &&
                                        !std::is_same<T, unsigned int>::value>>
  constexpr modint& operator*=(T x) {
    return *this *= modint(x);
  }
  constexpr modint& operator*=(modint x) {
    this->v = (unsigned long long int)this->v * x.v % P;
    return *this;
  }
  constexpr modint& operator/=(modint x) { return *this *= x.inv(); }
  constexpr friend modint operator+(modint a, modint b) {
    a += b;
    return a;
  }
  constexpr friend modint operator-(modint a, modint b) {
    a -= b;
    return a;
  }
  template <typename T,
            typename = std::enable_if_t<std::is_same<T, modint>::value ||
                                        std::is_integral<T>::value>>
  constexpr friend modint operator*(modint a, T b) {
    a *= b;
    return a;
  }
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  constexpr friend modint operator*(T a, modint b) {
    b *= a;
    return b;
  }
  constexpr friend modint operator/(modint a, modint b) { return a /= b; }
  constexpr modint operator-(void) const { return modint() - *this; }
  constexpr unsigned int val(void) const { return v; }
  constexpr friend std::ostream& operator<<(std::ostream& out, modint x) {
    return out << x.v;
  }
  constexpr modint inv(void) const { return mrpython::fastPow(*this, P - 2); }
  constexpr modint pow(unsigned long long int n) const {
    return mrpython::fastPow(*this, n);
  }
  static constexpr unsigned modval() { return P; }
};
}  // namespace mrpython
#endif  // MP_LIBRARY_MODINT_HPP
