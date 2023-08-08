#include "gtest/gtest.h"
#include <cmath>
#include <type_traits>

class Debug {
public:
  constexpr explicit Debug(bool b = true) : HW_(b), IO_(b), Other_(b) {}
  constexpr Debug(bool h, bool i, bool o) : HW_(h), IO_(i), Other_(o) {}
  [[nodiscard]] constexpr bool any() const { return HW_ || IO_ || Other_; }

private:
  bool HW_;
  bool IO_;
  bool Other_;
};

/*
 * @brief constexpr 修饰的constructor，
 */

TEST(constexpr_test, constexpr_ctor) {
  constexpr Debug IOSub{false, true, false};
  constexpr Debug Prod(false);

  std::stringstream oss;
  testing::internal::CaptureStderr();

  if (IOSub.any())
    std::cerr << "print appropriate error messages." << std::endl;
  oss << "print appropriate error messages.\n";

  if (Prod.any())
    std::cerr << "print an error message; " << std::endl;

  std::string act_output = testing::internal::GetCapturedStderr();

#ifndef NDEBUG
  std::cout << "Expected output:\n" << oss.str()
            << "Actual output:\n" << act_output
            << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

constexpr double power(double b, int x) {
  if (std::is_constant_evaluated() && x >= 0) {
    double r = 1.0, p = b;
    auto u = static_cast<unsigned>(x);

    while (u != 0) {
      if (u & 1)
        r *= p;
      u /= 2;
      p *= p;
    }
    return r;
  } else {
    return std::pow(b, static_cast<double>(x));
  }
}

TEST(constexpr_test, is_constant_evaluated_test) {
  constexpr double kilo = power(10.0, 3);
  int n = 3;
  double mucho = power(10.0, n);

  EXPECT_FLOAT_EQ(mucho, 10.0 * 10.0 * 10.0);
  EXPECT_FLOAT_EQ(kilo, mucho);
}
