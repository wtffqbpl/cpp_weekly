#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <type_traits>
#include <typeinfo>

// std::conditional
// Provides member typedef type, which is defined as T if B is `true` at
// compile time, or as F if B is `false`. The behavior of a program that adds
// specializations for conditional is undefined.
// Possible implementation
/// \code
/// template <bool B, typename T, typename F>
/// struct conditional { using type = T; }
///
/// template <class T, class F>
/// struct conditional<false, T, F> { using type = F; }
/// \endcode
// Helper types
/// \code
/// template <bool B, typename T, typename F>
/// using conditional_t = typename conditional<B, T, F>::type;
/// \endcode

TEST(type_traits_test, conditional_test) {
  typedef std::conditional<true, int, double>::type Type1;
  typedef std::conditional<false, int, double>::type Type2;
  typedef std::conditional<sizeof(int) >= sizeof(double), int, double>::type
      Type3;

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << typeid(Type1).name() << '\n';
  std::cout << typeid(Type2).name() << '\n';
  std::cout << typeid(Type3).name() << '\n';
  oss << "i\n"
         "d\n"
         "d\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}

#define SHOW(...) std::cout << #__VA_ARGS__ " : " << std::__VA_ARGS__ << '\n'

// Reference: [is_base_of](https://en.cppreference.com/w/cpp/types/is_base_of)
TEST(type_traits_test, is_base_of_test) {
  class A {};
  class B : A {};
  class C : B {};
  class D {};

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << std::boolalpha;
  SHOW(is_base_of_v<A, A>);
  SHOW(is_base_of_v<A, B>);
  SHOW(is_base_of_v<A, C>);
  SHOW(is_base_of_v<A, D>);
  SHOW(is_base_of_v<B, A>);
  oss << "is_base_of_v<A, A> : true\n"
         "is_base_of_v<A, B> : true\n"
         "is_base_of_v<A, C> : true\n"
         "is_base_of_v<A, D> : false\n"
         "is_base_of_v<B, A> : false\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

namespace {
// Possible implementation:
// template <typename T, typename U>
// struct is_same : std::false_type {};
// template <typename T>
// struct is_same<T, T> : std::true_type {};
// template <class T, class U>
// inline constexpr bool is_same_v = is_same<T, U>:: value;

void print_separator() { std::cout << "-----\n"; }

void is_same_test() {
  std::cout << std::boolalpha;
  // some implementation-defined facts.

  // usually true if 'int' is 32 bit.
  std::cout << std::is_same_v<int, std::int32_t> << ' '; // ~ true
  // possibly true if ILP64 data model is used.
  std::cout << std::is_same_v<int, std::int64_t> << "\n"; // ~ false

  print_separator();

  // compare the types of a couple variables.
  long double num1 = 1.0;
  long double num2 = 2.0;
  std::cout << std::is_same_v<decltype(num1), decltype(num2)> << '\n'; // true
  print_separator();

  // 'float' is never an integral type.
  std::cout << std::is_same_v<float, std::int32_t> << '\n'; // false
  print_separator();

  // 'int' is implicitly 'signed'
  std::cout << std::is_same_v<int, int> << ' ';          // true
  std::cout << std::is_same_v<int, unsigned int> << ' '; // false;
  std::cout << std::is_same_v<int, signed int> << '\n';  // true
  print_separator();

  // unlike other types, 'char' is neither 'unsigned' nor 'signed'.
  std::cout << std::is_same_v<char, char> << ' ';          // true
  std::cout << std::is_same_v<char, unsigned char> << ' '; // false
  std::cout << std::is_same_v<char, signed char> << '\n';  // false

  // const-qualified type T is not same as non-const T
  static_assert(not std::is_same_v<const int, int>);
}

} // namespace

TEST(is_same_test, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  is_same_test();
  oss << "true false\n"
         "-----\n"
         "true\n"
         "-----\n"
         "false\n"
         "-----\n"
         "true false true\n"
         "-----\n"
         "true false false\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}