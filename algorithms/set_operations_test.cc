#include "internal_check_conds.h"
#include <gtest/gtest.h>

// set_difference
// Copies the elements from the sorted range [first1, last1) which are not found
// in the sorted range [first2, last2) to the range beginning at d_first. The
// output range is also sorted.
// if [first1, last1) contains m elements that are equivalent to each other and
// [first2, last2) contains n elements that are equivalent to them, th final
// std::max(m - n, 0) elements will be copied from [first1, last1) to the output
// range, preserving order.
//  1) Elements are compared using operator< and the ranges must be sorted with
//    respect to the same.
// If either of the input ranges is not sorted (using operator< or comp,
// respectively) or overlaps with the output range, the behavior is undefined.
//
// Complexity
// Given M and N as std::distance(first1, last1), and std::distance(first2,
// last2) respectively: The time complexity of std::set_difference algorithm is
// O(2*(M + N)) - 1 comparisons using specified operator
//

namespace {

// Possible implementation
template <typename InputIt1, typename InputIt2, typename OutputIt,
          typename Compare>
OutputIt set_difference_impl(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                             InputIt2 last2, OutputIt d_first, Compare comp) {
  while (first1 != last1) {
    if (first2 == last2)
      return std::copy(first1, last1, d_first);

    if (comp(*first1, *first2))
      *d_first++ = *first1++;
    else {
      if (!comp(*first2, *first1))
        ++first1;
      ++first2;
    }
  }

  return d_first;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, std::vector<T> const &v) {
  for (os << "{ "; auto const &e : v)
    os << e << ' ';
  return os << '}';
}

struct Order {
  int order_id{};

  friend std::ostream &operator<<(std::ostream &os, const Order &ord) {
    return os << ord.order_id;
  }
};

void set_difference_test() {
  std::vector<Order> old_orders{{1}, {2}, {5}, {9}};
  std::vector<Order> new_orders{{2}, {5}, {7}};
  std::vector<Order> cut_orders;

  std::set_difference(old_orders.begin(), old_orders.end(), new_orders.begin(),
                      new_orders.end(), std::back_inserter(cut_orders),
                      [](auto &a, auto &b) { return a.order_id < b.order_id; });

  std::cout << "old orders = " << old_orders << '\n'
            << "new orders = " << new_orders << '\n'
            << "cut orders = " << cut_orders << '\n';
}

} // namespace

TEST(set_difference_test, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  set_difference_test();

  // Expected output:
  oss << "old orders = { 1 2 5 9 }\n"
         "new orders = { 2 5 7 }\n"
         "cut orders = { 1 9 }\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}