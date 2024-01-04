#include <gtest/gtest.h>
#include <atomic>
#include <concepts>
#include <ranges>
#include <set>
#include <type_traits>
#include <vector>

#include "internal_check_conds.h"

namespace concept_test1 {

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

template <typename Coll, typename T>
  requires std::floating_point<T>
void add(Coll &coll, const T &val) {
  // ... special code for floating-point values
  coll.push_back(val);
}

// Because we use a concept that applies to a single template parameter, we can
// also use the shorthand notation like this:
template <typename Coll, std::floating_point T>
void add(Coll &coll, const T &val) {
  coll.push_back(val);
}

// Alternatively, we can use `auto` parameters:
/// \code
/// void add(auto &coll, const std::floating_point auto &val) {
///   coll.push_back(val);
/// }
/// \endcode

// Overload resolution also prefers overloads or specializations that have
// constraints over those that have fewer or no constraints.

// It is better to define a concept that yields whether a type can be converted
// to another type without narrowing, which is possible in a short tricky
// requirement:
template <typename From, typename To>
concept ConvertWithoutNarrowing =
    std::convertible_to<From, To> && requires(From &&x) {
      {
        std::type_identity_t<To[]>{std::forward<From>(x)}
      } -> std::same_as<To[1]>;
    };

// EXPLAINS:
// std::convertible_to<From, To>:
//  * This part checks whether type `From` is convertible to type `To`. It is a
//    standard library concept that verifies if an object of type `From` can be
//    converted to type `To` using implicit conversion.
//  requires (From &&x) {...}
//    * This introduces a requires-clause, specifying additional requirements
//      that must be satisfied for the concept to be true. It uses a constrained
//      requires expression.
//  {std::type_identity_t<To[]>{std::forward<From>(x)}} -> std::same_as<To[1]>;
//    * This is the body of the requires-clause, defining a constrained
//      expression.
//    * std::type_identity_t<To[]> is used to form a type that is an identity
//      for `To[]`. This is used to prevent array-to-pointer decay during the
//      expression evaluation.
//    * `std::forward<From>(x): is used to forward the object `x` with its
//      original value category (lvalue or rvalue).
//    * The expression `{...}` then attempts to create an array of type `To[]`
//      from the forwarded `x`.
//    * The `-> std::same_as<To[1]>` specifies that the type of the resulting
//      array must be exactly `To[1]`. This ensures that the conversion does
//      not narrow the array.
// In summary, the concept `ConvertWithoutNarrowing` checks whether a conversion
// from type `From` to type `To` is possible and, additionally, ensures that the
// conversion does not involve narrowing. The array size is explicitly checked
// to be one (`To[1]`), preventing loss of information during the conversion.
//  This concept is useful in scenarios where you want to ensure that a
//  conversion between types maintains the size or precision of the original
//  value, and it's not subject to narrowing conversions.
//
// In C++17, you don't have the concept keyword, so you cannot express
// constraints directly. However, you can achieve similar functionality using
// SFINAE (Substitution Failure is Not An Error) techniques and type traits.
// Here's an attempt to express the same constraint without using the `concept`
// feature:
// clang-format off
template <typename From, typename To>
struct ConvertWithoutNarrowingHelper {
private:
  // Check if `From` is convertible to `To`.
  template <typename F, typename T>
  static auto testConvertible(const F &from) -> decltype(static_cast<T>(from), std::true_type{});

  template <typename F, typename T>
  static auto testConvertible(...) -> std::false_type;

  // Check if the expression creates an array of type To[1].
  template <typename F, typename T>
  static auto testExpression(const F &from) -> decltype(T{from}, std::true_type{});

  template <typename F, typename T>
  static auto testExpression(...) -> std::false_type;

public:
  static constexpr bool value =
      decltype(testConvertible<From, To>(std::declval<From>()))::value &&
      decltype(testExpression<From, To>(std::declval<From>()))::value;
};

template <typename From, typename To>
using ConvertWithoutNarrowingUnderC20 = std::bool_constant<ConvertWithoutNarrowingHelper<From, To>::value>;
// clang-format on
// std::bool_constant creates a compile-time boolean constant reflecting the
// result of the checks.
// You can use `ConvertWithoutNarrowingUnderC20<From, To>::value` to check
// whether the conversion satisfies the conditions. Please note that this
// approach relies on SFINAE, and the error messages might not be as
// user-friendly as those provided by concepts in C++20.
//
// clang-format off
// Another version:
template <typename From, typename To>
struct ConvertWithoutNarrowingV3 {
private:
  template <typename T, typename U>
  static constexpr std::false_type check(U*);

  template <typename T, typename U>
  static constexpr std::true_type check(T*);

