#include <gtest/gtest.h>
#include <iostream>

class Fraction {
public:
  // This is non-explicit-one-argument ctor.
  //  意思是，只要一个实参即可，两个也行.
  // explicit Fraction(int num, int den = 1) : numerator_(num),
  // denominator_(den) {}

  // explicit-one-agument ctor.
  // 声明不能隐式地将其他类型转换到该类型.
  explicit Fraction(const int num, const int den = 1)
      : numerator_(num), denominator_(den) {}

  operator double() const {
    return static_cast<double>(numerator_) / denominator_;
  }
  operator int() const {
    return static_cast<int>(numerator_ / denominator_);
  }

  Fraction operator+(const Fraction &f) {
    return Fraction(this->num() + f.num(), this->den() + f.den());
  }

  int num() const { return numerator_; }

  int den() const { return denominator_; }

private:
  int numerator_;   // fraction
  int denominator_; //
};

TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(Conversion_Function, conversionAdd) {
  Fraction f1{3, 5};
  Fraction f2{4};
  int d = f1 + f2;
  EXPECT_EQ(d, 1);
}