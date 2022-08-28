#include <cmath>
#include <gtest/gtest.h>
#include <limits>

// <limits>
// 是一个非常有用的泛型编程库,它提供了关于类型的重要信息。当跨平台编译程序且
// 某些类型的实现存在差异时，它可以防止一些预期之外的行为。当头文件<limits>
// 包含类模版 numeric_limits,
// 它提供了内建类型特定信息。这些信息对于处理数值类型参数尤为重要。

template <typename T> inline void test(const T &x) {
  std::cout << "x = " << x << " (";
  int oldp = std::cout.precision(std::numeric_limits<T>::digits10 + 1);
  // std::numeric_limits<T>::digits10
  // 的值是类型T能无更改地标示的底10位数，即任何拥有
  // 这么多十进制有效数字的数能转成T的值并转换回十进制形式。
  std::cout << x << ")" << std::endl;
  std::cout.precision(oldp);
}

TEST(limit, basic_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  test(1.f / 3.f);
  test(1. / 3.0);
  test(1. / 3.0l);

  oss << "x = 0.333333 (0.3333333)\n"
      << "x = 0.333333 (0.3333333333333333)\n"
      << "x = 0.333333 (0.3333333333333333)\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(act_output != "");
}

template <typename Container>
typename Container::value_type inline minimum(const Container &C) {
  using vt = typename Container::value_type;
  // max method is a static method, you don't need to initialize for max value.
  vt min_value = std::numeric_limits<vt>::max();

  for (const vt &x : C)
    if (x < min_value)
      min_value = x;

  return min_value;
}

TEST(limit, calc_min_template) {
  std::vector<int> pool = {3, 4, 5, 6, 7, 1, 2, 3, 4};

  auto min_value = minimum(pool);

#ifndef NDEBUG
  std::cout << "The minimum value of pool vector is " << min_value << std::endl;
#endif

  EXPECT_EQ(min_value, 1);
}

/*
 * @brief calculate square root of x.
 */
template <typename T> T square_root(const T &x) {
  const T my_eps = T{2} * x * std::numeric_limits<T>::epsilon();
  T r = x;

  while (std::abs((r * r) - x) > my_eps)
    r = (r + x / r) / T{2};

  return r;
}

TEST(limit, calc_square_root) {
  float a = 10.f;
  double b = 20.;

  float c = square_root(a);
  double d = square_root(b);

#ifndef NDEBUG
  std::cout << "The square root of " << a << " is " << c << '\n'
            << "The std::sqrt(" << a << ") is " << std::sqrt(a) << '\n'
            << "The square root of " << b << " is " << d << '\n'
            << "The std::sqrt(" << b << ") is " << std::sqrt(b) << '\n';
#endif

  EXPECT_TRUE(std::abs(c - std::sqrt(a)) <
              std::numeric_limits<float>::epsilon());
  EXPECT_TRUE(std::abs(d - std::sqrt(b)) <
              std::numeric_limits<double>::epsilon());
}

struct simple_point {
#ifdef __cplusplus
  simple_point(double x, double y) : x(x), y(y) {}
  simple_point() = default;
  simple_point(std::initializer_list<double> il) {
    auto it = std::begin(il);
    x = *it;
    y = *std::next(it);
  }
#endif

  double x, y;
};

TEST(pod, basic_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  std::cout << "simple_point is pod = " << std::boolalpha
            << std::is_pod<simple_point>::value << std::endl;

  oss << "simple_point is pod = true\n";

  simple_point p1{3.0, 7.1}, p2{};


  static_assert(std::is_trivially_copyable<simple_point>::value,
                "simple_point is not as simple as you think "
                "and cannot be memcpyd!");
  memcpy(&p2, &p1, sizeof p1);

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}