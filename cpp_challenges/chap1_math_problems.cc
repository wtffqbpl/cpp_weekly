#include <gtest/gtest.h>
#include <numeric>

namespace math_problems {

// Sum of naturals divisible by 3 and 5
// Write a program that calculates and prints the sum of all the natural numbers
// divisible by either 3 or 5, up to a given limit entered by the user.

int sum_of_naturals_divisible_by_3_and_5(int limits) {
  int res = 0;

  for (int i = 3; i < limits; ++i) {
    if (i % 3 == 0 || i % 5 == 0)
      res += i;
  }

  return res;
}

// 2. Greatest common divisor
// Write a program that, given two positive integers, will calculate and print
// the greatest common divisor of the two.
int greatest_common_divisor(int a, int b) {
  while (b != 0) {
    auto r = a % b;
    a = b;
    b = r;
  }
  return a;
}

// 3. Least common multiple
// Write a program that will, given two or more positive integers, calculate
// and print the least common multiple of them all.
int least_common_multiple(int a, int b) {
  int res = std::max(a, b);

  while (true) {
    if ((res % a == 0) && (res % b == 0))
      break;
    ++res;
  }

  return res;
}

} // namespace math_problems