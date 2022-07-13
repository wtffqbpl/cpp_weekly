#include <iostream>

class Fraction {
public:
  Fraction() = delete;
  Fraction operator&=(const Fraction &) = delete;

  Fraction(int num, int den = 1) : numerator_(num), denominator_(den) {}

  operator double() const {
    return static_cast<double>(numerator_) / denominator_;
  }

private:
  int numerator_;   // fraction
  int denominator_; //
};

int main() {

  Fraction f{3, 5};

  double d = 4 + f;

  std::cout << "result = " << d << std::endl;

  return 0;
}
