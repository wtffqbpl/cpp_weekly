#include <gtest/gtest.h>
#include <concepts>

namespace {

template <typename T>
requires (!std::is_pointer_v<T>)
T max_value(T a, T b) { return b < a ? a : b; }

// int x = 42;
// int y = 77;
// std::cout << max_value(x, y) << '\n';    // OK: maximum value of ints
// std::cout << max_value(&x, &y) << '\n';  // ERROR: constraint not met
// The requirement is a compile-time check and has no impact on the performance
// of the compiled code. It merely means that the template cannot be used for
// raw pointers. When raw pointers are passed, the compiler behaves as if the
// template were not there.
// Self-defined concept

/// \code
/// template <typename T>
/// concept IsPointer = std::is_pointer_v<T>;
///
/// template <typename T>
/// requires (!IsPointer<T>)
/// T max_value(T a, T b) { return b < a ? a : b; }
/// \endcode

// By using constraints and concepts, we can even overload the max_value()
// template to have one implementation for pointers and one for other types:
template <typename T>
concept IsPointer = std::is_pointer_v<T>;

template <typename T>
requires (!IsPointer<T>)
T max_value(T a, T b) { return b < a ? a : b; }

template <typename T>
requires IsPointer<T>
auto max_value(T a, T b) {
  return max_value(*a, *b); // compare values the pointers point to
}

}