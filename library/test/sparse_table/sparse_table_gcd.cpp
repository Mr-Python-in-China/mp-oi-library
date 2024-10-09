#include <gtest/gtest.h>

#include <mrpython/sparse_table.hpp>
#include <numeric>
#include <random>

TEST(sparse_table, gcd) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::vector<unsigned int> a(n);
  std::uniform_int_distribution<unsigned int> val_dist(
      std::numeric_limits<unsigned int>::min(),
      std::numeric_limits<unsigned int>::max()),
      size_dist(0, n - 1);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::sparse_table_gcd<unsigned int> st(a.begin(), a.end());
  while (q--) {
    std::size_t l = size_dist(gen), r = size_dist(gen);
    if (l > r) std::swap(l, r);
    assert(l < r + 1);
    unsigned int ans = std::accumulate(a.begin() + l, a.begin() + r + 1, a[l],
                                       mrpython::gcd());
    EXPECT_EQ(st.get(l, r + 1), ans);
  }
}
