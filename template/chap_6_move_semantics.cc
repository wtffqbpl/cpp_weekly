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

namespace chap_6_enable_if {
class Person {
public:
  // generic constructor for passed initial name:
  template <typename STR>
  explicit Person(STR &&Name) : Name_(std::forward<STR>(Name)) {
    std::cout << "TMPL-CONSTR for '" << Name_ << "'\n";
  }

  // copy and move constructor:
  Person(Person const &p) : Name_(p.Name_) {
    std::cout << "COPY-CONSTR Person '" << Name_ << "'\n";
  }

  Person(Person &&p) : Name_(std::move(p.Name_)) {
    std::cout << "MOVE-CONSTR Person '" << Name_ << "'\n";
  }

private:
  std::string Name_;
};

/// \code
/// std::string s = "sname";
/// Person p1(s);     // init with string object => calls TMPL-CONSTR
/// Person p2("tmp"); // init with string literal => calls TMPL-CONSTR
///
/// Person p3(p1); // ERROR since template matching machinism
///                // for a non-constant lvalue Person p, member template is
///                // prior than copy constructor.
///                // 因为对于模版来说，直接将STR替换成 Person&
///                // 而copy constructor 还需要一次const 转换
///                //
///                我们这里其实需要的是当参数类型是Person类型的non-constant变量时，
///                // 需要禁用掉 member template. 这里可以通过
///                std::enable_if<>实现
///
/// Person const p2c("ctmp"); // init constant object with string literal.
/// Person p3c(p2c); // OK: copy constant Person => calls COPY-CONSTR
/// Person p4(std::move(p1)); // OK: move Person => calls MOVE-CONST
/// \endcode

// std::enable_if<> 是一种类型萃取(type trait),
// 会根据给定的一个编译时期的表达式来 确定其行为:
// 1. 如果这个表达式为true, std::enable_if<>::type 会返回:
//     * 如果没有第二个模版参数，返回类型是void(这个是因为std::enable_if
//     的default参数为void.
//     * 否则，返回类型是其第二个参数的类型
// 2. 如果表达式结果为false, std::enable_if<>::type 不会被定义.
//    是根据 SFINAE (substitute failure is not an error)

/// \code
/// template <typename T>
/// std::enable_if_t<sizeof(T) > 4), T>
/// foo() { return T{}; }
/// \endcode
/// 如果上述enable_if_t 的第一个模版参数为true,那么展开后得到的代码如下
/// \code
/// MyType foo() {
///   return MyType{};
/// }
/// \endcode

// 如果觉得将 enable_if<> 放在声明中有些丑陋的话，通常的做法是:
/// \code
/// template <typename T,
///           typename = std::enable_if_t<(sizeof(T) > 4>>
/// void foo() { }
/// \endcode
// 当sizeof(T) > 4时，这会被展开为:
/// \code
/// template <typename T,
///           typename = void>
/// void foo() { }
/// \endcode

// 还有一种比较常见的用法:
/// \code
/// template <typename T>
/// using EnableIfSizeGreater4 = std::enable_if_t<(sizeof(T) > 4)>;
///
/// template <typename T,
///           typename = EnableIfSizeGreater4<T>>
/// void foo() { }
/// \endcode

// 使用enable_if<> 来解决实际问题

template <typename T>
using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;

class Person_enable_if {
public:
  // generic constructor for passed initial name:
  template <typename STR, typename = EnableIfString<STR>>
  explicit Person_enable_if(STR &&Name) : Name_(std::forward<STR>(Name)) {
    std::cout << "TMPL-CONSTR for '" << Name_ << "'\n";
  }

  // copy and move constructor:
  Person_enable_if(Person_enable_if const &p) : Name_(p.Name_) {
    std::cout << "COPY-CONSTR Person '" << Name_ << "'\n";
  }

  Person_enable_if(Person_enable_if &&p) noexcept : Name_(std::move(p.Name_)) {
    std::cout << "MOVE-CONSTR Person '" << Name_ << "'\n";
  }

private:
  std::string Name_;
};

// 核心点:
//  * 使用using 来简化std::enable_if<> 在成员模版函数中的写法
//  * 当构造函数的参数不能转换为string时，禁止使用该函数

template <typename T = int> class C {
public:
  C() = default;

  // There is a tricky solution, though. We can declare a copy constructor for
  // const volatile arguments and mark it "deleted". Doing so prevents another
  // copy constructor from being implicitly declared. With that in place, we can
  // define a constructor template that will be preferred over the (deleted)
  // copy constructor for nonvolatile types:

  // user-define the predefined copy constructor as deleted,
  // with conversion to volatile to enable better matches.
  C(C const volatile &) = delete;

  // if T is no integral type, provide copy constructor template with better
  // match.
  template <typename U, typename = std::enable_if_t<!std::is_integral_v<U>>>
  C(C<U> const &) {
    std::cout << "tmpl copy constructor\n";
  }

  C(T const &) { std::cout << "integer tmpl copy constructor.\n"; }
};

} // end of namespace chap_6_enable_if

TEST(chap_6_move_semantics, dis_special_member_function_with_enable_if_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  chap_6_enable_if::C<std::string> x;
  chap_6_enable_if::C<std::string> y{x};

  oss << "tmpl copy constructor\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

// SUMMARY
// 1. In templates, you can "perfectly" forward parameters by declaring them as
//    forwarding references (declared with a type formed with the name of a
//    template parameter followed by &&) and using std::forward<>() in the
//    forwarded call.
// 2. When using perfect forwarding member function templates, they might match
//    better than the predefined special member function to copy or move object.
// 3. With std:enable_if<>, you can disable a function template when a
//    compile-time condition is false (the template is then ignored once that
//    condition has been determined).
// 4. By using std::enable_if<> you can avoid problems when constructor
//    templates or assignment operator templates that can be called for single
//    arguments are a better match than implicitly generated special member
//    functions.
// 5. You can templify (and apply enable_if<>) to special member functions by
//    deleting the pre-defined special member functions for const volatile.
// 6. Concepts will allow us to use a more intuitive syntax for requirements
//    on function templates.