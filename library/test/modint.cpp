#include <gtest/gtest.h>

#include <limits>
#include <mrpython/modint.hpp>
#include <random>

using mrpython::modint;

// Test fixture for modint tests
class ModIntTest: public ::testing::Test {
 protected:
  // Common moduli for testing
  static constexpr int SMALL_PRIME = 11;
  static constexpr int MEDIUM_PRIME = 10007;
  static constexpr int LARGE_PRIME = 998244353;
};

constexpr int ModIntTest::SMALL_PRIME;
constexpr int ModIntTest::MEDIUM_PRIME;
constexpr int ModIntTest::LARGE_PRIME;

// Tests for construction and value representation
TEST_F(ModIntTest, Construction) {
  // Basic construction
  modint<SMALL_PRIME> a(3);
  EXPECT_EQ(a.val(), 3);

  // Construction from negative value
  modint<SMALL_PRIME> b(-1);
  EXPECT_EQ(b.val(), SMALL_PRIME - 1);

  // Construction from value larger than modulus
  modint<SMALL_PRIME> c(15);
  EXPECT_EQ(c.val(), 4);  // 15 % 11 = 4

  // Default constructor (zero)
  modint<SMALL_PRIME> d;
  EXPECT_EQ(d.val(), 0);

  // Construction with unsigned types
  modint<SMALL_PRIME> e(static_cast<unsigned int>(20));
  EXPECT_EQ(e.val(), 9);  // 20 % 11 = 9

  // Construction with large negative
  modint<SMALL_PRIME> f(-100);
  EXPECT_EQ(f.val(), 10);  // (-100 % 11 + 11) % 11 = 10

  // Construction with zero
  modint<SMALL_PRIME> g(0);
  EXPECT_EQ(g.val(), 0);

  // Test with large modulus
  modint<LARGE_PRIME> h(LARGE_PRIME - 1);
  EXPECT_EQ(h.val(), LARGE_PRIME - 1);

  modint<LARGE_PRIME> i(LARGE_PRIME + 5);
  EXPECT_EQ(i.val(), 5);
}

// Tests for addition
TEST_F(ModIntTest, Addition) {
  // Basic addition
  modint<SMALL_PRIME> a(5);
  modint<SMALL_PRIME> b(3);
  EXPECT_EQ((a + b).val(), 8);

  // Addition with overflow
  modint<SMALL_PRIME> c(7);
  modint<SMALL_PRIME> d(6);
  EXPECT_EQ((c + d).val(), 2);  // 13 % 11 = 2

  // Addition with exact modulus result
  modint<SMALL_PRIME> e(6);
  modint<SMALL_PRIME> f(5);
  EXPECT_EQ((e + f).val(), 0);  // 11 % 11 = 0

  // Testing += operator
  modint<SMALL_PRIME> g(4);
  g += modint<SMALL_PRIME>(9);
  EXPECT_EQ(g.val(), 2);  // 13 % 11 = 2

  // Addition with large modulus
  modint<LARGE_PRIME> h(LARGE_PRIME - 2);
  modint<LARGE_PRIME> i(1);
  EXPECT_EQ((h + i).val(), LARGE_PRIME - 1);

  modint<LARGE_PRIME> j(LARGE_PRIME - 1);
  modint<LARGE_PRIME> k(1);
  EXPECT_EQ((j + k).val(), 0);  // Wraps around to 0

  modint<LARGE_PRIME> l(LARGE_PRIME - 1);
  modint<LARGE_PRIME> m(2);
  EXPECT_EQ((l + m).val(), 1);  // Wraps to 1
}

