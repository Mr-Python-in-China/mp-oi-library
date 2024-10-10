#include <gtest/gtest.h>

#include <mrpython/typical_segment_tree.hpp>
#include <numeric>
#include <random>

TEST(typical_segment_tree, add) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::vector<unsigned> a(n);
  std::uniform_int_distribution<unsigned> val_dist(
      std::numeric_limits<unsigned>::min(),
      std::numeric_limits<unsigned>::max()),
      size_dist(0, n - 1), operator_dist(0, 1);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::typical_segment_tree_add<unsigned> tree(a.begin(), a.end());
  while (q--) {
    if (operator_dist(gen)) {
      std::size_t l = size_dist(gen), r = size_dist(gen);
      if (l > r) std::swap(l, r);
      assert(l < r + 1);
      int ans = std::accumulate(a.begin() + l, a.begin() + r + 1, (unsigned)0);
      EXPECT_EQ(tree.get(l, r + 1), ans);
    } else {
      std::size_t target = size_dist(gen);
      unsigned value = val_dist(gen);
      a[target] += value;
      tree.set(target, [value](unsigned x) { return x + value; });
    }
  }
}