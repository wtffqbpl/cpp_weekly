#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <ranges>
#include <string>
#include <vector>
#include <version>

#include "internal_check_conds.h"

#if support_format
#include <format>
#endif

// C++20 provides a new way to deal with ranges. it provides support for
// defining and using ranges and sub-ranges as single objects, such as passing
// them as a whole as single arguments instead of dealing with two iterators.
// C++20 provides several new features and utilities for dealing with ranges:
//  * New overloads or variations of standard algorithms that take a range as a
//    single argument.
//  * Several utilities for dealing with range objects:
//    - Helper functions for creating range objects.
//    - Helper functions for dealing with range objects.
//    - Helper types for dealing with range objects.
//    - Concepts for ranges.
//  * Lightweight ranges, called views, to refer to (a subset of) a range with
//    optional transformation of the value.
//  * Pipelines as a flexible way to compose the processing of ranges and views.

namespace {

void range_basic_test() {
  std::vector coll{25, 42, 2, 0, 122, 5, 7};

  // C++20 introduces the concept of a range, which is a single object that
  // represents a sequence of values. Any container can be used as such a range.
  // The C++ standard library provides all features for dealing with ranges in
  // special namespaces:
  //  * Most of them are provided in the namespace std::ranges.
  //  * A few of them are provided in std::views, which is an alias of
  //    std::ranges::views.
  std::ranges::sort(coll);

  std::ranges::copy(coll, std::ostream_iterator<int>(std::cout, ", "));
  std::cout << '\n';
}

// The new standard algorithms for ranges declare range parameters as template
// parameters. To specify and validate the necessary requirements when dealing
// with these range parameters, C++20 introduces serval range concepts. In
// addition, utilities help you to apply these concepts.

// For example, the possible implementation of std::ranges::sort definition:
/// \code
/// template <std::ranges::random_access_range R,
///           typename Comp = std::ranges::less,
///           typename Proj = identity>
/// requires std::sortable<std::ranges::iterator_t<R>, Comp>
/// ... sort(R&& r, Comp comp = {}, Proj proj = {}) { /* ... */ }
/// \endcode

template <typename Coll> void print(const Coll &coll) {
  for (const auto &elem : coll) {
    std::cout << elem << ' ';
  }
  std::cout << '\n';
}

template <typename Coll>
  requires requires { Coll::value_type; }
void print(const Coll &coll) {
  std::ranges::copy(
      coll, std::ostream_iterator<typename Coll::value_type>(std::cout, " "));
  std::cout << '\n';
}

void test2() {
  std::vector<std::string> coll{"Rio", "Tokyo", "New York", "Berlin"};

  std::ranges::sort(coll);    // sort elements
  std::ranges::sort(coll[0]); // sort character in first element
  print(coll);

  int arr[] = {42, 0, 8, 15, 7};
  std::ranges::sort(arr); // sort values in array
  print(arr);

  // clang-format off
  // std::list<std::string> coll2{"New York", "Rio", "Tokyo"};
  // std::ranges::sort(coll2);
  // ERROR MSG: because 'std::list<std::string> &' does not satisfy 'random_access_range'
  // clang-format on
}

// clang-format off
// Basic range concepts
// ----------------------------------------------------------------------------------------------------
// |  Concept (in std::ranges)  |   Requires                                                          |
// |----------------------------+---------------------------------------------------------------------|
// |  range                     | Can be iterated from begin to end                                   |
// |  output_range              | Range of elements to write values to                                |
// |  input_range               | Range to read element values from                                   |
// |  forward_range             | Range you can iterate over the elements multiple times              |
// |  bidirectional_range       | Range you can iterate over the elements forward and backward        |
// |  random_access_range       | Range with random access (jump back and forth between elements)     |
// |  contiguous_range          | Range with all elements in contiguous memory                        |
// |  sized_range               | Range with constant-time size()                                     |
// | view                       | Range that is cheap to copy or move and assign                      |
// | viewable_range             | Range that can be converted to a view (with std::ranges::all())     |
// | borrowed_range             | Range with iterators that are not tied to the lifetime of the range |
// | common_range               | Ranges where begin and end (sentinel) have the same type            |
// |---------------------------------------------------------------------------------------------------
// clang-format on

//  * std::ranges::range is the base concept of all other range concepts (all
//    other concepts subsume this concept).
//  * output_range as well as the hierarchy of input_range, forward_range,
//    bidirectional_range, random_access_range, and contiguous_range map to
//    corresponding iterator categories and build a corresponding subsumption
//    hierarchy.
//  * std::ranges::contiguous_range is a new category of ranges/iterator for
//    which it is guaranteed that the elements are sorted in contiguous memory.
//    Code can benefit from this fact by using raw pointers to iterate over
//    the elements.
//  * std::ranges::sized_range is independent from other constraints except that
//    it is a range.

} // namespace

