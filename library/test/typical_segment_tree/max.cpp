#include <gtest/gtest.h>

#include <mrpython/typical_segment_tree.hpp>
#include <numeric>
#include <random>

TEST(typical_segment_tree, max) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::vector<int> a(n);
  std::uniform_int_distribution<int> val_dist(std::numeric_limits<int>::min(),
                                              std::numeric_limits<int>::max());
  std::uniform_int_distribution<std::size_t> size_dist(0, n - 1),
      operator_dist(0, 1);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::typical_segment_tree_max<int> tree(a.begin(), a.end());
  while (q--) {
    if (operator_dist(gen)) {
      std::size_t l = size_dist(gen), r = size_dist(gen);
      if (l > r) std::swap(l, r);
      assert(l < r + 1);
      int ans = *std::max_element(a.begin() + l, a.begin() + r + 1);
      EXPECT_EQ(tree.get(l, r + 1), ans);
    } else {
      std::size_t target = size_dist(gen);
      int value = val_dist(gen);
      a[target] = value;
      tree.set(target, [value](int) { return value; });
    }
  }
}