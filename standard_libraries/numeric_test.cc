#include "internal_check_conds.h"
#include <array>
#include <gtest/gtest.h>
#include <iterator>
#include <numeric>

// cppreference: https://en.cppreference.com/w/cpp/header/numeric

// if [first, last) is not empty, computes the differences between the second
// and first of each adjacent pair of its elements and writes the differences to
// the range beginning at `d_first + 1`. An unmodified copy of `*first` is
// written to
// `*d_first`. The default operator for calculate the differences is `-`(minus
// operator). If the operator is specified, then use the given binary function.
/// \code
/// template <typename InputIt, typename OutputIt>
/// constexpr OutputIt adjacent_difference(InputIt first, InputIt last,
///                                        OutputIt d_first);
/// \endcode

//
//

// performed first
// *d_first = *first;
//
// performed after the initial assignment, might not be sequenced.
// *(d_first + 1) = *(first + 1) - *(first);
// or *(d_first + 1) = op(*(first + 1, *(first));
// *(d_first + 2) = *(first + 2) - *(first + 1);
// *(d_first + 3) = *(first + 3) - *(first + 2);

template <typename InputIt, typename OutputIt, typename BinaryOperation>
constexpr OutputIt adjacent_difference_my(InputIt first, InputIt last,
                                          OutputIt d_first,
                                          BinaryOperation op) {
  if (first == last)
    return d_first;

  using value_t = typename std::iterator_traits<InputIt>::value_type;
  value_t acc = *first;
  *d_first = acc;

  while (++first != last) {
    value_t val = *first;
    *++d_first = op(val, std::move(acc));
    acc = std::move(val);
  }

  return ++d_first;
}

TEST(adjacent_difference, basic_test) {
  testing::internal::CaptureStdout();
  std::stringstream oss;

  auto print = [](auto comment, auto const &sequence) {
    std::cout << comment;
    for (const auto &n : sequence)
      std::cout << n << ' ';
    std::cout << '\n';
  };

  std::vector v{4, 6, 9, 13, 18, 19, 15, 10};
  print("Initially, v = ", v);
  oss << "Initially, v = 4 6 9 13 18 19 15 10 \n";

  std::adjacent_difference(v.begin(), v.end(), v.begin());
  print("Modified v = ", v);
  oss << "Modified v = 4 2 3 4 5 1 -4 -5 \n";

  // Fibonacci
  std::array<int, 10> a{1};
  std::adjacent_difference(std::begin(a), std::prev(std::end(a)),
                           std::next(std::begin(a)), std::plus<>{});
  print("Fibonacci, a = ", a);
  oss << "Fibonacci, a = 1 1 2 3 5 8 13 21 34 55 \n";

  auto output = testing::internal::GetCapturedStdout();
#ifndef NDEBUG
  debug_msg(oss, output);
#endif

  EXPECT_TRUE(oss.str() == output);
}