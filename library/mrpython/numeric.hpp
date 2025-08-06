#ifndef MP_LIBRARY_NUMERIC_HPP
#define MP_LIBRARY_NUMERIC_HPP

#include <functional>
#include <vector>
namespace mrpython {
template <typename T, typename F = std::multiplies<T>>
constexpr T fastPow(T a, unsigned long long int n, T ans = 1,
                    F const& f = F()) {
  while (n) {
    if (n & 1) ans = f(ans, a);
    a = f(a, a);
    n >>= 1;
  }
  return ans;
}
template <typename T> T frac(unsigned int n) {
  static std::vector<T> mem = {1};
  while (mem.size() <= n) mem.push_back(mem.back() * mem.size());
  return mem[n];
}
template <typename T> T comb(unsigned int n, unsigned int m) {
  return n < m ? 0 : frac<T>(n) / (frac<T>(m) * frac<T>(n - m));
}
}  // namespace mrpython

#endif
