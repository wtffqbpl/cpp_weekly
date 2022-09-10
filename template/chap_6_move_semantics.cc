#include <gtest/gtest.h>
#include <type_traits>
#include <utility>

class X {};

void g(X &) { std::cout << "g() for variable\n"; }
void g(X const &) { std::cout << "g() for constant\n"; }
void g(X &&) { std::cout << "g() for movable object\n"; }

// let f() perfect forward argument val to g();
template <typename T> void f(T &&val) {
  g(std::forward<T>(val)); // call the right g() for any passed argument val.
}

TEST(chap_6_move_semantics, perfect_forward_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  X v;
  X const c;

  f(v);            // f() for variable calls f(X&) => calls g(X&)
  f(c);            // f() for constant calls f(X cont&) => calls g(X const&)
  f(X{});          // f() for temporary calls f(X&&) => calls g(X&&)
  f(std::move(v)); // f() for move-enable variable calls f(X&&) => calls g(X&&)

  oss << "g() for variable\n"
         "g() for constant\n"
         "g() for movable object\n"
         "g() for movable object\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

/// The definition of enable_if
/// \code
/// template <bool _Pre, typename _Tp = void>
/// struct enable_if{};
///
/// template <typename _Tp>
/// struct enable_if<true, _Tp> {
///   typedef _Tp type;
/// };
/// \endcode
// 所谓的SFINAE
// 规则就是在编译时进行查找替换，对于重载的函数，如果能找到合适的就会
// 替换，如果第一个不合适并不会报错，而会使用下一个替换直到最后一个，如果都不满足要求，
// 那么才会报错。出现二义性的话也会报错。 std::enable_if 的用法如下:

// 1. 用于模版类型偏特化

template <typename Tp> struct SmartPointer : public std::false_type {};

template <typename Tp>
struct SmartPointer<std::weak_ptr<Tp>> : public std::true_type {};

template <typename Tp>
struct SmartPointer<std::shared_ptr<Tp>> : public std::true_type {};

template <typename Tp>
struct is_smart_pointer
    : public SmartPointer<typename std::remove_cv<Tp>::type> {};

template <typename Tp>
typename std::enable_if<is_smart_pointer<Tp>::value, void>::type check(Tp p) {
  std::cout << "is smart pointer\n";
}

template <typename Tp>
typename std::enable_if<!is_smart_pointer<Tp>::value, void>::type check(Tp p) {
  std::cout << "not smart pointer\n";
}

TEST(chap_6_move_semantics, enable_if_test) {
  std::stringstream oss;

  testing::internal::CaptureStdout();
  int *p = new int(3);
  std::shared_ptr<int> sp = std::make_shared<int>(3);
  check(sp);
  oss << "is smart pointer\n";
  check(p);
  oss << "not smart pointer\n";
  delete p;

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

/// SFINAE --- Substitution failure is not an error
/// 意思是匹配失败不是错误。当调用模版函数时，编译器会根据传入参数推导最合适的模版函数，
/// 在这个推导过程中如果某一个或者某几个模版函数推导出来是编译无法通过的，只要有一个可以
/// 正确推导出来，那么那几个推导得到的可能产生编译错误的模版函数并不会引发编译错误。

struct Test {
  typedef int foo;
};

template <typename T> void f(typename T::foo) {} // definition #1
template <typename T> void f(T) {}               // Definition #2

template <bool B, class T = void> struct my_enable_if {};

template <class T> struct my_enable_if<true, T> { typedef T type; };

/// 当enable_if<bool, class T> 第一个参数为true时，才会定义type.
typename std::enable_if<true, int>::type t;

/// 下面这两个函数如果是普通函数的话，根据重载的规则是不会通过编译的，会产生二义性。
/// 但是因为std::enable_if
/// 的使用，导致这两个函数的返回值在同一时刻只会有一个合法，
/// 遵循SFINAE原则，则可以顺利通过编译。
template <typename T>
typename std::enable_if<std::is_trivial<T>::value>::type SFINAE_test(T value) {
  std::cout << "T is trivial type.\n";
}

template <typename T>
typename std::enable_if<!std::is_trivial<T>::value>::type SFINAE_test(T value) {
  std::cout << "T is non-trivial.\n";
}

// 这样写的好处:
// 利用SFINAE特性实现了通过不同返回值，相同函数参数进行重载，这样代码看起来
//              更加统一一些。std::enable_if
//              还可以在模版参数列表中，利用SFINAE特性 来实现某些函数的选择推导.

namespace detail {
struct inplace_t {};
} // namespace detail
void *operator new(std::size_t, void *p, detail::inplace_t) { return p; }

// enable via the return type
template <typename T, typename... Args>
typename std::enable_if_t<std::is_trivially_constructible_v<T, Args &&...>>
construct(T *t, Args &&...args) {
  std::cout << "constructing trivially constructible T\n";
}

// enabled via a parameter.
template <typename T>
void destroy(
    T *t,
    typename std::enable_if_t<std::is_trivially_destructible_v<T>> * = 0) {
  std::cout << "destroying trivially destructible T\n";
}

// enabled via a template parameter.
template <typename T, typename std::enable_if_t<
                          !std::is_trivially_destructible_v<T> &&
                              (std::is_class_v<T> || std::is_union_v<T>),
                          int> = 0>
void destroy(T *t) {
  std::cout << "destroying non-trivially destructible T\n";
  t->~T();
}

TEST(chap_6_move_semantics, enable_if_sfinae_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  SFINAE_test(std::string("123"));
  SFINAE_test(123);

  oss << "T is non-trivial.\n"
      << "T is trivial type.\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}