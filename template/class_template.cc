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

int main() {
  complex<double> c1(2.5, 1.5);
  complex<int> c2(2, 6);

  std::cout << "DOUBLE: c(2.5, 1.5): REAL = " <<
      c1.real() << ", IMAG = " << c1.imag() << std::endl;

  std::cout << "INT: c(2, 6): REAL = " << c2.real() << ", IMAG = " << c2.imag()
            << std::endl;

  return 0;
}
