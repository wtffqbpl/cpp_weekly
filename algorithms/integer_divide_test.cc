#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace integer_divide {
int divide(int dividend, int divisor) {
  if (dividend == divisor)
    return 1;

  auto a = static_cast<unsigned>(dividend);
  auto b = static_cast<unsigned>(divisor);

  int res = 0;

  while (a >= b) {
    unsigned q = 0;
    while (a > (b << (q + 1)))
      ++q;
    res += (1 << q);
    a = a - (b << q);
  }

  return res;
}

} // namespace integer_divide

TEST(integer_divide, basic_test) {
  EXPECT_EQ(integer_divide::divide(10, 3), 3);
  EXPECT_EQ(integer_divide::divide(10, 4), 2);
  EXPECT_EQ(integer_divide::divide(10, 5), 2);
  EXPECT_EQ(integer_divide::divide(10, 6), 1);
}