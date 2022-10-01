#include <algorithm>
#include <gtest/gtest.h>
#include <functional>

/// std::transform 在指定的范围内应用给定的操作，并将结果存储在指定的另一个范围内,
/// 要使用std::transform 函数，需要包含 algorithm header file.

/// \code
/// template <class InputIterator, class  OutputIterator, class UnaryOperation>
/// OutputIterator transform(InputIterator first, InputIterator last,
///                          OutputIterator result, UnaryOperation op);
/// \endcode
TEST(algorithm_transform, unary_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> pool = {1, 2, 3, 4, 5, 6};
  std::vector<int> res;

  std::transform(pool.begin(), pool.end(), std::back_inserter(res),
                 [](const auto val) -> int { return val + 5; });
  std::copy(res.begin(), res.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';

  // Expected results.
  std::copy(res.begin(), res.end(), std::ostream_iterator<int>(oss, " "));
  oss << '\n';

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n" << oss.str() << '\n'
            << "Actual output:\n" << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

/// 对于二元操作，使用 [first1, last1) 范围内的每个元素作为第一个参数调用 binary_op,
/// 并以fist2 开头的范围内的每个元素作为第二个参数调用binary_op, 每次调用返回值都存储
/// 在以result开头的范围内
/// \code
/// template <class InputIterator1, class InputIterator2,
///           class OutputIterator, class BinaryOperation>
/// OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
///                          InputIterator2 first2, OutputIterator result,
///                          BinaryOperation binary_op);
/// \endcode

template <typename T> struct plus {
  T operator()(const T v1, const T v2) { return v1 + v2; }
};

TEST(algorithm_transform, binary_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector<int> arr1 = {1, 3, 5};
  std::vector<int> arr2{arr1.begin(), arr1.end()};


  std::for_each(arr1.begin(), arr1.end(), [](auto &val) { val += 5; });
  for (const auto &val : arr1)
    std::cout << val << ' ';
  std::cout << '\n';
  // expected.
  std::copy(arr1.begin(), arr1.end(),
            std::ostream_iterator<int>(oss, " "));
  oss << '\n';

  std::vector<int> res;
  std::transform(arr1.begin(), arr1.end(), arr2.begin(),
                 std::back_inserter(res), plus<int>());
  std::copy(res.begin(), res.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
  std::copy(res.begin(), res.end(),
            std::ostream_iterator<int>(oss, " "));
  oss << '\n';

  std::vector<std::string> names = {"hi", "test", "foo"};
  std::vector<size_t> name_sizes;
  std::transform(names.begin(), names.end(), std::back_inserter(name_sizes),
                 [](const std::string &name)-> size_t { return name.size(); });
  std::copy(name_sizes.begin(), name_sizes.end(),
            std::ostream_iterator<size_t>(std::cout, " "));
  std::cout << '\n';
  std::copy(name_sizes.begin(), name_sizes.end(),
            std::ostream_iterator<size_t>(oss, " "));
  oss << '\n';

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n" << oss.str() << '\n'
            << "Actual output:\n" << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}