#include "internal_check_conds.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>

template <typename T> // a namespace scope class template
class Data {
public:
  static constexpr bool copyable = true;
};

template <typename T> // a namespace scope function template.
void log(T x) {}

template <typename T> // a namespace scope variable template(since C++14)
T zero = 0;

template <typename T> // a namespace scope variable template(since C++14).
bool dataCopyable = Data<T>::copyable;

template <typename T> // a namespace scope alias template.
using DataList = Data<T *>;

// The following example shows the four kinds of templates as class members
// that are defined within their parent class:
class Collection {
public:
  template <typename T> // an in-class member class template definition.
  class Node {
    //...
  };

  template <typename T> // an in-class (and therefore implicitly inline)
  T *alloc() {          // member function template definition.
    // ...
  }

  template <typename T> // a member variable template (since C++14)
  static T zero = 0;

  template <typename T> // a member alias template.
  using NodePtr = Node<T> *;
};

// template template parameters.
class MyAllocator;
template <template <typename T, typename A = MyAllocator> class Container>
class Adaption {
  Container<int>
      storage; // implicitly equivalent to Container<int, MyAllocator>
  // ...
};

// Usually however, the names of the template parameters of a template template
// parameter are not needed in the declaration of other template parameters and
// are therefore often left unnamed altogether. For example,
template <template <typename, typename = MyAllocator> class Container>
class Adaption2 {
  Container<int>
      storage; // implicitly equivalent to Container<int, MyAllocator>
  // ...
};

namespace {
// 12.2.4 Template Parameter Packs.
// Since C++11, any kind of template parameter can be turned into a template
// parameter pack by introducing an ellipsis(...) prior to the template
// parameter name or, if the template parameter is unnamed, where the template
// parameter name would occur:
template <typename... Types> // declares a template parameter pack named Types.
class Tuple;

// a template parameter pack can match any number of template arguments.
using IntTuple = Tuple<int>;            // one template argument
using IntCharTuple = Tuple<int, char>;  // two template arguments
using IntTriple = Tuple<int, int, int>; // three template arguments
using EmptyTuple = Tuple<>;             // zero template arguments

// 在C++17 标准下，我们可以用 auto keyword来声明non-type template argument,
// compiler 会以 "模版实际参数" 来推导 "模版参数" 的实际参数类型

template <auto Arg> struct Example {
  static constexpr auto value = Arg;

  friend std::ostream &operator<<(std::ostream &os, const Example &item) {
    os << item.value;
    return os;
  }
};

enum TestEnum {
  VAL1,
  VAL2,
};

} // end of namespace