// Tests for subtraction
TEST_F(ModIntTest, Subtraction) {
  // Basic subtraction
  modint<SMALL_PRIME> a(8);
  modint<SMALL_PRIME> b(3);
  EXPECT_EQ((a - b).val(), 5);

  // Subtraction with underflow
  modint<SMALL_PRIME> c(2);
  modint<SMALL_PRIME> d(5);
  EXPECT_EQ((c - d).val(), 8);  // (2-5) % 11 = 8

  // Testing -= operator
  modint<SMALL_PRIME> e(7);
  e -= modint<SMALL_PRIME>(10);
  EXPECT_EQ(e.val(), 8);  // (7-10) % 11 = 8

  // Subtraction resulting in 0
  modint<SMALL_PRIME> f(9);
  modint<SMALL_PRIME> g(9);
  EXPECT_EQ((f - g).val(), 0);

  // Subtraction with large modulus
  modint<LARGE_PRIME> h(0);
  modint<LARGE_PRIME> i(1);
  EXPECT_EQ((h - i).val(), LARGE_PRIME - 1);  // Wraps to MOD-1

  // Unary minus
  modint<SMALL_PRIME> j(4);
  EXPECT_EQ((-j).val(), 7);  // -4 % 11 = 7
}

// Tests for multiplication
TEST_F(ModIntTest, Multiplication) {
  // Basic multiplication
  modint<SMALL_PRIME> a(3);
  modint<SMALL_PRIME> b(4);
  EXPECT_EQ((a * b).val(), 1);  // 12 % 11 = 1

  // Multiplication by 0
  modint<SMALL_PRIME> c(7);
  modint<SMALL_PRIME> zero(0);
  EXPECT_EQ((c * zero).val(), 0);

  // Multiplication by 1
  modint<SMALL_PRIME> d(9);
  modint<SMALL_PRIME> one(1);
  EXPECT_EQ((d * one).val(), 9);

  // Multiplication by integer
  modint<SMALL_PRIME> e(5);
  EXPECT_EQ((e * 7).val(), 2);  // 35 % 11 = 2

  // Commutative property
  modint<SMALL_PRIME> f(6);
  EXPECT_EQ((f * 5).val(), (5 * f).val());

  // Testing *= operator
  modint<SMALL_PRIME> g(2);
  g *= modint<SMALL_PRIME>(6);
  EXPECT_EQ(g.val(), 1);  // 12 % 11 = 1

  g *= 3;
  EXPECT_EQ(g.val(), 3);  // 3 % 11 = 3

  // Testing multiplication with large values
  modint<MEDIUM_PRIME> h(123456);
  modint<MEDIUM_PRIME> i(789101112);
  modint<MEDIUM_PRIME> result = h * i;
  // Expected: (123456 * 789101112) % 998244353
  unsigned long long expected =
      (static_cast<unsigned long long>(123456) * 789101112) % MEDIUM_PRIME;
  EXPECT_EQ(result.val(), expected);

  // Multiplication with negative signed integer
  modint<SMALL_PRIME> n1(3);
  int neg_val = -8;
  EXPECT_EQ((n1 * neg_val).val(), 9);
  EXPECT_EQ((neg_val * n1).val(), 9);

  // Multiplication with negative modint
  modint<SMALL_PRIME> n2(-5);
  modint<SMALL_PRIME> n3(-7);
  EXPECT_EQ((n2 * n3).val(), 2);
  EXPECT_EQ((n3 * n2).val(), 2);

  // Multiplication with unsigned integer
  unsigned int uval = 8u;
  modint<SMALL_PRIME> n4(7);
  EXPECT_EQ((n4 * uval).val(), 1);
  EXPECT_EQ((uval * n4).val(), 1);

  // Multiplication with large numbers (potential overflow)
  modint<LARGE_PRIME> big1(123456789);
  modint<LARGE_PRIME> big2(987654321);
  unsigned long long expected_big =
      (static_cast<unsigned long long>(123456789) * 987654321) % LARGE_PRIME;
  EXPECT_EQ((big1 * big2).val(), expected_big);
  EXPECT_EQ((big2 * big1).val(), expected_big);

  // Multiplication with negative large signed integer
  int64_t neg_large = -10000000000LL;
  modint<LARGE_PRIME> n5(12345);
  auto expected_neg_large =
      (static_cast<long long>(12345) *
       ((neg_large % LARGE_PRIME + LARGE_PRIME) % LARGE_PRIME)) %
      LARGE_PRIME;
  EXPECT_EQ((n5 * neg_large).val(), expected_neg_large);
  EXPECT_EQ((neg_large * n5).val(), expected_neg_large);
}

