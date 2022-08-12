#include <gtest/gtest.h>
#include <type_traits>

TEST(Chap_1_Function_Template, max1_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  int i = 42;
  std::cout << "max(7, i): " << std::max(7, i) << '\n';
  oss << "max(7, i): 42\n";

  double f1 = 3.4;
  double f2 = -6.7;
  std::cout << "max(f1, f2): " << std::max(f1, f2) << '\n';
  oss << "max(f1, f2): 3.4\n";

  std::string s1 = "mathematics";
  std::string s2 = "math";
  std::cout << "max(s1, s2): " << std::max(s1, s2) << '\n';
  oss << "max(s1, s2): mathematics\n";

  std::string act_output = testing::internal::GetCapturedStdout();
  std::cout << "Expected output:\n" << oss.str() << '\n'
            << "Actual output:\n" << act_output << '\n';

  EXPECT_TRUE(oss.str() == act_output);
}

template<typename T1, typename T2>
auto max(T1 a, T2 b) -> typename std::decay<decltype(true ? a : b)>::type {
  return b < a ? a : b;
}

template<typename T1, typename T2>
typename std::common_type<T1, T2>::type max2(T1 a, T2 b) {
  return b < a ? a : b;
}

#if __cplusplus >= 201406L
/*
 * @brief 使用 std::common_type_t<T1, T2>，相当于 std::common_type<T1, T2>::type
 *        从c++14 开始有这个声明。
 */
template<typename T1, typename T2>
std::common_type_t<T1, T2> max3(T1 a, T2 b) {
  return b < a ? a : b;
}

/*
 * @brief c++14中，可以直接使用auto来声明函数返回值，让编译器来自动进行返回值类型推导。
 */
template<typename T1, typename T2>
auto max_auto_decl(T1 a, T2 b) {
  return b < a ? a : b;
}
#endif

/*
 * @brief 可以定义一个默认的返回值类型.
 */
template<typename T1, typename T2,
        typename RT = std::decay_t< decltype(true ? T1() : T2())>>
RT max_default_rt(T1 a, T2 b) {
  return b < a ? a : b;
};
