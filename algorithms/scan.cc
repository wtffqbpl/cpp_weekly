#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <numeric>

#include "internal_check_conds.h"

/// Computes an inclusive prefix sum operation using binary_op (or std::plus<>()
/// for overloads(1-2)) for the range [first, last), using init as the initial
/// value (if provided), and writes the results to teh range beginning at
/// d_first. "inclusive" means that the i-th input element is included in the
/// i-th sum.
TEST(scan_test, inclusive_scan_basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector data{3, 1, 4, 1, 5, 9, 2, 6};

  std::cout << "inclusive sum: ";
  /// \code
  /// template <typename InputIt, typename OutputIt>
  /// OutputIt inclusive_scan(InputIt first,
  ///                         InputIt last, OutputIt d_first);
  /// \endcode
  std::inclusive_scan(data.begin(), data.end(),
                      std::ostream_iterator<int>(std::cout, " "));
  oss << "inclusive sum: 3 4 8 9 14 23 25 31 ";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(scan_test, exclusive_scan_basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::vector data{3, 1, 4, 1, 5, 9, 2, 6};

  std::cout << "exclusive sum: ";
  /// \code
  /// template <typename InputIt, typename OutputIt>
  /// OutputIt exclusive_scan(InputIt first,
  ///                         InputIt last, OutputIt d_first);
  /// \endcode
  std::exclusive_scan(data.begin(), data.end(),
                      std::ostream_iterator<int>(std::cout, " "), 0);
  oss << "exclusive sum: 0 3 4 8 9 14 23 25 ";

  auto act_output = testing::internal::GetCapturedStdout();
#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}