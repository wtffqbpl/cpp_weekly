#include <gtest/gtest.h>

#include <concepts>

#include "internal_check_conds.h"

// format feature checking
#define __support_format __has_include(<format>)

#if __support_format
#include <format>
#endif

namespace {

// Standard concepts are defined in different header files:
//  * Many basic concepts are defined in the header file <concepts>, which is
//    included by <ranges> and <iterator>.
//  * Concepts for iterators are defined in the header file <iterator>.
//  * Concepts for ranges are defined in the header file <ranges>.
//  * `three_way_comparable` concepts are defined <compare> (which is included
//    by almost every other header file).
//  * `uniform_random_bit_generator` is defined in <random>.
// Almost all concepts are defined in the namespace std. The only exceptions to
// this rule are the ranges concepts, which are defined in the namespace
// std::ranges.

struct S {
  int member;
  int mfunc(int);
  void operator()(int i) const;
};

auto val = std::invocable<decltype(&S::member), S>;

void callable_test() {
#if __support_format
  std::cout << std::format("std::invocable<decltype(callable_test)> = {}\n",
                           std::invocable<decltype(callable_test)>);

  std::cout << std::format("std::invocable<decltype([](int){{}}), char> = {}\n",
                           std::invocable<decltype([](int) {}), char>);

  std::cout << std::format("std::invocable<decltype(&S::mfunc), S, int> = {}\n",
                           std::invocable<decltype(&S::mfunc), S, int>);

  std::cout << std::format("std::invocable<decltype(&S::member), S> = {}\n",
                           std::invocable<decltype(&S::member), S>);
#else
  std::cout << std::boolalpha;
  std::cout << "std::invocable<decltype(callable_test)> = "
            << std::invocable<decltype(callable_test)> << '\n';

  std::cout << "std::invocable<decltype([](int){}), char> = "
            << std::invocable<decltype([](int) {}), char> << '\n';

  std::cout << "std::invocable<decltype(&S::mfunc), S, int> = "
            << std::invocable<decltype(&S::mfunc), S, int> << '\n';

  std::cout << "std::invocable<decltype(&S::member), S> = "
            << std::invocable<decltype(&S::member), S> << '\n';
#endif
  // FIXME: Explain why std::invocable<decltype(&S::member), S> is true.
  // std::invocable is a concept in C++ that checks whether a type can be
  // invoked like a function.
  // It requires two key characteristics:
  //  * The type must have a `operator()` for invocation.
  //  * It must be possible to check if the invocation is `noexcept`.
  // Pointer to Data Member `&S::member`:
  // While not a function itself, the pointer to data member `&S::member` can be
  // used in a way that resembles function call syntax:
  //  * (object.*ptrToMember)(args...)
  //  * This syntax accesses the member variable through the object, giving it
  //    a "callable" appearance.
  // The std::invocable<decltype(&S::member), S>
  //
  // The `std::invocable` concept checks if the expression
  // `std::declval<S>().*std::declval<decltype(&S::member)>()` is valid. In
  // other words, it checks whether you can use the pointer to member syntax to
  // access the member data member of an instance of `S`.
  // This means that, conceptually, `&S::member` can be treated as an invocable
  // entity within generic programming contexts.

  // Key Point:
  // It's essential to remember that `&S::member` doesn't invoke a function in
  // the traditional sense. It merely provides a mechanism to access a data
  // member using a syntax similar to function calls, making it compatible with
  // concepts like `std::invocable`.

#if __support_format
  std::cout << std::format("std::invocable<S, int> = {}\n",
                           std::invocable<S, int>);
#else
  std::cout << "std::invocable<S, int> = " << std::invocable<S, int> << '\n';
#endif
}

// std::indirectly_unary_invocable
//  Purpose:
//  * It checks whether a callable type can be invoked with a single argument
//    obtained through indirection (e.g., a pointer or iterator).
//  * It's primarily used in generic programming to ensure types can work with
//    algorithms that operate indirectly on elements.
//  Requirements:
//  * A type `F` satisfies `std::indirectly_unary_invocable` if:
//    * It's callable with a single argument of type `I`.
//    * It can be invoked with an argument obtained by dereferencing an `I`
//    (e.g., *i).
//  Key Points:
//  * It's a refinement of the more general std::invocable concept, which only
//    requires a type to be callable.
//  * It's often used with range-based algorithms or functions that work with
//    iterators or pointers.

template <typename F, typename I>
concept can_increment = std::indirectly_unary_invocable<F, I> &&
                        std::convertible_to<std::invoke_result_t<F, I>, I &>;

// Use case: Checking if a function can increment elements in a range.
template <typename I, can_increment<I> F> void increment_all(I first, I last) {
  for (; first != last; ++first)
    F{}(*first);
}

// Common Use Cases:
//  * Range-based algorithms that modify elements indirectly (e.g.,
//  std::transform).
//  * Custom iterator adaptors that apply transformations through indirection.
//  * Function objects that operate on values through pointers or iterators.
//
// Benefits
//  * Type safety: Ensures functions can handle indirect invocation, preventing
//    errors.
//  * Generic programming: Enables writing flexible code that works with
//    different callable types and indirect access patterns.
//  * Concept-based design: Promotes clear and concise code by expressing
//    requirements explicitly.

} // namespace

TEST(standard_concepts_test, test1) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  callable_test();

  oss << "std::invocable<decltype(callable_test)> = true\n"
         "std::invocable<decltype([](int){}), char> = true\n"
         "std::invocable<decltype(&S::mfunc), S, int> = true\n"
         "std::invocable<decltype(&S::member), S> = true\n"
         "std::invocable<S, int> = true\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}