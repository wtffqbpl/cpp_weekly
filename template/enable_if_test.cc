#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>

namespace {

// In C++, the class signature of std::enable_if is as follows.
// If B is true, std::enable_if has a public member typedef type, equal to T;
// otherwise, there is no member typedef.
template <bool B, typename T = void>
struct [[maybe_unused]] original_enable_if {};

// 只有 B 为true时，才有typedef T type 这样的定义
template <typename T>
struct [[maybe_unused]] original_enable_if< true, T> { typedef T type; };

// This means, whenever the implementation tries to access
// original_enable_if<B, T>::type when B = false, the compiler will raise
// compilation error, as the object member type is not defined.

// Enable template specialization via template parameters.
// It does matter what type it is. 可以看到下面写的是float类型，这里不关心
// TODO: Why???
// Explanation:
// 从 original_enable_if 的定义中可以看到，enable_if 需要两个参数，其中第一个参数为
// bool 参数，这个参数为true时，才会定义typedef T type, 当需要使用enable_if中的
// 第二个类型 T 时，才需要明确指定这个参数类型.
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, float>>
void foo() { std::cout << "T could only be integer\n"; }

template <typename T,
          std::enable_if_t<std::is_integral_v<T>, bool> = true>
// It does not matter what type it is and what the value is,
// as long as the value is of the type.
void bar() { std::cout << "T could only be integer\n"; }

// 1. the return type (bool) is only valid if T is an integral type:
// 例如这里的enable_if 中定义的 type 需要用在函数返回值定义中, 所以这里的enable_if的定义
// 中，需要明确指定类型
template <typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type
is_odd(T i) { return bool(i % 2); }

// 2. the second template argument is only valid if T is an integral type:
template <typename T,
          typename = typename std::enable_if<std::is_integral_v<T>>::type>
bool is_even(T i) { return !bool(i % 2); }

template <typename T,
          typename = std::enable_if_t<std::is_integral_v<T>, float>>
[[maybe_unused]] void foo2() { std::cout << "T is int\n"; }

// Compile-time error: redefinition
// template <typename T,
//           typename = std::enable_if_t<std::is_floating_point_v<T>, int>>
// [[maybe_unused]] void foo2() { std::cout << "T is float\n"; }

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
[[maybe_unused]] void bar2() { std::cout << "T is int\n"; }

template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
[[maybe_unused]] void bar2() { std::cout << "T is float\n"; }

// Notice that in this case we used the type template parameter compile-time
// checking for the function foo and used the non-type template parameter
// compile-time checking for the function bar.
// When it comes to enabling multiple types for a template function, the type
// template parameter compile-time checking will be prevented by compiler as
// the declarations are treated as declarations of the same function template.
// Here we enabled both integer types and floating point types for the function
// bar using the predicate std::is_integral<T>::value and
// std::is_floating_point<T>::value, respectively.
// The std::is_integral<T>::value and std::is_floating_point<T>::value are
// mutually exclusive and only one can be true at compile time for one
// specialization.
//
// To understand std::enable_if, when std::is_integral<T>::value is evaluated
// to be true at compile time,
// std::enable_if_t<std::is_integral<T>::value, bool> = true is equivalent
// std::enable_if_t<true, bool> = true and is equivalent as
// typename std::enable_if<true, bool>::type = true, and is equivalent as
// typename bool = true.
// The remaining typename is probably an indicator and will be removed during
// preprocessing. So ultimately what compiler will see is bool = true which
// is exactly the same as the class C scenario in the non-type template
// parameters.
// The same analysis could be performed on
// typename = std::enable_if_t<std::is_integral<T>::value, float>
// as well to help the understanding.

// std::enable_if or std::enable_if_t could be used for restricting or enabling
// the types used for template specialization via return type or function
// parameters. Understanding those are almost equivalent as understand enabling
// template specialization via template parameters.

// References
// [C++ Template Specialization Using Enable If](https://leimao.github.io/blog/CPP-Enable-If/)

} // end of namespace

TEST(enable_if_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  foo<int>();
  oss << "T could only be integer\n";
  // foo<float>(); // Compilation error.
  bar<int>();
  oss << "T could only be integer\n";
  // bar<float>(); // Compilation error.

  foo2<int>();
  oss << "T is int\n";

  auto act_output = testing::internal::GetCapturedStdout();

  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(enable_if_test, basic_demo) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  short int i = 1; // code does not compile if type of i is not integral.

  std::cout << std::boolalpha;
  std::cout << "i is odd: " << is_odd(i) << '\n';
  std::cout << "i is even: " << is_even(i) << '\n';
  oss << "i is odd: true\n"
      << "i is even: false\n";

  auto act_output = testing::internal::GetCapturedStdout();

  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}