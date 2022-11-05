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