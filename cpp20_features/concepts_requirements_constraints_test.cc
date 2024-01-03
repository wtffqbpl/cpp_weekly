#include <gtest/gtest.h>
#include <concepts>
#include <type_traits>

#include "internal_check_conds.h"

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

// The following `IsPointer` version could filter pointer-like types (such as
// smart pointers).
template <typename T>
concept IsPointerGeneric = requires(T p) { *p; };

// We can also require multiple operations, type members, and that expressions
// yield constrained types. For example:
// clang-format off
template <typename T>
concept IsPointerMultipleConstraints = requires(T p) {
                                         *p;                            // operator * has to be valid
                                         p == nullptr;                  // can compare with nullptr
                                         {p < p} -> std::same_as<bool>; // operator < yields bool
                                       };
// clang-format on
//
// We specify three requirements, which all apply to a parameter p of the type
// T that we defined this concept for:
//  * The type has to support the operator *.
//  * The type has to support the operator <, which has to yield type bool.
//  * Objects of that type have to be comparable with nullptr.

// The syntax {p < p} -> std::same_as<bool> is part of the requires-clause in a
// C++20 concept. It is used to express a requirement that a certain expression
// involving the parameter p must be valid and have a specific type.

template <typename T>
requires (!IsPointer<T>)
T max_value(T a, T b) { return b < a ? a : b; }

template <typename T>
requires IsPointer<T>
auto max_value(T a, T b) {
  return max_value(*a, *b); // compare values the pointers point to
}

// Trailing requires Clauses
auto max_value(IsPointer auto a, IsPointer auto b) { return max_value(*a, *b); }

// The concept `std::totally_ordered_with` takes two template parameters to
// check whether the values of the passed types are comparable with the
// operators ==, !=, <, <=, >, and >=.
auto max_value(IsPointer auto a, IsPointer auto b)
  requires std::totally_ordered_with<decltype(*a), decltype(*b)>
{
  return max_value(*a, *b);
}

// max_value() for plain values:
auto max_value(auto a, auto b) { return b < a ? a : b; }

// max_value() for pointers:
auto max_value(IsPointerMultipleConstraints auto a,
               IsPointerMultipleConstraints auto b)
  requires std::totally_ordered_with<decltype(*a), decltype(*b)>
{
  return max_value(*a, *b);
}

// You can use requires clauses or concepts to constrain almost all forms of
// generic code:
//  * Function template:
/// \code
/// template <typename T>
/// requires ...
/// void print(const T&) {
///     ...
/// }
/// \endcode
//
//  * Class templates
/// \code
/// template <typename T>
/// requires ...
/// class MyType {
/// ...
/// }
/// \endcode
//
//  * Alias templates
//  * Variable templates
//  * You can even constrain member functions
//
// However, note that you cannot constrain concepts:
/// \code
/// template <std::ranges::sized_range T>
/// concept IsIntegralValType = std::integral<std::ranges::range_value_t<T>>;
/// \endcode
// Instead, you have to specify this as follows:
/// \code
/// template <typename T>
/// concept IsIntegralValType = std::ranges::sized_range<T> &&
///                             std::integral<std::ranges::range_value_t<T>>;
/// \endcode

// Constraining Alias Templates
template <std::ranges::range T> using ValueType = std::ranges::range_value_t<T>;

// The declaration is equivalent to the following:
template <typename T>
  requires std::ranges::range<T>
using ValueType2 = std::ranges::range_value_t<T>;

// Type ValueType<> is now defined only for types that are ranges:
// ValueType<int> vt1;                  // ERROR
// ValueType<std::vector<int>> vt2;     // int
// ValueType<std::list<double>> vt3;    // double

// Constraining Variable Templates
template <std::ranges::range T>
constexpr bool IsIntegralValType = std::integral<std::ranges::range_value_t<T>>;
// Again, this is equivalent to the following:
template <typename T>
  requires std::ranges::range<T>
constexpr bool IsIntegralValType2 =
    std::integral<std::ranges::range_value_t<T>>;

// The boolean variable template is now defined only for ranges:
/// \code
/// bool b1 = IsIntegralValType<int>;                       // ERROR
/// bool b2 = IsIntegralValType<std::vector<int>>;          // true
/// bool b3 = IsIntegralValType<std::vector<double>>;       // false
/// \endcode

// Constraining Member Function.