// Tests for division and modular inverse
TEST_F(ModIntTest, Division) {
  // Basic division
  modint<SMALL_PRIME> a(8);
  modint<SMALL_PRIME> b(2);
  EXPECT_EQ((a / b).val(), 4);  // 8/2 = 4

  // Division using modular inverse
  modint<SMALL_PRIME> c(3);
  modint<SMALL_PRIME> d(4);
  modint<SMALL_PRIME> result = c / d;

  // Verify: c / d = c * d^-1, so result * d should equal c
  EXPECT_EQ((result * d).val(), c.val());

  // Test modular inverse explicitly
  modint<SMALL_PRIME> e(3);
  modint<SMALL_PRIME> e_inv = e.inv();
  EXPECT_EQ((e * e_inv).val(), 1);  // Inverse property: x * x^-1 = 1

  // Testing /= operator
  modint<SMALL_PRIME> f(10);
  f /= modint<SMALL_PRIME>(2);
  EXPECT_EQ(f.val(), 5);

  // Division with larger prime modulus
  modint<MEDIUM_PRIME> g(123456789);
  modint<MEDIUM_PRIME> h(987654321);
  modint<MEDIUM_PRIME> div_result = g / h;
  EXPECT_EQ((div_result * h).val(), g.val());  // Verify inverse relation
}

// Tests for the power function
TEST_F(ModIntTest, Power) {
  // Test various powers
  modint<SMALL_PRIME> a(2);
  EXPECT_EQ(a.pow(0).val(), 1);   // 2^0 = 1
  EXPECT_EQ(a.pow(1).val(), 2);   // 2^1 = 2
  EXPECT_EQ(a.pow(2).val(), 4);   // 2^2 = 4
  EXPECT_EQ(a.pow(3).val(), 8);   // 2^3 = 8
  EXPECT_EQ(a.pow(4).val(), 5);   // 2^4 = 16, 16 % 11 = 5
  EXPECT_EQ(a.pow(10).val(), 1);  // 2^10 = 1024, 1024 % 11 = 1

  // Test power of 0
  modint<SMALL_PRIME> zero(0);
  EXPECT_EQ(zero.pow(0).val(), 1);  // 0^0 is conventionally 1
  EXPECT_EQ(zero.pow(1).val(), 0);  // 0^1 = 0

  // Test power of 1
  modint<SMALL_PRIME> one(1);
  EXPECT_EQ(one.pow(1000000).val(), 1);  // 1^n = 1 for any n

  // Test with larger prime
  modint<MEDIUM_PRIME> b(123);
  // Check that b^(p-1) = 1 (Fermat's Little Theorem)
  EXPECT_EQ(b.pow(MEDIUM_PRIME - 1).val(), 1);
}

TEST_F(ModIntTest, InverseHelpers) {
  // linear_inv should behave like multiplicative inverse on [1, P)
  for (unsigned int i = 1; i < SMALL_PRIME; ++i) {
    auto inv = modint<SMALL_PRIME>::linear_inv(i);
    EXPECT_EQ((inv * modint<SMALL_PRIME>(i)).val(), 1u);
  }

  // frac_inv is 1 / n!; verify by multiplying forward factorials
  modint<SMALL_PRIME> fact = 1;
  for (unsigned int i = 0; i <= 8; ++i) {
    if (i > 0) fact *= modint<SMALL_PRIME>(i);
    auto inv_fact = modint<SMALL_PRIME>::frac_inv(i);
    EXPECT_EQ((fact * inv_fact).val(), 1u);
  }
}

