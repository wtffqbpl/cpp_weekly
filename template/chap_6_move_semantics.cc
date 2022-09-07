#include <gtest/gtest.h>
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