template <typename T> class ValueOrColl {
  T value;

public:
  explicit ValueOrColl(const T &val) : value{val} {}

  ValueOrColl(T &&val) : value{std::move(val)} {}

  void print() const { std::cout << value << '\n'; }

  // Constrain member functions.
  // FIXME: Is there any multiple definition issue?
  // EXPLAIN: If type T is collection, the constraint is satisfied so that both
  //          `print()` member functions are available. However, the second
  //          `print()`, which iterates over the elements of the collection, is
  //          preferred by overload resolution, because this member function has
  //          a constraint.
  void print() const
    requires std::ranges::range<T>
  {
    for (const auto &elem : value)
      std::cout << elem << ' ';
    std::cout << '\n';
  }
};

void constrain_member_function_test() {
  ValueOrColl o1 = 42;
  o1.print();

  ValueOrColl o2 = std::vector{1, 2, 3, 4};
  o2.print();
}

// Constraining Non-Type Template Parameters
template <int Val>
concept LessThan10 = Val < 10;

// Or more generic:
template <auto Val>
concept LessThan10Generic = Val < 10;

// This concept can be used as follows:
template <typename T, int Size>
  requires LessThan10<Size>
class MyType {};

// There are multiple reasons why using requirements as constraints can be
// useful:
//  * Constraints help us to understand the restrictions on templates and to get
//    more understandable error messages when requirements are broken.
//  * Constraints can be used to disable generic code for cases where the code
//    does not make sense:
//    - For some types, generic code might compile but not do the right thing.
//    - We might have to fix overload resolution, which decides which operation
//      to call if there are multiple valid options.
//  * Constraints can be used to overload or specialize generic code so that
//    different code is compiled for different types.

// Using concepts to understand code and error messages

template <typename Coll, typename T>
concept SupportsPushBack = requires(Coll c, T v) { c.push_back(v); };

// requires SupportsPushBack<Coll, T>
template <typename Coll, typename T>
  requires std::convertible_to<T, typename Coll::value_type>
void add(Coll &coll, const T &val) {
  coll.push_back(val);
}

} // namespace

TEST(concept_test, test1) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  constrain_member_function_test();

  oss << "42\n"
      << "1 2 3 4 \n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(concept_test, test2) {
  std::vector<int> vec;
  add(vec, 42); // CORRECT

  std::set<int> coll;
  // add(coll, 42);  // ERROR: no push_back() supported by std::set<>
  // clang-format off
  // Error messages:
  // /Users/yuanjunren/Documents/programs/cpp_weekly/cpp20_features/concepts_requirements_constraints_test.cc:261:3: error: no matching function for call to 'add'
  //  261 |   add(coll, 42);  // ERROR: no push_back() supported by std::set<>
  //      |   ^~~
  // / Users/yuanjunren/Documents/programs/cpp_weekly/cpp20_features/concepts_requirements_constraints_test.cc:230:6: note: candidate template ignored: constraints not satisfied [with Coll = std::set<int>, T = int]
  //  230 | void add(Coll &coll, const T &val) {
  //      |      ^
  // / Users/yuanjunren/Documents/programs/cpp_weekly/cpp20_features/concepts_requirements_constraints_test.cc:229:10: note: because 'SupportsPushBack<std::set<int>, int>' evaluated to false
  //  229 | requires SupportsPushBack<Coll, T>
  //      |          ^
  // / Users/yuanjunren/Documents/programs/cpp_weekly/cpp20_features/concepts_requirements_constraints_test.cc:225:32: note: because 'c.push_back(v)' would be invalid: no member named 'push_back' in 'std::set<int>'
  // clang-format on

  std::vector<std::atomic<int>> ai_vec;
  std::atomic<int> ai{42};
  // add(ai_vec, ai); // ERROR: cannot copy/move atomics
  // clang-format off
  // Error messages:
  // /opt/homebrew/opt/llvm/bin/../include/c++/v1/__memory/allocator_traits.h:304:9: error: no matching function for call to 'construct_at'
  //  304 |         _VSTD::construct_at(__p, _VSTD::forward<_Args>(__args)...);
  //
  // when add requires std::convertible_to<T, typename Coll::value_type> to add function. The error msg is as follows:
  // /Users/yuanjunren/Documents/programs/cpp_weekly/cpp20_features/concepts_requirements_constraints_test.cc:230:10: note: because 'std::convertible_to<std::atomic<int>, typename vector<atomic<int> >::value_type>' evaluated to false
  //  230 | requires std::convertible_to<T, typename Coll::value_type>
  //      |          ^
  // / opt/homebrew/opt/llvm/bin/../include/c++/v1/__concepts/convertible_to.h:27:26: note: because 'is_convertible_v<std::atomic<int>, std::atomic<int> >' evaluated to false
  //   27 | concept convertible_to = is_convertible_v<_From, _To> && requires { static_cast<_To>(std::declval<_From>()); };
  //      |                          ^
  // 1 error generated.
  // ninja: build stopped: subcommand failed.
  // clang-format on
}