TEST_F(ModIntTest, Comb) {
  // Small Pascal triangle slice to ensure comb matches expected values
  const unsigned int n = 8;
  for (unsigned int k = 0; k <= n; ++k) {
    unsigned int expected = 1;
    for (unsigned int i = 1; i <= k; ++i) {
      expected = expected * (n - i + 1) / i;
    }
    EXPECT_EQ(modint<SMALL_PRIME>::comb(n, k).val(), expected % SMALL_PRIME);
  }

  // Zero and out-of-range cases
  EXPECT_EQ(modint<SMALL_PRIME>::comb(0, 0).val(), 1u);
  EXPECT_EQ(modint<SMALL_PRIME>::comb(5, 6).val(), 0u);
}

// Test edge cases and miscellaneous functionality
TEST_F(ModIntTest, EdgeCases) {
  // Test modval() static method
  EXPECT_EQ(modint<SMALL_PRIME>::modval(), SMALL_PRIME);
  EXPECT_EQ(modint<LARGE_PRIME>::modval(), LARGE_PRIME);

  // Test with values at the modulus boundary
  modint<SMALL_PRIME> a(SMALL_PRIME - 1);
  modint<SMALL_PRIME> b(1);
  EXPECT_EQ((a + b).val(), 0);  // (MOD-1) + 1 = MOD = 0 (mod MOD)

  // Test operations that wrap exactly once
  modint<SMALL_PRIME> c(SMALL_PRIME - 3);
  modint<SMALL_PRIME> d(SMALL_PRIME - 4);
  EXPECT_EQ((c + d).val(), SMALL_PRIME - 7);  // (MOD-3) + (MOD-4) = 2*MOD - 7 =
                                              // MOD + (MOD-7) = MOD-7 (mod MOD)

  // Test operations that wrap multiple times
  modint<SMALL_PRIME> e(5);
  EXPECT_EQ(e.pow(100).val(),
            (mrpython::fastPow(modint<SMALL_PRIME>(5), 100)).val());

  // Test stream output
  std::ostringstream oss;
  modint<SMALL_PRIME> f(7);
  oss << f;
  EXPECT_EQ(oss.str(), "7");
}

// Test with random values
TEST_F(ModIntTest, RandomValues) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(),
                                          std::numeric_limits<int>::max());

  // Test addition and subtraction properties with random values
  for (int i = 0; i < 100000; i++) {
    int va = dist(gen), vb = dist(gen);

    modint<MEDIUM_PRIME> a(va);
    modint<MEDIUM_PRIME> b(vb);

    EXPECT_NE(vb, 0);

    EXPECT_EQ((a + b).val(), (b + a).val());
    EXPECT_EQ(
        (a + b).val(),
        (((long long)va + vb) % MEDIUM_PRIME + MEDIUM_PRIME) % MEDIUM_PRIME);

    EXPECT_EQ(((a - b) + b).val(), a.val());
    EXPECT_EQ(
        (a - b).val(),
        (((long long)va - vb + MEDIUM_PRIME) % MEDIUM_PRIME + MEDIUM_PRIME) %
            MEDIUM_PRIME);

    EXPECT_EQ((a * b).val(), (b * a).val());
    EXPECT_EQ((a * b).val(),
              (((long long int)va * vb) % MEDIUM_PRIME + MEDIUM_PRIME) %
                  MEDIUM_PRIME);

    if (vb % MEDIUM_PRIME != 0) {
      EXPECT_EQ(
          ((long long int)((a / b).val()) * vb % MEDIUM_PRIME + MEDIUM_PRIME) %
              MEDIUM_PRIME,
          (va % MEDIUM_PRIME + MEDIUM_PRIME) % MEDIUM_PRIME);

      EXPECT_EQ(
          ((long long int)(b.inv().val()) * vb % MEDIUM_PRIME + MEDIUM_PRIME) %
              MEDIUM_PRIME,
          1);
    }
  }
}
