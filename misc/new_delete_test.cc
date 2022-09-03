#include <gtest/gtest.h>

/*
 * @brief Placement new 存在的理由
 *    1. 用placement new解决buffer的问题
 *      用 new
 * 分配的数组buffer，由于调用了默认构造函数，因此执行效率上不佳。若没有默认
 *      构造函数,则会发生编译错误。
 */
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