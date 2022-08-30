#include <gtest/gtest.h>

TEST(new_delete, placement_new_test) {
  // buffer on stack
  unsigned char Buf[sizeof(int) * 2];

  // placement new in Buf.
  int *Int1 = new (Buf) int{3};
  int *Int2 = new (Buf + sizeof(int)) int{5};

  int *BufIntPtr = reinterpret_cast<int *>(Buf);

  EXPECT_EQ(BufIntPtr[0], 3);
  EXPECT_EQ(BufIntPtr[1], 5);
  EXPECT_EQ(*Int1, 3);
  EXPECT_EQ(*Int2, 5);
}