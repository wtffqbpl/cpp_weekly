#include <gtest/gtest.h>

// define a function for array

template <typename T, unsigned N> T sum(const T (&array)[N]) {
  T sum{0};
  for (unsigned i = 0; i < N; ++i)
    sum += array[i];

  return sum;
}

TEST(array_template_function, test) {
  int array1[] = {2, 4, 7};
  unsigned array2[] = {2, 4, 7, 8};

  EXPECT_EQ(sum(array1), 13);
  EXPECT_EQ(sum(array2), 21);
}