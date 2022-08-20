#include <complex>
#include <gtest/gtest.h>

/*
 * @brief 函数的部分特化
 *        事实上，函数模版并不支持部分特化。但是和完全特化一样，我们可以通过重载提供特殊的实现
 *        下面
 */

//###############################方式 1#########################################
template <typename T> inline T my_abs(const T &x) { return x < T(0) ? -x : x; }

template <typename T> inline T my_abs(const std::complex<T> &x) {
  return std::sqrt(std::real(x) * std::real(x) + std::imag(x) * std::imag(x));
}

TEST(partial_specialization, basic_test) {
  std::complex<float> a{3, 4};
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << "::my_abs(std::complex<float>(1, 2)) = " << ::my_abs(a)
            << std::endl;
  oss << "::my_abs(std::complex<float>(1, 2)) = 5\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

//###############################方式 2#########################################
/*
 * @brief 这种方式，使用创建一个functor,
 * 在functor中通过模版特化来实现不同数据类型的不同操作
 *        这种场景下，abs函数只需要定义成一个标准模版即可.
 */
template <typename T> struct abs_functor {
  typedef T result_type;
  T operator()(const T &X) { return X < T(0) ? -X : X; }
};

template <typename T> struct abs_functor<std::complex<T>> {
  typedef T result_type;

  T operator()(const std::complex<T> &X) {
    return std::sqrt(std::real(X) * std::real(X) + std::imag(X) * std::imag(X));
  }
};

template <typename T> decltype(auto) my_abs_functor(const T &X) {
  return abs_functor<T>()(X);
}

TEST(function_partial_specialization, functor_test) {
  std::complex<float> a{3, 4};
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << "::my_abs_functor(std::complex<float>(1, 2)) = "
            << ::my_abs_functor(a) << std::endl;
  oss << "::my_abs_functor(std::complex<float>(1, 2)) = 5\n";

  std::cout << "::my_abs_functor(-2) = " << ::my_abs_functor(-2) << std::endl;
  oss << "::my_abs_functor(-2) = 2\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}