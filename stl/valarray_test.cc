#include <gtest/gtest.h>
#include <iostream>
#include <valarray>

//\brief: valarray 是一种数组的替代品，支持逐元素数学运算与多种形式的广义下表运算符、切片、及间接访问。
//        simd 形式的运算。

TEST(StlTest, valarrayTest){
  std::valarray<int> v = {1, 2, 3}, w = {7, 8, 9}, s = v + 2 * w;
  // v = sin(s); // = {sin(s[0]), sin(s[1]), sin(s[2])}

  EXPECT_EQ(s[0], 15);
  EXPECT_EQ(s[1], 18);
  EXPECT_EQ(s[2], 21);
}