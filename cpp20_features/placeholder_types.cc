#if __cplusplus >= 202002L

#ifdef __APPLE__

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

} // namespace

#endif // __APPLE__

#endif // __cplusplus >= 202002L
