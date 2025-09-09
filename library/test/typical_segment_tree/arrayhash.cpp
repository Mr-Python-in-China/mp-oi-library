#include <gtest/gtest.h>

#include <cassert>
#include <mrpython/typical_segment_tree.hpp>
#include <random>
#include <vector>

const unsigned long long MOD = (unsigned long long)1e18 + 9;
const unsigned long long W = 1e9;

struct WBaseHashMerge {
  std::vector<unsigned long long> poW;

  WBaseHashMerge(size_t sz): poW(sz + 1) {
    poW[0] = 1;
    for (size_t i = 1; i <= sz; ++i) {
      poW[i] = (__uint128_t)poW[i - 1] * W % MOD;
    }
  }
  unsigned long long operator()(unsigned long long lval, size_t lsize,
                                unsigned long long rval, size_t rsize) const {
    return ((__uint128_t)lval * poW[rsize] + rval) % MOD;
  }
};

long long bruteForceHash(const std::vector<unsigned>& a, size_t l, size_t r,
                         const WBaseHashMerge& merger) {
  if (l >= r) return 0;
  long long v = a[l] % MOD;
  for (size_t i = l + 1; i < r; ++i) v = ((__uint128_t)v * W + a[i]) % MOD;
  return v;
}

TEST(typical_segment_tree, arrayhash) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 1000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 1000}(gen);

  std::vector<unsigned> a(n);
  std::uniform_int_distribution<unsigned> valDist(0, W - 1);
  std::uniform_int_distribution<std::size_t> szDist(0, n - 1), opDist(0, 1);
  std::generate(a.begin(), a.end(), [&] { return valDist(gen); });
  WBaseHashMerge merger(n);
  mrpython::typical_segment_tree<unsigned long long, WBaseHashMerge> tree(
      a.begin(), a.end(), merger);

  while (q--) {
    if (opDist(gen)) {
      std::size_t l = szDist(gen), r = szDist(gen);
      if (l > r) std::swap(l, r);
      ++r;
      unsigned long long tx = tree.get(l, r);
      unsigned long long bx = bruteForceHash(a, l, r, merger);
      EXPECT_EQ(tx, bx);
    } else {
      std::size_t p = szDist(gen);
      unsigned long long x = valDist(gen);
      a[p] = x;
      tree.set(p, [x](unsigned long long) { return x; });
    }
  }
}
