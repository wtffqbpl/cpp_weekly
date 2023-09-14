#include <gtest/gtest.h>
#include <algorithm>
#include <array>

#include "internal_check_conds.h"

namespace {

// std::partial_sort
// Defined in header <algorithm>
/// \code
/// template <typename ExecutionPolicy, typename RandomIt>
/// void partial_sort(ExecutionPolicy &&policy,
///                   RandomIt first, RandomIt middle, RandomIt last);
/// \endcode
///
/// \code
/// template <typename RandomIt, typename Compare>
/// void partial_sort(RandomIt first, RandomIt middle, RandomIt last,
///                   Compare comp);
/// \endcode


void print_res(auto const &s, int middle)
{
  for (int a : s)
    std::cout << a << ' ';
  std::cout << '\n';

  if (middle > 0)
  {
    while (middle-- > 0)
      std::cout << "--";
    std::cout << '^';
  }
  else if (middle < 0)
  {
    for (auto i = s.size() + middle; --i; std::cout << "  ")
    {}
    for (std::cout << '^'; middle++ < 0; std::cout << "--")
    {}
  }
  std::cout << '\n';
}

void test1()
{
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::array<int, 10> s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
  print_res(s, 0);
  oss << "5 7 4 2 8 6 1 9 0 3 \n\n";

  std::partial_sort(s.begin(), s.begin() + 3, s.end());
  print_res(s, 3);
  oss << "0 1 2 7 8 6 5 9 4 3 \n"
         "------^\n";

  std::partial_sort(s.rbegin(), s.rbegin() + 4, s.rend());
  print_res(s, -4);
  oss << "4 5 6 7 8 9 3 2 1 0 \n"
         "          ^--------\n";

  std::partial_sort(s.rbegin(), s.rbegin() + 5, s.rend(), std::greater{});
  print_res(s, -5);
  oss << "4 3 2 1 0 5 6 7 8 9 \n"
         "        ^----------\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// The possible implementation

namespace impl {
template <typename RandomIt, typename Compare>
void sift_down(RandomIt first, RandomIt last, const Compare &comp)
{
  // shift down element at 'first'
  const auto length = static_cast<size_t>(last - first);
  std::size_t current = 0;
  std::size_t next = 2;

  while (next < length)
  {
    if (comp(*(first + next), *(first + (next - 1))))
      --next;
    if (!comp(*(first + current), *(first + next)))
      return;
    std::iter_swap(first + current, first + next);
    current = next;
    next = 2 * current + 2;
  }
  --next;
  if (next < length && comp(*(first + current), *(first + next)))
    std::iter_swap(first + current, first + next);
}

template <typename RandomIt, typename Compare>
void heap_select(RandomIt first, RandomIt middle, RandomIt last, const Compare &comp)
{
  std::make_heap(first, middle, comp);
  for (auto i = middle; i != last; ++i)
  {
    if (comp(*i, *first))
    {
      std::iter_swap(first, i);
      sift_down(first, middle, comp);
    }
  }
}

template <typename RandomIt, typename Compare>
void partial_sort_new(RandomIt first, RandomIt middle, RandomIt last, Compare comp)
{
  impl::heap_select(first, middle, last, comp);
  std::sort_heap(first, middle, comp);
}

}

}

TEST(partial_sort_test, test1)
{
  test1();
}