  static constexpr bool convertible = std::is_convertible_v<From, To>;
  static constexpr bool noNarrowing = decltype(check<To[1], std::type_identity_t<To[]>{std::declval<From>()}>())::value;

public:
  static constexpr bool value = convertible && noNarrowing;
};
template <typename From, typename To>
using ConvertWithoutNarrowingV3_v = std::bool_constant<ConvertWithoutNarrowingV3<From, To>::value>;
// clang-format on

// We can then use this concept to formulate a corresponding constraint:
template <typename Coll, typename T>
  requires ConvertWithoutNarrowing<T, typename Coll::value_type>
void add(Coll &coll, const T &val) {
  coll.push_back(val);
}

template <typename Coll>
concept SupportsPushBackV2 = requires(Coll coll, Coll::value_type val) {
    coll.push_back(val);
};
// Note that we do not have to use `typename` here to use Coll::value_type.
// Since C++20, `typename` is no longer required when it is clear by the context
// that a qualified member must be a type.

template <typename Coll>
concept SupportsPushBackV3 = requires(Coll coll) {
    coll.push_back(std::declval<typename Coll::value_type&>());
};
// Note that the & is important here. Without &, we would only require that we
// can insert an rvalue using move semantics. With &, we create an lvalue so
// that we require that push_back() copies.

template <typename Coll>
concept SupportsPushBackV4 = requires(Coll coll) {
  coll.push_back(std::declval<std::ranges::range_value_t<Coll>>());
};
// In general, using std::ranges::range_value_t<> makes code more generic when
// the element type of a collection is needed.

// We can also use the concept SupportsPushBack directly in an `if constexpr` condition:

void foo(auto &coll, const auto &val) {
  if constexpr (SupportsPushBackV3<decltype(coll)>) {
    coll.push_back(val);
  } else {
    coll.insert(val);
  }
}

void foo2(auto &coll, const auto &val) {
  if constexpr (requires { coll.push_back(val); }) {
    coll.push_back(val);
  } else {
    coll.insert(val);
  }
}
// Concepts versus Variable Templates
// Concepts have the following benefits:
//  * They subsume.
//  * They can be used as type constraints directly in front of template
//    parameters or auto.
//  * They can be used with a compile-time if when using ad-hoc requirements.

// Inserting Single and Multiple values
#if 1
template <SupportsPushBackV3 Coll, std::ranges::input_range T>
void add(Coll &coll, const T &val) {
  coll.push_back(coll.end(), val.begin(), val.end());
}

#else

template <SupportsPushBackV3 Coll, std::ranges::input_range T>
void add(Coll &coll, const T &val) {
  coll.push_back(coll.end(), std::ranges::begin(val), std::ranges::end(val));
}
#endif

} // namespace concept_test1

TEST(concept_test, test1) {
  using namespace concept_test1;

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
  using namespace concept_test1;

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

namespace concept_test2 {

// concept for container with push_back():
template <typename Coll>
concept SupportsPushBack = requires(Coll coll, Coll::value_type val) {
  coll.push_back(val);
};

// concept to disable narrowing conversion:
template <typename From, typename To>
concept ConvertsWithoutNarrowing =
  std::convertible_to<From, To> &&
      requires (From &&x) {
        {
          std::type_identity_t<To[]>{std::forward<From>(x)}
        } -> std::same_as<To[1]>;
};

// add() for single value:
template <typename Coll, typename T>
requires ConvertsWithoutNarrowing<T, typename Coll::value_type>
void add(Coll &coll, const T &val) {
  if constexpr (SupportsPushBack<Coll>)
  {
    coll.push_back(val);
  } else {
    coll.insert(val);
  }
}

// add() for multiple values:
template <typename Coll, std::ranges::input_range T>
requires ConvertsWithoutNarrowing<std::ranges::range_value_t<T>,
                                  typename Coll::value_type>
void add(Coll &coll, const T &val) {
  if constexpr (SupportsPushBack<Coll>) {
    coll.insert(coll.end(), std::ranges::begin(val), std::ranges::end(val));
  } else {
    coll.insert(std::ranges::begin(val), std::ranges::end(val));
  }
}

void test() {
  std::vector<int> i_vec;
  add(i_vec, 42);

  std::set<int> i_set;
  add(i_set, 42);

  short s = 42;
  add(i_vec, s);

  long long ll = 42;
  // add(i_vec, ll);  // ERROR: narrowing
  // add(i_vec, 7.7); // ERROR: narrowing

  std::vector<double> d_vec;
  add(d_vec, 0.7);
  add(d_vec, 0.7f);
  // add(d_vec, 7);   // ERROR: narrowing

  // insert collections:
  add(i_vec, i_set);
  add(i_set, i_vec);

  // can even insert raw array:
  int vals[] = {0, 8, 18};
  add(i_vec, vals);
  // add(d_vec, vals); // ERROR: narrowing

}

} // namespace concept_test2

TEST(concept_test, test3) {
  using namespace concept_test2;

  test();
}

namespace concept_test3 {

// Semantic Constraints
// Concepts might check both syntactic and semantic constraints:
//  * Syntactic constraints: means that at compile time, we can check whether
//    certain functional requirements are satisfied ("Is a specific operation
//    supported?" or "Does a specific operation yield a specific type?").
//  * Semantic constraints: mean that certain requirements are satisfied that
//    can only be checked at runtime ("Does an operation have the same effect?"
//    or "Does the same operation performed for a specific value always yield
//    the same result?").
// Sometimes, concepts allow programmers to convert semantic constraints into
// syntactic constraints by providing an interface to specify that a semantic
// constraint is fulfilled or is not fulfilled.
//
// Examples of Semantic Constraints
// std::ranges::sized_range: it guarantees that the number of elements in a
// range can be computed in constant time (either by calling the member size()
// or by computing the difference between the beginning and the end).

// Concepts subsume, which means that a concept can be a subset of another
// concept, so that in overload resolution the more constrained concept is
// preferred.

// Concepts are more than just expressions that evaluate Boolean results at
// compile time. You should usually prefer them over type traits and other
// compile-time expressions. Concepts have a couple of benefits:
//  * They subsume.
//  * They can be used as type constraints directly in front of template
//    parameters or auto.
//  * They can be used with the compile-time if (if constexpr) as introduced
//    before.

// requires Clauses
// A requires clause uses the keyword requires with a compile-time Boolean
// expression to restrict the availability of the template. The Boolean
// expression can be:
//  * An ad-hoc Boolean compile-time expression.
//  * A concept
//  * A requires expression
// All constraints can also be used whether a Boolean expression can be used
// (especially as an if constexpr condition).

template <typename T>
requires (sizeof(T) > 4)                          // ad-hoc Boolean expression
          && requires { typename T::value_type; } // requires expression
          && std::input_iterator<T>               // concept
void foo(T x)
{}

}