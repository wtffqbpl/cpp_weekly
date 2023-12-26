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

} // namespace