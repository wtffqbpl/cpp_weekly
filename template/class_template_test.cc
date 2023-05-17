#include <complex>
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

template <typename T> struct abs_functor {
  T operator()(const T &x) const { return x < T{0} ? -x : x; }
};

template <typename T> decltype(auto) abs_0(const T &x) {
  return abs_functor<T>()(x);
}

template <typename T> decltype(auto) abs_1(T &&x) {
  return abs_functor<T>()(std::forward<T>(x));
}

template <typename T> auto abs_2(T &&x) -> decltype(abs_functor<T>()(x)) {
  return abs_functor<T>()(std::forward<T>(x));
}

// In C++03, we cannot use type deduction at all for the return type. Thus, the
// functor must provide it, say, by a typedef named result_type:
template <typename T> typename abs_functor<T>::result_type abs_3(const T &x) {
  return abs_functor<T>()(x);
}

// Here, we have to rely on the implementor(s) of abs_functor that result_type
// is consistent with the return type of operator().

template <typename T> struct abs_functor<std::complex<T>> {
  using result_type = T;

  T operator()(const std::complex<T> &x) const {
    return std::sqrt(std::real(x) * std::real(x) + std::imag(x) * std::imag(x));
  }
};