TEST(nontype_template_argument_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Example<42> a;   // Arg == 42, decltype(Arg) == int
  Example<VAL1> b; // Arg == VAL1, decltype(Arg) == TestEnum

  std::cout << a << '\n';
  std::cout << b << '\n';
  oss << "42\n0\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

namespace {

// There are three types of template argument.
// 1. Type template argument
// 2. template template argument
// 3. non-type template argument
template <typename T, // type template argument
          template <typename U, typename A>
          class Container, // template template argument
          int K            // non-type template argument
          >
struct Example2 {};
// Example2<int, std::vector, 47> ex;

// 在上面声明 non-type template parameter 时，我们显式指定了类型,
// 在c++17中，我们 可以直接用 auto keyword 来声明模版类型

template <auto Arg> struct AutoExample {
  static constexpr auto value = Arg;
};

} // end of namespace

TEST(auto_nontype_template_argument_test, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << AutoExample<1>::value << '\n';   // decltype(Arg) == int
  std::cout << AutoExample<'c'>::value << '\n'; // decltype(Arg) == char
  oss << "1\nc\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

// non-type template argument type deduction 是以 模版实际参数 [template actual
// argument] 来推导 模版型参类别 [template formal argument], 这个与
// 函数模版参数推导[function template argument deduction] 是两个不同的概念。
// 函数模版参数推导 (function template)

namespace {

// non-type template argument type deduction 使用场景
// case 1: 将常数表达式包装成类别
// c++ generic programming 通常会将计算结果保存在以 value 为名字的静态常数成员.

template <typename T, T Value> struct my_integral_constant {
  static constexpr T value = Value;
};

// template alias
template <bool Value> using bool_constant = my_integral_constant<bool, Value>;

// c++17 以后，可以使用 template auto 来直接让编译器来进行类型推导
template <auto Value> struct constant {
  static constexpr auto value = Value;
};

// std::cout << constant<5>::value << '\n';

} // end of namespace

TEST(auto_nontype_template_argument_test, test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  my_integral_constant<int, 5> a1;
  bool_constant<true> a2;
  constant<10> a3;

  std::cout << my_integral_constant<int, 5>::value << '\n';
  std::cout << bool_constant<true>::value << '\n';
  std::cout << constant<10>::value << '\n';

  oss << "5\n1\n10\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

namespace {
// 有时使用 std::transform 搭配 unary function 来提取数据成员

struct X {
  int a;
  double b;
};

int GetA(const X &x) { return x.a; }
double GetB(const X &x) { return x.b; }

void test1() {
  X xs[] = {{1, 2.2}, {3, 4.4}, {5, 6.6}, {7, 8.8}};
  std::cout << "int:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<int>(std::cout, "\n"), GetA);

  std::cout << "double:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<double>(std::cout, "\n"), GetB);
}

// 使用一个泛型函数来同时涵盖 GetA & GetB
template <typename DataType, typename ClassType, DataType ClassType::*Ptr>
DataType Getter(const ClassType &obj) {
  return obj.*Ptr;
}

// DataType: 抽取的数据类型
// ClassType: 结构体的类型
// Ptr: Pointer to data member

void test2() {
  X xs[] = {{1, 2.2}, {3, 4.4}, {5, 6.6}, {7, 8.8}};

  std::cout << "int:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<int>(std::cout, "\n"),
                 Getter<int, X, &X::a>);

  std::cout << "double:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<double>(std::cout, "\n"),
                 Getter<double, X, &X::b>);
}

// 使用 template <auto> 后可以将代码改写为
template <typename T> struct MemPtrTraits {};

template <typename U, typename V> struct MemPtrTraits<U(V::*)> {
  typedef V ClassType;
  typedef U DataType;
};

template <auto Ptr>
const typename MemPtrTraits<decltype(Ptr)>::DataType &
Getter(const typename MemPtrTraits<decltype(Ptr)>::ClassType &obj) {
  return obj.*Ptr;
}

// 这里我们先定义一个 MemPtrTraits 用来抽取成员变量的类型为DataType 和
// ClassType. 接着 Getter 函数模版只声明了一个 auto Ptr (non-type template
// parameter). Getter 函数的参数类别与回传类别通过 MemPtrTraits 和 decltype(Ptr)
// 推导的到。 这样的话，我们使用 Getter 就能直接以 Getter<&X::a> 产生读取 struct
// X 的成员a的 Getter Function.
void test3() {
  X xs[] = {{1, 2.2}, {3, 4.4}, {5, 6.6}, {7, 8.8}};

  std::cout << "int:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<int>(std::cout, "\n"), Getter<&X::a>);

  std::cout << "double:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<double>(std::cout, "\n"), Getter<&X::b>);
}

// Partial Specialization for non-type template parameter deduction

// 在C++17之前，non-type template parameter 不会影响partial
// specialization的比对原则 从C++17开始，non-type template parameter
// 会被作为推导规则信息的来源。

template <auto Ptr> struct GetterImpl {};

template <typename DataType, typename ClassType, DataType ClassType::*Ptr>
struct GetterImpl<Ptr> {
  static const DataType &Get(ClassType &obj) { return obj.*Ptr; }
};

template <auto Ptr> auto NewGetter = GetterImpl<Ptr>::Get;

void test4() {
  X xs[] = {{1, 2.2}, {3, 4.4}, {5, 6.6}, {7, 8.8}};

  std::cout << "int:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<int>(std::cout, "\n"), NewGetter<&X::a>);

  std::cout << "double:\n";
  std::transform(std::begin(xs), std::end(xs),
                 std::ostream_iterator<double>(std::cout, "\n"),
                 NewGetter<&X::b>);
}

// 函数追踪
// 如果我们写的程序必须提供API让第三方开发者调用，比较好的设计是把公开内容整理成一个struct
// 让第三方开发者通过struct里面的函数指针来调用我们定义的API. Example:

struct Env {
  int (*api1)(int);
  const char *(*api2)(int, int);
  const char *(*api3)(int, const char *, const char *);
};

static int api1(int a) { return a + 1; }
static const char *api2(int a, int b) {
  return ((a + b) % 2 == 0) ? "sum-even" : "sum-odd";
}

static const char *api3(int a, const char *b, const char *c) {
  return (a % 2 == 0) ? b : c;
}

const Env *GetAPI() {
  static const Env env = {api1, api2, api3};
  return &env;
}

// 第三方开发者只需要根据一个GetAPI 来使用对外提供的api
void test5() {
  const Env *env = GetAPI();

  std::cout << env->api1(5) << '\n';
  std::cout << env->api2(1, 2) << '\n';
  std::cout << env->api3(5, "even", "odd") << '\n';
}

// 如果要定义一个debug模式的接口，则需要另外重写一套接口. Eg.

static int api1_debug(int a) {
  std::cerr << "[trace] api1(" << a << ')' << std::flush;
  auto result = api1(a);
  std::cerr << "-> " << result << std::endl;
  return result;
}

static const char *api2_debug(int a, int b) {
  std::cerr << "[trace] api2(" << a << ", " << b << ')' << std::flush;
  auto res = api2(a, b);
  std::cerr << "-> " << res << std::endl;
  return res;
}

static const char *api3_debug(int a, const char *b, const char *c) {
  std::cerr << "[trace] api3(" << a << ", " << b << ", " << c << ')'
            << std::flush;
  auto res = api3(a, b, c);
  std::cerr << "-> " << res << std::endl;
  return res;
}

const Env *GetAPI_with_debug() {
  const bool is_debug_mode = std::getenv("DEBUG") != nullptr;

  static const Env env = {api1, api2, api3};
  static const Env env_debug = {api1_debug, api2_debug, api3_debug};

  return is_debug_mode ? &env_debug : &env;
}

// 虽然api1_debug & api2_debug & api3_debug
// 的代码不复杂，但是很繁琐，我们可以使用 template <auto> 定义一个全能的Logger

void dump_values(std::ostream &os) {}

template <typename A0> void dump_values(std::ostream &os, A0 &&a0) { os << a0; }

template <typename A0, typename A1, typename... Args>
void dump_values(std::ostream &os, A0 &&a0, A1 &&a1, Args &&...args) {
  os << a0 << ", ";
  dump_values(os, std::forward<A1 &&>(a1), std::forward<Args &&>(args)...);
}

template <typename... Args>
void dump_call(std::ostream &os, const char *name, Args &&...args) {
  os << "[trace] " << name << '(';
  dump_values(os, args...);
  os << ')' << std::flush;
}

template <auto Function, const char *Name> struct LoggerImpl;

template <class ReturnType, class... Args, ReturnType (*Function)(Args...),
          const char *Name>
struct LoggerImpl<Function, Name> {
  static ReturnType call(Args... args) {
    dump_call(std::cerr, Name, args...);

    auto res = Function(args...);
    std::cerr << "-> " << res << std::endl;
    return res;
  }
};

template <auto Function, const char *Name>
constexpr auto Logger = LoggerImpl<Function, Name>::call;

const Env *GetAPINew() {
  const bool is_debug_mode = std::getenv("DEBUG") != nullptr;

  static const Env env = {api1, api2, api3};
  static constexpr char api1_name[] = "api1";
  static constexpr char api2_name[] = "api2";
  static constexpr char api3_name[] = "api3";

  static const Env env_debug = {Logger<api1, api1_name>,
                                Logger<api2, api2_name>,
                                Logger<api3, api3_name>};

  return is_debug_mode ? &env_debug : &env;
}

// 使用macro来实现GetAPI
const Env *getAPI_with_macro() {
  const bool is_debug_mode = std::getenv("DEBUG") != nullptr;

#define FOR_EACH_API(V)                                                        \
  V(api1)                                                                      \
  V(api2)                                                                      \
  V(api3)

  static const Env env = {
#define DEFINE_API(X) X,
      FOR_EACH_API(DEFINE_API)
#undef DEFINE_API
  };

#define DEFINE_API_NAME(X) static constexpr char X##_name[] = #X;
  FOR_EACH_API(DEFINE_API_NAME)
#undef DEFINE_API_NAME

  static const Env env_debug = {
#define DEFINE_API(X) Logger<X, X##_name>,
      FOR_EACH_API(DEFINE_API)
#undef DEFINE_API
  };

#undef FOR_EACH_API

  return is_debug_mode ? &env_debug : &env;
}

} // end of namespace

// template <auto...>
// template <auto> 也可以和 Variadic Template 合并使用。
// template <auto... xs>  --- is equivalent to --- template <auto x0, auto x1,
// ..., auto xn> template <auto...>
// 能让我们声明一个任意长度的模版，各个参数是不同的类别，编译器
// 会根据实际模版参数来推导

// 一个简单的 nth_value 例子

namespace {
template <int n, auto... xs> struct nth_value;

template <int n, auto x0, auto... xs> struct nth_value<n, x0, xs...> {
  static constexpr auto value = nth_value<n - 1, xs...>::value;
};

template <auto x0, auto... xs> struct nth_value<0, x0, xs...> {
  static constexpr auto value = x0;
};

static const char str[] = "abc";

template <auto... xs> struct value_list {
  template <int n> using get = nth_value<n, xs...>;
};

} // end of namespace

TEST(template_auto, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << nth_value<0, 1, str, 'c'>::value << std::endl;
  std::cout << nth_value<1, 1, str, 'c'>::value << std::endl;
  std::cout << nth_value<2, 1, str, 'c'>::value << std::endl;
  oss << "1\n"
         "abc\n"
         "c\n";

  std::cout << value_list<1, str, 'c'>::get<0>::value << std::endl;
  std::cout << value_list<1, str, 'c'>::get<1>::value << std::endl;
  std::cout << value_list<1, str, 'c'>::get<2>::value << std::endl;
  oss << "1\n"
         "abc\n"
         "c\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}

namespace {
// A template parameter for a class template, variable template, or alias
// template can have a default template argument only if default arguments were
// also supplied for the subsequent parameters.
// e.g.

template <typename T1, typename T2, typename T3, typename T4 = char,
          typename T5 = char>
class Quintuple; // OK

template <typename T1, typename T2, typename T3 = char, typename T4,
          typename T5>
class Quintuple; // OK: T4 and T5 already have defaults.

// template <typename T1 = char, typename T2, typename T3,
//           typename T4, typename T5>
// class Quintuple; // ERROR: T2 hasn't default.

// Default template arguments for template parameters of function templates do
// not require subsequent template parameters to have a default template
// argument.
template <typename R = void, typename T>
R *address_of(T &value); // OK: if not explicitly specified, R will be void.

// NOTE: Default template arguments cannot be repeated.
} // namespace

namespace {

// 12.3 Template Arguments
template <typename Func, typename T> void apply(Func funcPtr, T x) {
  funcPtr(x);
}

template <typename T> void single(T){};

template <typename T> void multi(T){};
template <typename T> void multi(T *){};

int test() {
  apply(&single<int>, 3); // OK

  // ERROR:
  // No matching function for call to 'apply' candidate template ignored:
  // couldn't infer template argument 'Func'
  // apply(&multi<int>, 7);

  // Because function templates can be overloaded, explicitly providing all the
  // arguments for a function template may not be sufficient to identify a
  // single function.
  // &multi<int> could be one of two different types and therefore Func cannot
  // be deduced in this case.

  return 0;
}

template <typename T> void test_rt1(typename T::X const *){};
template <typename T> void test_rt1(...) {}

// The Expression test_rt1<int> identifies the address of a single function (the
// second one, test_rt1(...)). The fact that the substitution of int into the
// first template fails does not make the expression invalid. This SFINAE
// (substitution failure is not an error) principle is an important ingredient
// to make the overloading of function templates practical.

// 12.3.3 Non-type Arguments
// Here are some valid examples of non-type template arguments:
// A general constraint of template arguments is that a compiler or a linker
// must be able to express their value when the program is being built. Values
// that aren't known until a program is run aren't compatible with the notion
// that templates are instantiated when the program is built.
// Even so, there are some constant values that are, perhaps surprisingly,
// not currently valid:
//  * Floating-point numbers
//  * String literals

template <typename T, T nonTypeParam> class C;

void f() {}
void f(int) {}

template <typename T> void temp_func() {}

struct XNonType {
  static inline bool b;
  int n;

  constexpr operator int() const { return 42; }
};

C<int, 33> *c1; // integer type

int a;
C<int *, &a> *c2; // address of an external variable

C<void (*)(int), f> *c3; // name of a function: overload resolution selects
                         // f(int) in this case; the & is implied.

// FIXME: I don't know why this introduce an error in github action. I have
//        already set the c++ standard in cmake.
// error: ‘void()’ is not a valid type for a template non-type parameter
// C<void(), &temp_func<double>> *c4; // function template instantiations
// are functions.

C<bool &, XNonType::b> *c5; // static class members are acceptable
                            // variable/function names.

C<int XNonType::*, &XNonType::n> *c6; // an example of a pointer-to-member
                                      // constant

C<long, XNonType{}> *c7; // OK: XNonType is first converted to int via a
                         // constexpr conversion function and then to long
                         // via a standard integer conversion.

// Introducing an additional variable to hold the string for constant string
// template arguments.

template <char const *str> class Message {};

extern char const hello[] = "Hello World!";
char const hello11[] = "Hello World!";

void foo_test() {
  static char const hello17[] = "hello world!";
  Message<hello> msg03;   // OK in all versions.
  Message<hello11> msg11; // OK since c++11
  Message<hello17> msg17; // OK since c++17
}

} // namespace

namespace {
// 12.3.4 Template Template Arguments
// A template template argument must generally be a class template or alias
// template with parameters that exactly match the parameters of the template
// template parameter it substitutes.
// C++17 relaxed the matching rule to just require that the template template
// parameter be at least as specialized as the corresponding template template
// argument. Eg.

// declares in namespace std;
// template <typename T, typename Allocator = allocator<T>>
// class list;

template <typename T1, typename T2,
          template <typename> class Cont> // Cont expects one parameter
class Rel {};

// Rel<int, double, std::list> rel; //

} // namespace

namespace {

template <typename... Types>
void print(Types... values) {
  (std::cout << ... << values) << std::endl;
}
} // namespace

TEST(function_parameter_pack, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::string welcome("Welcome to ");
  // calls print<std::string, char const *, int, char>
  print(welcome, "C++ ", 2011, '\n');
  oss << "Welcome to C++ 2011\n\n";

  std::string act_output = testing::internal::GetCapturedStdout();

  debug_msg(oss, act_output);

  EXPECT_TRUE(oss.str() == act_output);
}

namespace {

// The following function copies all of its arguments before forwarding them on
// to the function object f.
template <typename F, typename... Types>
void forwardCopy(F f, Types const &...values) {
  f(Types(values)...);
}

template <typename... Types> void g(Types... args) {}

// A nested pack expansion.
template <typename... OuterTypes> class Nested {
  template <typename... InnerTypes> void f(InnerTypes const &...innerValues) {
    g(OuterTypes(InnerTypes(innerValues)...)...);
  }
};

// The following example shows how to expand nested parameter packs.
// Multiple and nested pack expansions are a powerful tool.
template <typename O1, typename O2> class Nested_impl {
  template <typename I1, typename I2, typename I3>
  void f(I1 const &iv1, I2 const &iv2, I3 const &iv3) {
    g(O1(I1(iv1), I2(iv2), I3(iv3)), O2(I1(iv1), I2(iv2), I3(iv3)));
  }
};

// Fold Expressions

// This is a recursive version.
bool and_all() { return true; }
template <typename T> bool and_all(T cond) { return cond; }
template <typename T, typename... Ts> bool and_all(T cond, Ts... conds) {
  return cond && and_all(conds...);
}

template <typename T> T trait() {}

// Before we use fold expression, the original definition is as follows.
template <typename... T> bool g() { return and_all(trait<T>()...); }
// With fold expression, the function `bool g()` would be re-written to:
template <typename... T> bool g_with_fold_expression() {
  return (trait<T>() && ... && true);
}

} // namespace

namespace {

template <typename T> class Pal;

class CFriendTemplate {
  friend class Pal<CFriendTemplate>; // Pal<C> is the friend class of C
  template <typename T>
  friend class Pal2; // all initialization of Pal2 classes
                     // are friend class of C
private:
  void print() const { std::cout << "class CFriendTemplate" << std::endl; }
};

template <typename T> class CFriendTemplate2 {
  friend class Pal<T>; // Pal<T> is the friend class of C2
  // All initialization of Pal2 classes are friend class of C2.
  // Note: the template parameter X must not be the same type of T (the
  // notation) If the X and T are the same notation, then Pal2 must have the
  // same template parameter with C2 class.
  template <typename X> friend class Pal2;
  // The ordinary friend class.
  friend class Pal3;
  // A template parameter is the friend class of C2.
  // This is valid for any type T but is ignored if T is not actually a
  // class type. This is introduced since C++11.
  friend T;

private:
  void print() const { std::cout << "class C2" << std::endl; }
};

template <typename T> class Pal {
  CFriendTemplate myC;
  CFriendTemplate2<T> myC2;

public:
  void printC() const {
    std::cout << "this is class Pal: ";
    myC.print();
  }

  void printC2() const {
    std::cout << "this is class Pal : ";
    myC2.print();
  }
};

template <typename T> class Pal2 {
  CFriendTemplate myC;
  CFriendTemplate2<double> myC2;

public:
  void printC() const {
    std::cout << "This is class Pal2: ";
    myC.print();
  }

  void printC2() const {
    std::cout << "This is class Pal2: ";
    myC2.print();
  }
};

class Pal3 {
  CFriendTemplate2<double> myC2;

public:
  void printC2() const {
    std::cout << "This is class Pal3: ";
    myC2.print();
  }
};

class Pal4 {
  CFriendTemplate2<Pal4> myC2;

public:
  void printC2() const {
    std::cout << "This is class Pal4: ";
    myC2.print();
  }
};

} // namespace

TEST(friend_template, basic_test) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  Pal<CFriendTemplate> pc{};
  pc.printC();
  oss << "this is class Pal: class CFriendTemplate\n";

  Pal2<int> pi2{};
  pi2.printC();
  oss << "This is class Pal2: class CFriendTemplate\n";

  Pal<int> pi{};
  pi.printC2();
  oss << "this is class Pal : class C2\n";

  pi2.printC2();
  oss << "This is class Pal2: class C2\n";

  Pal3 p3{};
  p3.printC2();
  oss << "This is class Pal3: class C2\n";

  Pal4 p4{};
  p4.printC2();
  oss << "This is class Pal4: class C2\n";

  auto act_output = testing::internal::GetCapturedStdout();
  debug_msg(oss, act_output);
  EXPECT_TRUE(oss.str() == act_output);
}
