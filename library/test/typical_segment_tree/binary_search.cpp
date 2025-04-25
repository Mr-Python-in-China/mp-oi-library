#include <gtest/gtest.h>

#include <cstddef>
#include <mrpython/typical_segment_tree.hpp>
#include <random>

TEST(typicaL_segment_tree, find_first_right) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::vector<int> a(n);
  std::uniform_int_distribution<int> val_dist(std::numeric_limits<int>::min(),
                                              std::numeric_limits<int>::max());
  std::uniform_int_distribution<std::size_t> size_dist(0, n),
      operator_dist(0, 1);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::typical_segment_tree_min<int> tree(a.begin(), a.end());
  while (q--) {
    std::size_t l = size_dist(gen);
    int tv = val_dist(gen);
    auto check = [tv](int x) { return x <= tv; };
    size_t ans = std::find_if(a.begin() + l, a.end(), check) - a.begin();
    EXPECT_EQ(tree.find_first_right(l, check), ans);
  }
}

TEST(typicaL_segment_tree, find_last_left) {
  std::mt19937_64 gen(std::random_device{}());
  std::size_t n = std::uniform_int_distribution<std::size_t>{1, 5000}(gen),
              q = std::uniform_int_distribution<std::size_t>{1, 5000}(gen);
  std::vector<int> a(n);
  std::uniform_int_distribution<int> val_dist(std::numeric_limits<int>::min(),
                                              std::numeric_limits<int>::max());
  std::uniform_int_distribution<std::size_t> size_dist(0, n),
      operator_dist(0, 1);
  std::generate(a.begin(), a.end(), [&] { return val_dist(gen); });
  mrpython::typical_segment_tree_min<int> tree(a.begin(), a.end());
  while (q--) {
    std::size_t l = size_dist(gen) - 1;
    int tv = val_dist(gen);
    auto check = [tv](int x) { return x <= tv; };
    size_t ans = a.rend() - std::find_if(a.rend() - l - 1, a.rend(), check) - 1;
    size_t out = tree.find_last_left(l, check);
    EXPECT_EQ(out, ans);
  }
}
