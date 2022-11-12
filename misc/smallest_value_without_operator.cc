#include <gtest/gtest.h>

namespace {

int findSmallest(int x, int y, int z) {
  int c = 0;
  while (x && y && z) {
    x--, y--, z--;
    c++;
  }
  return c;
}

} // namespace

TEST(misc_test, find_smallest_value) {
  int a = 2, b = 5, c = 6;
  EXPECT_EQ(findSmallest(a, b, c), 2);
}