TEST(range_view_test, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  range_basic_test();
  oss << "0, 2, 5, 7, 25, 42, 122, \n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, test2) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  test2();

  oss << "Beilnr New York Rio Tokyo \n"
         "0 7 8 15 42 \n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

namespace views_test {
// Views are lightweight ranges that are cheap to create and copy/move.
// Views can:
//  * Refer to ranges and subranges
//  * Own temporary ranges
//  * Filter out elements
//  * Yield transformed values of the elements
//  * Generate a sequence of values themselves
// Views are usually used to process, on an ad-hoc basis, a subset of the
// elements of an underlying range and/or their values after some optional
// transformation. For example, you can use a view to iterate over only the
// first five elements of a range as follows.

void test1() {
  std::vector coll{1, 2, 3, 4, 5, 6, 7};

  for (const auto &elem : std::views::take(coll, 5)) {
    // ...
  }

  // Pipelines of ranges and views
  for (const auto &elem : coll | std::views::take(5)) {
    // ...
  }

  // view with elements of coll that multiples of 3:
  (void)std::views::filter(coll, [](auto elem) { return elem % 3 == 0; });

  // view with squared elements of coll:
  (void)std::views::transform(coll, [](auto elem) { return elem * elem; });

  // view with first three elements of coll:
  (void)std::views::take(coll, 3);

  // view with first three squared values of the elements in coll that are
  // multiples of 3:
  auto v = coll | std::views::filter([](auto elem) { return elem % 3 == 0; }) |
           std::views::transform([](auto elem) { return elem * elem; }) |
           std::views::take(3);
}

void test2() {
  std::vector coll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

  for (const auto &elem :
       coll | std::views::filter([](auto elem) { return elem % 3 == 0; }) |
           std::views::transform([](auto elem) { return elem * elem; }) |
           std::views::take(3)) {
    std::cout << elem << ' ';
  }
}

void test3() {
  // map of composers (mapping their name to their year of birth):
  std::map<std::string, int> composers{
      {"Bach", 1685},        {"Mozart", 1765}, {"Beethoven", 1770},
      {"Tchaikovsky", 1840}, {"Chopin", 1810}, {"Vivaldi ", 1678},
  };

  // iterate over the names of the first three composers born since 1700:
  for (const auto &elem :
       composers | std::views::filter([](const auto &y) { // since 1700
         return y.second >= 1700;
       }) | std::views::take(3) // first three
           | std::views::keys   //  keys/names only
  ) {
    std::cout << "- " << elem << '\n';
  }
}

// Views can also generate a sequence of values themselves.

void test4() {
  for (auto val : std::views::iota(1, 11)) {
    std::cout << val << ", ";
  }
  std::cout << '\n';
}

// Views on lvalues usually have reference semantics. This means that, in
// principle, views can be used for both reading and writing.

void test5() {
  std::vector coll{4, 3, 2, 1, 1, 2, 3, 4, 5, 4, 3, 2, 1};

  // Only sort the first five elements of coll as follows.
  std::ranges::sort(std::views::take(coll, 5));

  // or as follows:
  std::ranges::sort(coll | std::views::take(5));

  // clang-format off
  // This means that usually:
  //  * If elements of the referred range are modified, the elements of the view were modified.
  //  * If elements of the view are modified, the elements of the referred range were modified.
  // clang-format on

  using value_type = typename decltype(coll)::value_type;
  std::ranges::copy(coll, std::ostream_iterator<value_type>(std::cout, ", "));
  std::cout << '\n';

  // Lazy Evaluation
  // Besides having reference semantics, views use lazy evaluation. This means that views do their
  // processing for the next element when an iterator to the view calls begin() or ++ or the value
  // of the element is requested:
  auto v = coll | std::views::take(5); // Neither goes to the first element nor to its value
  // ...
  auto pos = v.begin(); // goes to the first element
  // ...
  std::cout << *pos; // goes to its value
  // ...
  ++pos; // goes to the next element
  // ...
  std::cout << *pos; // goes to its value

  // In addition, some views use caching. If going to the first element of a view with begin() needs
  // some computation (because we skip leading elements), a view might cache the return value of
  // begin() so that the next time we call begin(), we do not have to compute the position of the
  // first element again.
  // FIXME: Some questions
  //  * Which views could use cache?
  //  * How to enable caching for specified views?
  //  * How to disable caching for specified views?

  // However, this optimization has significant consequences:
  //  * You might not be able to iterate over views when they are const.
  //  * Concurrent iterations can cause undefined behavior even if we do not modify anything.
  //  * Early read access might invalidate or change the later iterations over elements of views.
  //
  // FIXME: NOTE THAT THE STANDARD VIEWS ARE CONSIDERED HARMFUL WHEN MODIFICATIONS HAPPEN:
  //  * INSERTING OR REMOVING ELEMENTS IN THE UNDERLYING RANGE MAY HAVE SIGNIFICANT IMPACT ON THE
  //    FUNCTIONALITY OF A VIEW. AFTER SUCH MODIFICATION, A VIEW MIGHT BEHAVE DIFFERENTLY OR EVEN
  //    NO LONGER BE VALID.
  //    THEREFOR, IT IS STRONGLY RECOMMENDED TO USE VIEWS RIGHT BEFORE YOU NEED THEM. CREATE VIEWS
  //    TO USE THEM AD HOC. IF MODIFICATIONS HAPPEN BETWEEN INITIALIZING A VIEW AND USING IT, CARE
  //    HAS TO BE TAKEN.
}

} // namespace views_test

TEST(range_view_test, view_test1) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  views_test::test2();

  oss << "9 36 81 ";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, view_test2) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  views_test::test3();

  oss << "- Beethoven\n"
         "- Chopin\n"
         "- Mozart\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif
  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, view_test3) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  views_test::test5();

  oss << "1, 1, 2, 3, 4, 2, 3, 4, 5, 4, 3, 2, 1, \n"
         "11";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

