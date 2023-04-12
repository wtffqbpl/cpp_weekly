#include <gtest/gtest.h>
#include <iostream>

template <typename T>
class complex {
public:
  explicit complex(T r = 0, T i = 0) : re(r), im(i) {}

  complex &operator+=(const complex &other) {
    return complex{re + other.real(), im + other.imag()};
  };
  [[nodiscard]] T real() const { return re; }
  [[nodiscard]] T imag() const { return im; }

private:
  T re, im;
};

TEST(ClassTemplate, Test1) {
  // complex<double> c1(2.5, 1.5);
  complex<int> c2(2, 6);

  EXPECT_EQ(c2.real(), 2);
  EXPECT_EQ(c2.imag(), 6);
}
