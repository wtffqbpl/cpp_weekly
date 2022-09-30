#include <gtest/gtest.h>

template <unsigned p, unsigned d> struct DoIsPrime {
  static constexpr bool value = (p % d != 0) && DoIsPrime<p, d - 1>::value;
};

template <unsigned p> struct DoIsPrime<p, 2> {
  static constexpr bool value = (p % 2 != 0);
};

template <unsigned p> struct IsPrime {
  // start recursion with divisor from p / 2;
  static constexpr bool value = DoIsPrime<p, p / 2>::value;
};

// special cases(to avoid endless recursion with template instantiation):
template <> struct IsPrime<0> { static constexpr bool value = false; };
template <> struct IsPrime<1> { static constexpr bool value = false; };
template <> struct IsPrime<2> { static constexpr bool value = true; };
template <> struct IsPrime<3> { static constexpr bool value = true; };

// IsPrime<9>::value
// expands to
// DoIsPrime<9, 4>::value
// which expands to
// 9 % 4 != 0 && DoIsPrime<9, 3>::value
// which expands to
// 9 % 4 != 0 && 9 % 3 != 0 && DoIsPrime<9, 2>::value
// which expands to
// 9 % 4 != 0 && 9 % 3 != 0 && 9 % 2 != 0
// which evaluates to false, because 9 % 3 = false;
// As this chain of instantiations demonstrates:
//  * We use recursive expansions of DoIsPrime<> to iterate over all divisors
//    from (p / 2) down to 2 to find out whether any of these divisors divide
//    the given integer exactly (i.e. without remainder).
//  * The partial specialization of DoIsPrime<> for d equal to 2 serves as the
//    criterion to end the recursion.
// Note that all this is done at compile time.

/*
 * @param p: number to check
 * @param d: current divisor
 * @return whether p is prime number.
 */
constexpr bool doIsPrime(unsigned p, unsigned d) {
  return d != 2 ? (p % d != 0) &&
                      doIsPrime(p, d - 1) // check this and smaller divisors.
                : (p % 2 != 0);           // end recursion if divisor is 2.
}

constexpr bool isPrime(unsigned p) {
  return p < 4 ? !(p < 2)             // handle special cases.
               : doIsPrime(p, p / 2); // start recursion with divisor form p / 2
}

// isPrime(9)
//  xor eax, eax
//  ret

// With C++14, constexpr functions can make use of most control structures
// available in general C++ code.
constexpr bool isPrime14(unsigned int p) {
  for (unsigned int d = 2; d < p / 2; ++d)
    if (p % d == 0)
      return false; // found divisor without remainder
  return p > 1;     // no divisor without remainder found
}

TEST(chap_8_compile_time_programming, prime_test) {
  EXPECT_FALSE(IsPrime<9>::value);
  EXPECT_TRUE(IsPrime<11>::value);
  EXPECT_FALSE(IsPrime<18>::value);

  EXPECT_FALSE(isPrime(9));
  EXPECT_TRUE(isPrime(11));
  EXPECT_FALSE(isPrime(18));
}

// primary helper template.
template <int SZ, bool = isPrime(SZ)> struct Helper;

// implementation if SZ is not a prime number:
template <int SZ> struct Helper<SZ, false> {};

// implementation if SZ is a prime number:
template <int SZ> struct Helper<SZ, true> {};

template <typename T, std::size_t SZ> long foo(std::array<T, SZ> const &coll) {
  // implementation depends on whether array has prime number than size.
  Helper<SZ> h;
}

// depending on whether the size of the std::array<> argument is a prime number.
// Because function templates do not support partial specialization, you have
// to use other mechanisms to change function implementation based on certain
// constraints. Our options include the following:
//  * Use classes with static functions.
//  * Use std::enable_if
//  * Use the SFINAE feature
//  * Use the compile-time `if` feature, available since C++17

// number of elements in a raw array:
template <typename T, unsigned N> size_t len(T (&)[N]) noexcept { return N; }
// number of elements for a type having size_type:
//  template T::size_type len(T const &t) { return t.size(); }