namespace sentinels_test {

// sentinels: It represents the end of the range. In programming, a sentinel is a special value that
//  marks an end or termination. Typical examples are:
//  * The null terminator '\0' as the end of a character sequence.
//  * nullptr marking the end of a linked-list.
//  * -1 to mark the end of a list of non-negative integers.
// In the ranges library, sentinels define the end of a range. In the traditional approach of the
// STL, sentinels would be the end iterators, which usually have the same type as the iterators that
// iterate over a collection. However, with C++20 ranges, having the same type is no longer
// required.

//  The requirement that end iterators have to have the same type as the iterators that define the
//  beginning of a range and that are used to iterate over the elements causes some drawbacks.
//  Creating an end iterator may be expensive or might not even be possible:
//  * If we want to use a C string or string literal as a range, we first have to compute the end
//    iterator by iterating over the characters until we find '\0'. Thus, before we use the string
//    as a range, we have already done the first iteration. Dealing then with all the characters
//    would need a second iteration.
//  * In general, this principle applies if we defined the end of a range with a certain value. If
//    we need an end iterator to deal with the range, we first have to iterate over the whole range
//    to find its end.
//  * Sometimes, iterating twice (once to find the end and then once to process the elements of the
//    range) is not possible. This applies to ranges that use pure input iterators, such as using
//    input streams that we read from as ranges. To compute the end of the input (maybe EOF), we
//    already have to read the input. Reading the input again is not possible or will yield
//    different values.
//  The generalization of end iterators as sentinels solves this dilemma. C++20 ranges support
//  sentinels (end iterators) of different types. They can signal "until '\0'", "until EOF", or
//  until any other value. They can even signal "there is no end" to define endless ranges and "hey,
//  iterating iterator, check yourself whether there is the end."

// By relaxing the requirement that sentinels (end iterators) now have to have the same type as
// iterating iterators, we gain a couple of benefits:
//  * We can skip the need to find the end before we start to process. We do both together: process
//    the values and find the end while iterating.
//  * For the end iterator, we can use types that disable operations that cause undefined behavior
//    (such as calling the operator*, because there is no value at the end). We can use this feature
//    signal an error at compile time when we try to dereference an end iterator.
//  * Defining an end iterator becomes easier.

struct NullTerm {
  bool operator==(auto pos) const {
    return *pos == '\0'; // end is where iterator points to '\0'
  }

