#include <gtest/gtest.h>

#include <cstddef>
#include <mrpython/lazy_segment_tree.hpp>
#include <random>

TEST(lazy_segment_tree, add) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::uniform_int_distribution<unsigned> val_dist(
      std::numeric_limits<unsigned>::min(),
      std::numeric_limits<unsigned>::max()),
      size_dist(0, n - 1), operator_dist(0, 2);
  std::vector<unsigned> a(n);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::lazy_segment_tree_add_add<unsigned> tree(a.begin(), a.end(), 0);
  while (q--) {
    unsigned char op = operator_dist(gen);
    if (op == 0) {
      std::size_t l = size_dist(gen), r = size_dist(gen);
      if (l > r) std::swap(l, r);
      assert(l < r + 1);
      unsigned ans =
          std::accumulate(a.begin() + l, a.begin() + r + 1, (unsigned)0);
      EXPECT_EQ(tree.get(l, r + 1), ans);
    } else if (op == 1) {
      std::size_t l = size_dist(gen), r = size_dist(gen);
      if (l > r) std::swap(l, r);
      assert(l < r + 1);
      unsigned value = val_dist(gen);
      for (std::size_t i = l; i < r + 1; ++i) a[i] += value;
      tree.set(l, r + 1, value);
    } else {
      size_t p = size_dist(gen);
      unsigned value = val_dist(gen);
      a[p] += value;
      tree.set(p, [&](unsigned x) { return x + value; });
    }
  }
}