#include <gtest/gtest.h>

// 函数签名
// c++中的函数签名(function signature):
//    包含了一个函数的信息，包括函数名、参数类型、参数个数、顺序以及它所在的类和命名空间.
//    普通函数签名并不包含函数返回值部分，如果两个函数仅仅只有函数返回值不同，那么系统是
//    无法区分这两个函数的，此时编译器会提示语法错误。
//    函数签名用于识别不同的函数，函数的名字只是函数签名的一部分。在编译器及连接器处理
//    符号时，使用某种名称修饰的方法，使得每个函数签名对应一个修饰后名称(decorated
//    name).
//    编译器在c++源码编译后的目标文件中所使用的符号名是相应的函数和变量的修饰名称。
//    C++编译器和链接器都使用符号来识别和处理函数和变量，所以对于不同函数签名的函数，即使
//    函数名相同，编译器和连接器都认为他们是不同的函数。

// SFINAE (Substitution Failure Is Not An Error)
// 在函数模版的重载决议中会应用此规则：当模版形参在替换成显式指定的类型活推导出的类型失败时,
// 从重载集中丢弃这个特化，而非导致编译失败。
// 此特性用于模版元编程。

// 函数模版重载
// 函数模版与非模版函数可以重载，
// 非模版函数与具有相同类型的模版特化始终不同。即使具有相同类型，不同函数模版的特化也始终
// 互不相同。两个具有相同返回类型和相同形参列表的函数模版时不同的，而且可以用显式模版实参
// 列表进行区分。

// A common idiom is to use expression SFINAE on the return type, where the
// expression uses the comma operator, whose letf subexpression is the one that
// is being examined (cast to void to ensure the user-defined operator comma on
// the returned type is not selected), and the right subexpression has the type
// that the function is supposed to return.

// this overload is always in the set of overloads ellipsis parameter has the
// lowest ranking for overload resolution.
void test(...) { std::cout << "Catch-all overload called\n"; }

// this overload is added to the set of overloads if C is a reference-to-class
// type and F is a pointer to member function of C.
template <typename C, typename F>
auto test(C c, F f) -> decltype((void)(c.*f)(), void()) {
  std::cout << "Reference overload called\n";
}

// this overload is added to the set of overloads if C is a pointer-to-class
// type and F is a pointer to member function of C.
template <typename C, typename F>
auto test(C c, F f) -> decltype((void)(c->*f)(), void()) {
  std::cout << "Pointer overload called\n";
}

struct X {
  void f() {}
};

TEST(sfinae_test, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  X x;
  test(x, &X::f);
  test(&x, &X::f);
  test(42, 1337);

  oss << "Reference overload called\n"
         "Pointer overload called\n"
         "Catch-all overload called\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << '\n'
            << "Actual output:\n"
            << act_output << '\n';
#endif
  EXPECT_TRUE(oss.str() == act_output);
}

template <typename T,
          typename U = typename std::vector<T>::iterator // 1
          >
typename std::vector<T>::value_type foo(         // 1
    T *,                                         // 1
    T &,                                         // 1
    typename T::internal_type,                   // 1
    typename std::add_lvalue_reference<T>::type, // 1
    int) {
  // implementation, no substitution.
}
// 上述所有标记为1的地方，都是substitution 时虚啊哟处理的东西

struct X1 {
  typedef int type;
};
struct Y1 {
  typedef int type2;
};

template <typename T> void foo(typename T::type);  // foo0
template <typename T> void foo(typename T::type2); // foo1
template <typename T> void foo(T);                 // foo2

void call_foo() {
  // foo<X1>(5);   // foo0: Succeed, foo1: failed,  foo2: failed.
  // foo<Y1>(10);  // foo0: failed,  foo1: succeed, foo2: failed
  // foo<int>(15); // foo0: failed,  foo1: failed,  foo2: succeed
}

struct ICounter {
  virtual void increase() = 0;
  virtual ~ICounter() = default;
};

struct Counter : public ICounter {
  void increase() override { std::cout << "increase\n"; }
};

// 实际上,std::enable_if 也是借用了这个原理
template <typename T>
void inc_counter(
    T &counter_obj,
    typename std::enable_if<std::is_base_of_v<T, ICounter>>::type * = nullptr) {
  std::cout << "std::is_base_of_v\n";
}

template <typename T>
void inc_counter(
    T *counter_int,
    typename std::enable_if<std::is_integral_v<T>>::type * = nullptr) {
  std::cout << "std::is_integral_v\n";
}

// 首先，substitution只有在推断函数类型的时候，才会起作用。推断函数类型需要参数的类型，
// 所以，typename std::enable_if<std::is_integral_v<T>>::type 这么一长串代码，
// 就是为了让enable_if 参与到函数类型中.
// 其次，is_integral_v<T>返回一个boolean
// 类型的编译器常数，告诉我们它是或者不是一个 integral, std::enable_if<C>
// 的作用就是，如果这个C值为true，那么type就会被推断
// 成一个void或者是别的什么类型，让整个函数匹配后的类型变成
//    void inc_counter<int>(int &counter_int, void *dummy = nullptr)
// 如果这个值为 False, 那么 std::enable_if<false>
// 这个特化形式中，压根就没有这个type, 于是 substitution 就失败了 ---
// 所以这个函数原型根本就不会被产生出来。

// 当觉得要写 std::enable_if 的时候，首先要考虑一下可能性:
//  * 重载 (对模版函数)
//  * 偏特化 (对模版类而言)
//  * 虚函数

// SFINAE
// 最主要的作用，是保证编译器在泛型函数、偏特化、及一般重载函数中遴选函数原型的
// 候选列表时不被打断。除此之外，它含有一个很重要的元编程作用就是实现部分的编译期自省和反射.