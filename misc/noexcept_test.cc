#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <utility>
#include <vector>

// The noexcept operator performs a compile-time check that returns `true` if an
// expression is declared to not throw any exceptions.
// It can be used within a function template's noexcept specifier to declare
// that the function will throw exceptions for some types but not others.
/// \code
/// noexcept(expression)
/// \endcode
// The noexcept operator does not evaluate expression.
// The result is `true` if the set of potential exceptions of the expression is
// `expression` is non-throwing, and false for otherwise.

namespace noexcept_test {
void may_throw() {}
void no_throw() noexcept {}
auto lmay_throw = [] {};
auto lno_throw = []() noexcept {};

class T {
public:
  ~T() {} // dtor prevents move ctor
          // copy ctor is noexcept.
};

// Compile class T in https://cppinsights.io, we get the following code snippet:
/// \code
/// class T
/// {
/// public:
///   inline ~T() noexcept { }
///
///   inline constexpr T() noexcept = default;
///   inline constexpr T(const T &) noexcept = default;
/// };
/// \endcode

class U {
public:
  ~U() {} // dtor prevents move ctor
          // copy ctor is noexcept(false)
  std::vector<int> v;
};

// Compile class U in https://cppinsights.io, and we get the following code
// snippet:
/// \code
/// class U
/// {
///
/// public:
///   inline ~U() noexcept { }
///
///   std::vector<int, std::allocator<int> > v;
///   inline constexpr U(const U &) noexcept(false) = default;
///   inline constexpr U() noexcept = default;
/// };
/// \endcode

class V {
public:
  std::vector<int> v;
};

// Compile class V in https://cppinsights.io, we get the following code snippet:
/// \code
/// class V
/// {
/// public:
///   std::vector<int, std::allocator<int> > v;
///   inline constexpr V(const V &) noexcept(false) = default;
///   inline constexpr ~V() noexcept = default;
///   inline constexpr V() noexcept = default;
/// };
/// \endcode

// The explanation about "destructor prevent move constructor":
// §12.8 Copying and moving class objects
//
// If the definition of a class X does not explicitly declare a move
// constructor, one will be implicitly declared as defaulted if and only if:
//   * X does not have a user-declared copy constructor,
//   * X does not have a user-declared copy assignment operator,
//   * X does not have a user-declared move assignment operator,
//   * X does not have a user-declared destructor, and
//   * the move constructor would not be implicitly defined as deleted.
//
// Referenced:
// https://stackoverflow.com/questions/34810580/does-base-class-destructor-prevent-move-constructor-being-generated

} // namespace noexcept_test

TEST(noexcept_test, basic_test) {
  using namespace noexcept_test;

  T t;
  U u;
  V v;

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout
      << std::boolalpha << "Is may_throw() noexcept? " << noexcept(may_throw())
      << '\n'
      << "Is no_throw() noexcept? " << noexcept(no_throw()) << '\n'
      << "Is lmay_throw() noexcept? " << noexcept(lmay_throw()) << '\n'
      << "Is ~T() noexcept? " << noexcept(std::declval<T>().~T())
      << '\n'
      // note: the following tests also require that ~T() is noexcept because
      // the expression within noexcept constructs and destroys a temporary.
      << "Is T(rvalue T) noexcept? " << noexcept(T(std::declval<T>())) << '\n'
      << "Is T(lvalue T) noexcept? " << noexcept(T(t)) << '\n'
      << "Is U(rvalue U) noexcept? " << noexcept(U(std::declval<U>())) << '\n'
      << "Is U(lvalue U) noexcept? " << noexcept(U(u)) << '\n'
      << "Is V(rvalue V) noexcept? " << noexcept(V(std::declval<V>())) << '\n'
      << "Is V(lvalue V) noexcept? " << noexcept(V(v)) << '\n';

  oss << "Is may_throw() noexcept? false\n"
      << "Is no_throw() noexcept? true\n"
      << "Is lmay_throw() noexcept? false\n"
      << "Is ~T() noexcept? true\n"
      << "Is T(rvalue T) noexcept? true\n"
      << "Is T(lvalue T) noexcept? true\n"
      << "Is U(rvalue U) noexcept? false\n"
      << "Is U(lvalue U) noexcept? false\n"
      << "Is V(rvalue V) noexcept? true\n"
      << "Is V(lvalue V) noexcept? false\n";

  auto output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, output);
#endif

  EXPECT_TRUE(oss.str() == output);
}