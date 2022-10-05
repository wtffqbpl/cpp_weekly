#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <iterator>
#include <numeric>

TEST(accumulate_test, basics) {
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  int sum = std::accumulate(v.begin(), v.end(), 0);

  int product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<>());

  auto dash_fold = [](std::string a, int b) {
    return std::move(a) + '-' + std::to_string(b);
  };

  std::string s =
      std::accumulate(std::next(v.begin()), v.end(),
                      std::to_string(v[0]), // start with first element
                      dash_fold);

  // Right fold using reverse iterators
  std::string rs =
      std::accumulate(std::next(v.rbegin()), v.rend(),
                      std::to_string(v.back()), // start with last element
                      dash_fold);

  std::cout << "sum: " << sum << '\n'
            << "product: " << product << '\n'
            << "dash-separated string: " << s << '\n'
            << "dash-separated string (right-folded): " << rs << '\n';
}

TEST(accumulate_test, fold_basic_test) {
  std::vector<int> pool = {1, 2, 3, 4, 5, 6, 7, 8};

  auto val =
      std::accumulate(pool.cbegin(), pool.cend(), std::vector<int>{},
                      [](std::vector<int> prev_collected, const int val) {
                        if (val & 1)
                          prev_collected.push_back(val);
                        return std::move(prev_collected);
                      });

  std::ostringstream oss;
  testing::internal::CaptureStdout();

  std::copy(val.begin(), val.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
  oss << "1 3 5 7 \n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(accumulate_test, impl_copy_if) {
  std::vector<int> pool{1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto res = std::accumulate(pool.begin(), pool.end(), std::vector<int>{},
                             [](std::vector<int> prev, const int val) {
                               if (val % 3 == 0)
                                 prev.push_back(val);
                               return std::move(prev);
                             });

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::copy(res.begin(), res.end(), std::ostream_iterator<int>(std::cout, " "));

  std::string act_output = testing::internal::GetCapturedStdout();
  oss << "3 6 9 ";

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(accumulate_test, impl_any_of) {
  std::vector<int> pool{1, 2, 3, 4, 5, 6, 7, 8, 9};

  std::stringstream oss;
  testing::internal::CaptureStdout();

  if (std::accumulate(pool.begin(), pool.end(), false,
                      [](bool prev, const int val) {
                        prev |= (val & 1);
                        return prev;
                      }))
    std::cout << "has odd value\n";

  oss << "has odd value\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(accumulate_test, impl_all_of) {
  std::vector<int> pool{1, 3, 5, 7, 9};

  auto res1 = std::accumulate(pool.begin(), pool.end(), true,
                              [](bool prev, const int val) {
                                prev &= (val & 1);
                                return prev;
                              });
  EXPECT_TRUE(res1);

  std::vector<int> pool2{1, 3, 5, 7, 9, 10};
  auto res2 = std::accumulate(pool2.begin(), pool2.end(), true,
                              [](bool prev, const int val) {
                                prev &= (val & 1);
                                return prev;
                              });

  EXPECT_FALSE(res2);
}

TEST(accumulate_test, multiply_test) {
  std::vector<int> pool{1, 2, 3, 4};

  // 在这里，推荐使用 std::multiplies<>(), 不推荐使用 std::multiplies<int>(),
  // 原因是： 从
  // c++14开始，无需再指明类型，因为在调用时会自动推断参数的类型。如果使用的compiler
  // 和标准库是c++14兼容的，那么就推荐这种写法，除非比较之前想把参数强制转换成特定的类型。
  // 例如, 从大到小排序:
  /// \code
  /// std::sort(numbers.begin(), numbers.end(), std::greater<>());
  /// \endcode
  // 在C++14中，对于std::multiplies 函数来说，c++14 特化了一个参数为 void
  // 的实现, 该实现中，会根据multiplies
  // 函数的两个输入参数的类型，来推断返回值的类型, 因此无需 再次指定返回值类型,
  // 直接使用特化后的sd::multiplies即可.

  auto val = std::accumulate(pool.begin(), pool.end(), 1, std::multiplies<>());

#ifndef NDEBUG
  std::cout << "The product of std::vector<int> {1, 2, 3, 4} is:\n"
            << "Expected result = " << 24 << '\n'
            << "Actual result = " << val << '\n';
#endif

  EXPECT_EQ(val, 24);
}