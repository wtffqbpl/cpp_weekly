#include <gtest/gtest.h>
#include <type_traits>

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

// 8.4.1 Expression SFINAE with decltype
template <typename T> typename T::size_type len_8_4_1(T const &t) {
  return t.size();
}

// this will be failed when dealing following case:
/// \code
/// std::allocator<int> x;
/// std::cout << len_8_4_1(x) << '\n'; // ERROR: len() selected, but x has no
///                                    // size()
/// \endcode

// There is a common pattern or idiom to deal with such a situation.
//  * Specify the return type with the trailing return type syntax (use auto
//    at the front and -> before the return type at the end).
//  * Define the return type using decltype and the comma operator.
//  * Formulate all expressions that must be valid at the beginning of the comma
//    operator (converted to void in case in the comma operator is overloaded).
//  * Define an object of the real return type at the end of the comma operator.
//  for example:
template <typename T>
auto len_8_4_1_new(T const &t) -> decltype((void)(t.size()), T::size_type()) {
  return t.size();
}
// Here, the return type is given by:
// decltype((void)(t.size()), T::size_type())

// 8.5 Compile-time if
// With the syntax if constexpr(...), the compiler uses a compile-time
// expression to decide whether to apply the then part or the else part
// (if any).
// For example:
template <typename T, typename... Types>
void print_8_5(T const &firstArg, Types const &...args) {
  std::cout << firstArg << '\n';
  if constexpr (sizeof...(args) > 0) {
    print_8_5(args...); // code only available if sizeof...(args) > 0(since 17).
  }
}

TEST(chap_8_compile_time_programming, compile_time_if_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  print_8_5("test", "compile", "if", 1, 2, "hello", "world");
  oss << "test\n"
      << "compile\n"
      << "if\n"
      << "1\n"
      << "2\n"
      << "hello\n"
      << "world\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// The fact that the code is not instantiated means that only the first
// translation phase (the definition time) is performed, which checks for
// correct syntax and names that don't depend on template parameters.
// e.g.
template <typename T> void foo(T t) {
  if constexpr (std::is_integral_v<T>) {
    if (t > 0)
      foo(t - 1);
  } else {
    // error if not declared and not discarded.
    std::undeclare_reachable(t);
    static_assert(!std::is_integral_v<T>, "no integral");
  }
}

// Not that if constexpr can be used in any function, not only in templates.
// We only need a compile-time expression that yields a Boolean value.

// Summary
//  * Templates provide the ability to compute at compile time (using recursion
//    to iterate and partial specialization or operator ?: for selections).
//  * With constexpr functions, we can replace most compile-time computations
//    with "ordinary functions" that are callable in compile-time contexts.
//  * With partial specialization, we can choose between different
//    implementations of class templates based on certain compile-time
//    constraints.
//  * Templates are used only if needed and substitutions in function template
//    declarations do not result in invalid code. This principle is called
//    SFINAE (substitution failure is not an error).
//  * SFINAE can be used to provide function templates only for certain types
//    and/or constraints.
//  * Since C++17, a compile-time if allows us to enable or discard statements
//    according to compile-time conditions (even outside templates).