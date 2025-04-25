#include <gtest/gtest.h>

#include <cstddef>
#include <mrpython/lazy_segment_tree.hpp>
#include <random>

TEST(lazy_segment_tree, find_first_right) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::uniform_int_distribution<unsigned> val_dist(
      std::numeric_limits<unsigned>::min(),
      std::numeric_limits<unsigned>::max()),
      size_dist(0, n - 1), operator_dist(0, 1);
  std::uniform_int_distribution<unsigned long long> q_dist(
      std::numeric_limits<unsigned>::min(),
      (unsigned long long)std::numeric_limits<unsigned>::max() * n);
  std::vector<unsigned> a(n);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::lazy_segment_tree_add_add<unsigned long long> tree(a.begin(),
                                                               a.end(), 0);
  while (q--) {
    if (operator_dist(gen)) {
      std::size_t l = size_dist(gen), r = size_dist(gen);
      if (l > r) std::swap(l, r);
      assert(l < r + 1);
      unsigned long long ans =
          std::accumulate(a.begin() + l, a.begin() + r + 1, 0ull);
      EXPECT_EQ(tree.get(l, r + 1), ans);
    } else {
      std::size_t l = size_dist(gen), v = q_dist(gen);
      size_t p = l;
      unsigned long long s = 0;
      while (p < n) {
        s += a[p];
        if (s > v) break;
        ++p;
      }
      size_t out=tree.find_first_right(l, [&](unsigned long long x) { return x > v; });
      EXPECT_EQ(out, p);
    }
  }
}

TEST(lazy_segment_tree, find_last_left) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::uniform_int_distribution<unsigned> val_dist(
      std::numeric_limits<unsigned>::min(),
      std::numeric_limits<unsigned>::max()),
      size_dist(0, n - 1), operator_dist(0, 1);
  std::uniform_int_distribution<unsigned long long> q_dist(
      std::numeric_limits<unsigned>::min(),
      (unsigned long long)std::numeric_limits<unsigned>::max() * n);
  std::vector<unsigned> a(n);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::lazy_segment_tree_add_add<unsigned long long> tree(a.begin(),
                                                               a.end(), 0);
  while (q--) {
    if (operator_dist(gen)) {
      std::size_t l = size_dist(gen), r = size_dist(gen);
      if (l > r) std::swap(l, r);
      assert(l < r + 1);
      unsigned long long ans =
          std::accumulate(a.begin() + l, a.begin() + r + 1, 0ull);
      EXPECT_EQ(tree.get(l, r + 1), ans);
    } else {
      std::size_t l = size_dist(gen), v = q_dist(gen);
      size_t p = l;
      unsigned long long s = 0;
      while (p < n) {
        s += a[p];
        if (s > v) break;
        --p;
      }
      size_t out=tree.find_last_left(l, [&](unsigned long long x) { return x > v; });
      EXPECT_EQ(out, p);
    }
  }
}
