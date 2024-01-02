#if __cplusplus >= 202002L

#ifdef __APPLE__

#include <concepts>
#include <gtest/gtest.h>
#include <iostream>

namespace {

// Since C++20, you can use placeholders such as auto for all functions
// (including member functions and operators): Abbreviated function template
// syntax.
void print_coll(const auto &coll) // generic function
{
  for (const auto &elem : coll) {
    std::cout << elem << '\n';
  }
}
// Such a declaration is just a shortcut for declaring a template such as the
// following:
template <typename T>
void print_coll_template(const T &coll) // equivalent generic function
{
  for (const auto &elem : coll) {
    std::cout << elem << '\n';
  }
}

// In addition, you can specify the template parameters explicitly:
/// \code
/// void print(auto val)
/// {
///   std::cout << val << '\n';
/// }
///
/// print(64)           // val has type int
/// print<char>(64)     // val has type char

// With `auto` parameters, it is significantly easier to implement code with
// circular dependencies.
/// \code
/// class C2;
///
/// class C1 {
/// public:
///   void foo(const C2 &c2) const {  // OK
///     c2.print();                   // ERROR: C2 is incomplete type
///   }
///   void print() const;
/// };
///
/// class C2 {
/// public:
///   void foo(const C1 &c1) const {
///     c1.print();                   // OK
///   }
///
///   void print() const;
/// };
/// \endcode

// As a consequence, you have to implement `C1::foo()` after the structures of
// both classes are declared.

// Because generic functions check members of generic parameters when the call
// happens, by using `auto`, you can just implement the following:
class C1 {
public:
  void foo(const auto &c2) const {
    c2.print(); // OK
  }

  void print() const {}
};

class C2 {
public:
  void foo(const auto &c1) const { c1.print(); }

  void print() const {}
};

// Not that `auto` allows the caller to pass parameters of any type as long as
// the type provides a member function `print()`. If you do not want that, you
// can use the standard `concept std::same_as` to constrain the use of this
// member function for parameters of type `C2` only. For the concept,
// an incomplete type works fine.

/// \code
/// class C2;
/// class C1WithConcept {
/// public:
///   void foo(const std::same_as<C2> auto &c2) const {
///     c2.print(); // OK
///   }
///   void print() const;
/// };

// For a generic lambda, we can use the following step to specify types
// explicitly.
/// \code
/// auto print_func = [] (const auto &arg) {};
/// print_func.operator()<std::string>("hello")
/// \endcode

// Using `auto` to declare a function parameter follows the same rules as using
// it to declare a parameter of a lambda:
// For each parameter declared with `auto`, the function has an implicit
// template parameter.
//  * The parameters can be a parameter pack:
//      void foo(auto... args);
//    This is equivalent to the following (without introducing Types):
//      template <typename... Types>
//      void foo(Types... args);
//  * Using decltype(auto) is not allowed.
// Abbreviated function templates can still be called with (partially)
// explicitly specified template parameters. The template parameters have the
// order of the call parameters. For example:
/// \code
/// void foo(auto x, auto y)
/// {
///     ...
/// }
///
/// foo("hello", 42);                   // x has type const char*, y has type
/// int foo<std::string>("hello", 42);      // x has type std::string, y has
/// type int foo<std::string, long>("hello", 42); // x has type std::string, y
/// has type long \endcode

} // namespace

#endif // __APPLE__

#endif // __cplusplus >= 202002L
