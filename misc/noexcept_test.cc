#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <ios>
#include <type_traits>
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

int foo() noexcept { return 42; }
int foo1() { return 42; }
int foo2() throw() { return 42; }

void test_noexcept_test2() {
  std::cout << std::boolalpha;
  std::cout << "noexcept(foo())   = " << noexcept(foo()) << std::endl;
  std::cout << "noexcept(foo1())  = " << noexcept(foo1()) << std::endl;
  std::cout << "noexcept(foo2())  = " << noexcept(foo2()) << std::endl;
}

template <typename T> T copy(const T &o) noexcept(noexcept(T(o))) { /* ... */
}
// noexcept(T(o)) --- 用来判断T(o) 是否可能抛出异常
// noexcept(第一个noexcept) ---
// 接受第二个noexcept运算符的返回值，以此来决定T类型的复制
// 函数是否声明为不抛出异常。

template <typename T>
void swap(T &a, T &b) noexcept(
    noexcept(T(std::move(a))) &&noexcept(a.operator=(std::move(b)))) {
  static_assert(
      noexcept(T(std::move(a))) &&noexcept(a.operator=(std::move(b))));
  T tmp(std::move(a));
  a = std::move(b);
  b = std::move(tmp);
}

// FINAL SWAP VERSION

struct X {
  X() {}
  X(X &&) noexcept {}
  X(const X &) {}
  X operator=(X &&) noexcept { return *this; }
  X operator=(const X &) { return *this; }
};

struct X1 {
  X1() {}
  X1(X1 &&) {}
  X1(const X1 &) {}
  X1 operator=(X1 &&) { return *this; }
  X1 operator=(const X1 &) { return *this; }
};

template <typename T>
void swap_impl(T &a, T &b, std::integral_constant<bool, true>) noexcept {
  T tmp(std::move(a));
  a = std::move(b);
  b = std::move(tmp);
}

template <typename T>
void swap_impl(T &a, T &b, std::integral_constant<bool, false>) {
  T tmp(a);
  a = b;
  b = tmp;
}
// noexcept(T(std::move(a))) && noexcept(a.operator=(std::move(b)))>())))
// is equivalent to
// std::is_nothrow_move_constructible_v<T> &&
// std::is_nothrow_move_assignable_v<T>

template <typename T>
void swap_new(T &a, T &b) noexcept(noexcept(swap_impl(
    a, b,
    std::integral_constant<bool, noexcept(T(std::move(a))) &&noexcept(
                                     a.operator=(std::move(b)))>()))) {
  swap_impl(a, b,
            std::integral_constant<bool, noexcept(T(std::move(a))) &&noexcept(
                                             a.operator=(std::move(b)))>());
}

} // namespace noexcept_test

TEST(noexcept_test, basic_test) {
  using namespace noexcept_test;

  std::vector<float> b(10);
  std::vector<float> c = b;

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

TEST(noexcept_test, test2) {
  testing::internal::CaptureStdout();
  noexcept_test::test_noexcept_test2();
  auto act_output = testing::internal::GetCapturedStdout();

  std::stringstream oss;

  oss << "noexcept(foo())   = true\n"
         "noexcept(foo1())  = false\n"
         "noexcept(foo2())  = true\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}