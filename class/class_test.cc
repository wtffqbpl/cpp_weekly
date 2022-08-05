#include <gtest/gtest.h>
#include <iostream>
#include <initializer_list>

class Complex {
public:
  Complex(double rnew, double inew) : r{rnew}, i{inew} { };

private:
  double r = 0.0, i = 0.0;
};