  // FIXME: When we define an operator!= with auto specifier, then we'll meet compile error. Please
  //        Find why occurs this error.
  // bool operator!=(const T &pos) const { return *pos != '\0'; }
  // The following operator!= is correct, that wierd
  // template <typename T>
  // bool operator!=(const T &pos) const { return *pos != '\0'; }
};

void test1() {
  const char *raw_str = "hello world";

  // iterate over the range of the beginning of raw_str and its end:
  for (auto pos = raw_str; pos != NullTerm{}; ++pos) {
    std::cout << ' ' << *pos;
  }
  std::cout << '\n';

  // call range algorithm with iterator and sentinel:
  std::ranges::for_each(raw_str,    // begin of range
                        NullTerm{}, // end is null terminator
                        [](char c) { std::cout << ' ' << c; });
  std::cout << '\n';

  // There is a concepts restraints for last
  // [iterator.concept.sentinel]
  // template<class _Sp, class _Ip>
  // concept sentinel_for =
  //   semiregular<_Sp> &&
  //   input_or_output_iterator<_Ip> &&
  //   __weakly_equality_comparable_with<_Sp, _Ip>;
  //
  // The definition of std::ranges::for_each may like this:
  // template <input_iterator _Iter,
  //           sentinel_for<_Iter> _Sent,
  //           class _Proj = identity,
  //           indirectly_unary_invocable<projected<_Iter, _Proj>> _Func>
  // constexpr for_each_result<_Iter, _Func>
  // for_each(_Iter __first, _Sent __last, _Func __func, _Proj __proj = {}) const {
  //  // ...
  // }
}

// Range Definitions with Sentinels and Counts
// Ranges can be more than just containers or a pair of iterators. Ranges can be defined by:
//  * A begin iterator and an end iterator of the same type.
//  * A begin iterator and a sentinel (an end marker of maybe a different type).
//  * A begin iterator and a count.
//  * Array
//
// In addition, there are several utilities for defining ranges defined by iterators and sentinels
// or counts, and these are introduced in the following subsections.

// Sub-ranges
// To define ranges of iterators and sentinels, the ranges library provides type
// std::ranges::subrange<>.
// A sub-range is the generic type that can be used to convert a range defined by an iterator and a
// sentinel into a single object that represents this range. In fact, the range is even a view,
// which internally, just stores the iterator and the sentinel. This means that sub-ranges have
// reference semantics and are cheap to copy.
// Note that sub-ranges are not always `common ranges`, meaning that calling `begin()` and `end()`
// for them may yield different types. Sub-ranges just yield what was passed to define the range.
void subrange_test() {
  const char *raw_str = "hello world";

  // define a range of a raw string and a null terminator:
  std::ranges::subrange raw_str_range{raw_str, NullTerm{}};

  // use the range in an algorithm:
  std::ranges::for_each(raw_str_range, [](char c) { std::cout << ' ' << c; });
  std::cout << '\n';

  // range-based for loop also supports iterator/sentinel:
  for (char c : raw_str_range) {
    std::cout << ' ' << c;
  }
  std::cout << std::endl;

  // use std::ranges::copy
  std::ranges::copy(raw_str_range, std::ostream_iterator<char>(std::cout, " "));
  std::cout << '\n';
}

// We can make this approach even more generic by defining a class template where you can specify
// the value that ends a range.
template <auto End> struct EndValue {
  bool operator==(auto pos) const {
    return *pos == End; // end is where iterator points to End.
  }
};

void generic_sentinel_test() {

  std::vector coll = {42, 8, 0, 15, 7, -1};

  // define a range referring to coll with the value 7 as end:
  std::ranges::subrange range{coll.begin(), EndValue<7>{}};

  // sort the elements of this range
  std::ranges::sort(range);

  // print the elements of the range:
  std::ranges::for_each(range, [](auto val) { std::cout << ' ' << val; });
  std::cout << '\n';

  // print all elements of coll up to -1:
  std::ranges::for_each(coll.begin(), EndValue<-1>{}, [](auto val) { std::cout << ' ' << val; });
  std::cout << '\n';
}

// Ranges of Begin and a Count
// The ranges library provides multiple ways of dealing with ranges defined as beginning and  count.
// The most convenient way to create a range with a begin iterator and a count is to use the range
// adaptor `std::views::counted()`. It creates a cheap view to the first n elements of a begin
// iterator/pointer.
void sentinels_begin_count_creation_test() {
  std::vector coll{1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto pos = std::ranges::find(coll, 5);
  if (std::ranges::distance(pos, coll.end()) >= 3) {
    for (auto val : std::views::counted(pos, 3)) {
      std::cout << val << ' ';
    }
  }

  // std::views::counted(pos, 3): indicates a range which starts from pos, and there are 3 elements.
  // Note that counted() does not check whether there are elements, it is up to the programmer to
  // ensure that the code is valid. Therefore, with `std::views::distance()` , we can check whether
  // there are enough elements.
  // NOTE: THAT THIS CHECK CAN BE EXPENSIVE IF YOUR COLLECTION DOES NOT HAVE RANDOM-ACCESS ITERATORS.
  // for std::views::counted, the count may be 0, which means that the range is empty.
  // Note that you should use std::views::counted() only when you really have an iterator and a
  // count. If you already have a range and want to deal with the first n elements only, use
  // std::views:take().
}

} // namespace sentinels_test

TEST(range_view_test, sentinels_test1) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  sentinels_test::test1();

