#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <ranges>
#include <string>
#include <vector>

#include "internal_check_conds.h"

// C++20 provides a new way to deal with ranges. it provides support for
// defining and using ranges and subranges as single objects, such as passing
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