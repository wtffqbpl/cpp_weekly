#include <gtest/gtest.h>
#include <iostream>

template <typename T>
class complex {
public:
  complex(T r = 0, T i = 0) : re(r), im(i) {}

  complex &operator+=(const complex &);
  T real() const { return re; }
  T imag() const { return im; }

private:
  T re, im;

  friend complex &__doapl(complex *, const complex &);
};

TEST(ClassTemplate, Test1) {
  // complex<double> c1(2.5, 1.5);
  complex<int> c2(2, 6);

  EXPECT_EQ(c2.real(), 2);
  EXPECT_EQ(c2.imag(), 6);
}