  oss << " h e l l o   w o r l d\n"
         " h e l l o   w o r l d\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, sentinels_test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  sentinels_test::subrange_test();

  oss << " h e l l o   w o r l d\n"
         " h e l l o   w o r l d\n"
         "h e l l o   w o r l d \n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, sentinels_test3) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  sentinels_test::generic_sentinel_test();
  oss << " 0 8 15 42\n"
         " 0 8 15 42 7\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, sentinels_test4) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  sentinels_test::sentinels_begin_count_creation_test();

  oss << "5 6 7 ";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

namespace projections_test {
// Many algorithms for ranges usually have an additional optional template parameter, a projection.
// The optional additional parameter allows you to specify a transformation (projection) for each
// element before the algorithm processes it further.
// For example:
template <std::ranges::random_access_range R,
          typename Comp = std::ranges::less,
          typename Proj = std::identity>
requires std::sortable<std::ranges::iterator_t<R>, Comp, Proj>
void sort_impl(R&& r, Comp comp = {}, Proj proj = {});

void basic_proj_test() {
  std::vector coll{1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
  std::ranges::reverse(coll);
  std::ranges::sort(coll, std::ranges::less{}, [] (auto val) { return std::abs(val); });

  std::ranges::for_each(coll, [](auto val) { std::cout << val << ' '; });
  std::cout << '\n';
}

// User-defined projections simply have to take one parameter and return a value for the transformed
// parameters.
// Projections in C++ ranges algorithms provide a way to transform and extract specific elements
// from the input range before applying the algorithm. A projection is essentially a function or
// functor that, when applied to an element in the range, transforms it into a value that is then
// used by the algorithm for comparison, modification, or other purposes.
// Projections are often used in algorithms that require comparing or operating on specific
// properties of elements rather then the elements themselves. Projections are especially useful
// when working with structures or complex types where you want to focus on a particular field or
// property.
// Projections are commonly used in various algorithms like `std::ranges::sort`,
// `std::ranges::max_element`, `std::ranges::min_element`, and others. They provide a flexible way
// to customize the behavior of algorithms based on specific properties of the elements.
// The use of projections enhances code expressiveness and allows for more concise and readable
// code when working with complex data structures or when focusing on specific attributes of the
// elements in a range.

struct Person {
  std::string name;
  int age;
};

void test2() {
  std::vector<Person> people = {{"Alice", 25}, {"Bob", 30}, {"Charlie", 20}};

  // Using a projection to sort based on age
  // std::ranges::sort(people, [](const Person &p1, const Person &p2) { return p1.age < p2.age; });
  // std::ranges::sort(people, std::ranges::less{}, [](const Person &person) { return person.age; });
  std::ranges::sort(people, {}, [](const Person &person) { return person.age; });
  // The second argument `{}` represents an empty comparator, allowing the default comparator
  // (`operator<`) to be used for the sorting based on the projected values.

  // Display the sorted result
  for (const auto &person : people) {
    std::cout << person.name << " - " << person.age << " years old." << std::endl;
  }
}

// Projections in std::ranges Algorithms
// Purpose:
//  * Projections provide a way to selectively access or modify specific parts of elements within
//    a range during algorithm operations.
//  * They enable algorithms to work with particular properties of elements, rather than always
//    using the elements themselves directly.

// Mechanism:
//  * Function objects: Projections are typically implemented using function objects (functors) that
//    take an element as input and return the desired property or a modified version of it.
//  * Algorithm as an argument: Many `std::ranges` algorithms accept a projection as an optional
//    argument.
//
// Benefits of Projections:
//  * Flexibility: Customize algorithm behavior without modifying element types.
//  * Performance: Can potentially improve performance by avoiding unnecessary data copies.
//  * Readability: Make code more expressive by focusing on relevant properties.
//  * Genericity: Write algorithms that work with different properties without code duplication.
//
// Key Points:
//  * Projections are a powerful tool for customizing and optimizing algorithm operations in
//    std::ranges.
//  * They enhance code clarity, usability, and adaptability.
//  * Consider using projections whenever you need to work with specific aspects of elements
//    within ranges.

void test3() {
  std::vector numbers{1, 2, 3, 4};

  // Square each number using a projection.
  std::ranges::transform(numbers, numbers.begin(), [](auto x) { return x * x; });

  // Output
  std::ranges::for_each(numbers, [](auto x) { std::cout << x << ' '; });
  std::cout << '\n';
}

}

TEST(range_view_test, projection_test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  projections_test::basic_proj_test();

  oss << "-1 1 -2 2 -3 3 -4 4 -5 5 \n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, projection_test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  projections_test::test2();

  oss << "Charlie - 20 years old.\n"
         "Alice - 25 years old.\n"
         "Bob - 30 years old.\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

TEST(range_view_test, projection_test3) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  projections_test::test3();

  oss << "1 4 9 16 \n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

namespace ranges_utilities_test {
// To make it easy to program against all the different kinds of ranges, the ranges library provides
// the following utilities:
//  * Generic functions that, for example, yield iterators or the size of a range.
//  * Type functions that, for example, yield the type of iterator or the type of the element.

// Assume that we want to implement an algorithm that yields the maximum value in a range:
template <std::ranges::input_range Range>
std::ranges::range_value_t<Range> max_value(const Range &rg) {
  if (std::ranges::empty(rg))
    return std::ranges::range_value_t<Range>{};

  auto pos = std::ranges::begin(rg);
  auto max = *pos;
  while (++pos != std::ranges::end(rg)) {
    if (*pos > max)
      max = *pos;
  }
  return max;
}

void test1() {
  std::vector coll{3, 2, 1, 0, 4, 3, 2, 1, 0, 5, 4, 3, 2, 1, 0};

  auto max_val = max_value(coll);

#if support_format
  std::cout << std::format("Max value = {}\n", max_val);
#else
  std::cout << "Max value = " << max_val << '\n';
#endif
}

}

TEST(range_view_test, range_utilities_test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  ranges_utilities_test::test1();

  oss << "Max value = 5\n";
  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_EQ(oss.str(), act_output);
}

// Limitations and Drawbacks of Ranges
// In C++20, ranges also have some major limitations and drawbacks that should be mentioned in a
// general introduction to them:
//
//  * There is no ranges support for numeric algorithms yet. To pass a range to a numeric algorithm,
//    you have to pass `begin()` and `end()` explicitly.
//    std::accumulate(coll.begin(), coll.end(), 0L);
//
//  * There is no support for ranges for parallel algorithms yet.
//    std::sort(std::execution::par, coll.begin(), coll.end()); // OK
//
//  * Several traditional APIs for ranges defined by a `begin` iterator and an `end` iterator
//    require that the iterators have the same type (e.g., this applies to containers and the
//    algorithms in the namespace std). You might have to harmonize their types with
//    `std::views::common()` or `std::common_iterator`.
//
//  * `cbegin()` and `cend()` functions, which were designed to ensure that you cannot
//    (accidentally) modify the element you iterate over, are broken for views that refer to
//    non-const objects.
//
//  * When views refer to containers, their propagation of constness may be broken.
//
//  * Ranges lead to a significant namespace mess. For example, look at the following declaration
//    of std::find() but with all standard names fully qualified:
template <std::ranges::input_range Rg,
          typename T,
          typename Proj = std::identity>
requires std::indirect_binary_predicate<std::ranges::equal_to,
                                        std::projected<std::ranges::iterator_t<Rg>, Proj>,
                                        const T*>
constexpr std::ranges::borrowed_iterator_t<Rg>
find_impl(Rg &&rg, const T &value, Proj proj = {});
// It is really not easy to know which namespace has to be used where.
// In addition, we have some symbols in both the namespace `std` and the namespace `std::ranges`
// with slightly different behavior. In the declaration above, `equal_to` is such an example. You
// could also use `std::equal_to`, but in general, utilities in `std::ranges` provide better support
// and are more robust for corner cases.