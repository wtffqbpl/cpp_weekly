#include <gtest/gtest.h>

// If you want to disable passing constant objets to non-constant references,
// you can do the following:
//  * Use a static assertion to trigger a compile-time error.
template <typename T> void outR(T &arg) {
  static_assert(!std::is_const_v<T>, "out parameter of foo<T>(T&) is const");
  // ...
}

//  * Disable the template for this case either by using std::enable_if<>
template <typename T, typename = std::enable_if_t<!std::is_const_v<T>>>
void outRR(T &arg) {
  // ...
}

class C {
public:
  C() = default;
  C(C &&c) noexcept { std::cout << "calling move constructor\n"; }
  C(C const &c) { std::cout << "calling copy constructor\n"; }
  C &operator=(C const &c) {
    std::cout << "calling assignment constructor";
    return *this;
  }
};
template <typename T,
          typename = std::enable_if<std::is_move_constructible_v<T>>>
void passR(T &&arg) {
  std::cout << "T is movable.\n";
}

template <typename T> void passRR(T arg) {}
template <typename T> void passRRR(T const &arg) {}
template <typename T> void passRRRR(T &arg) {}

TEST(chap_7_test, forward_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  C c;
  passRR(c);
  passRRR(c);
  passRRRR(c);
  passR(std::move(c));
  passRRRR(c);
  oss << "calling copy constructor\n"
         "T is movable.\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// This gives the impression that declaring a parameter as a forwarding
// reference is almost perfect. But beware, there is no free launch.
// For example, this is the only case where the template parameter T implicitly
// can become a reference type. As a consequence, it might become an error to
// use T to declare a local object without initialization.
/// \code
/// template <typename T>
/// void newPassR(T &&arg) { // arg is a forwarding reference.
///   T x; // for passed lvalues, x is a reference, which requires an
///   initializer.
///        // ERROR MSG:
///        // Declaration of reference variable 'x' requires an initializer in
///        // instantiation of function template specialization 'newPassR<int
///        &>'
///        // requested here
///   // ...
/// }
///
/// newPassR(42); // OK: T deduced as int.
/// int i;
/// newPassR(i); // ERROR: T deduced as int&, which makes the declaration of x
///             // in newPassR() invalid.
/// \endcode

template <typename T> void printT(T arg) {
  std::cout << "printT function calling.\n";
}

TEST(chap_7_test, forward_test_2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  C c;
  printT(c);
  // We can use std::cref() and std::ref(), declared in
  // header file <functional>, to pass the argument by reference.
  printT(std::cref(c));
  // No copy constructor called.

  oss << "calling copy constructor\n"
         "printT function calling.\n"
         "printT function calling.\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// Since string literals are raw arrays, so we always have to take them into
// account. There are two ways to declare string template function.

//  * You can declare template parameters so that they are only valid for
//  arrays:
template <typename T, std::size_t L1, std::size_t L2>
void foo(T (&arg1)[L1], T (&arg2)[L2]) {
  T *pa = arg1; // decay arg1
  T *pb = arg2; // decay arg2
  // ...
}

//  * You can use type traits to detect whether an array (or a pointer)
//  was passed:
template <typename T, typename = std::enable_if_t<std::is_array_v<T>>>
void foo(T &&arg1, T &&arg2) {
  //...
}

// For returning values, we also have two options for safe using.
//  * Use the type trait std::remove_reference<> to convert type T to
//  a non-reference.
template <typename T> typename std::remove_reference_t<T> retV(T p) {
  return T{}; // always returns by value.
}
// Other traits, such as std::decay<>, may also be useful here because they also
// implicitly remove references.

//  * Let the compiler deduce the return type by just declaring the return type
//    to be auto (since C++14), because auto always decays:
template <typename T>
auto retV_auto(T p) { // by-value return type deduced by compiler.
  return T{};         // always returns by value.
}