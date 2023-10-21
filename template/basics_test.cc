#include "internal_check_conds.h"
#include <deque>
#include <gtest/gtest.h>
#include <list>

namespace class_variable_alias_templates {

template <typename T> class my_vector {
public:
  typedef T *my_iterator;

public:
  my_vector();

  my_vector(T tmp) {}

  my_vector &operator=(const my_vector &item) = default;

public:
  void my_func() { std::cout << "myfunc() is called." << std::endl; }

public:
  my_iterator my_begin() {}
  my_iterator my_end() {}
};

template <typename T> my_vector<T>::my_vector() {}

void test_class_template() {
  my_vector<int> tmp_vec;
  tmp_vec.my_func();

  my_vector tmp_vec2(12);
}

// Deduction Guide
template <typename T> struct A {
  A(T val, T val2) {
    std::cout << "A::A(T val, T val2) is called." << std::endl;
  }

  A(T val) { std::cout << "A::A(T val) is called." << std::endl; }
};

// Deduction Guide
template <typename T> A(T, T) -> A<T>;

template <typename T>
struct B // Aggregate type
{
  T m_b;
};

template <typename T> B(T) -> B<T>;

void deduction_guide_test() {
  A aobj1{15, 16};
  A aobj2{12.8};

  B<int> bobj1;
  B<int> bobj2{15};

  B bobj3{15}; // Must be deduced with deduction guide
}

// Partial specialization
template <typename T, typename U> struct TC {
  TC() { std::cout << "TC general constructor function" << std::endl; }

  void functest1() { std::cout << "functest1 general version" << std::endl; }
};

template <> struct TC<int, int> {
  TC() {
    std::cout << "TC<int, int> fully partial specialization version"
              << std::endl;
  }

  void functest1();
};

void TC<int, int>::functest1() {
  std::cout << "functest1 partial specialization version" << std::endl;
}

void partial_specialization_test1() {
  TC<int, float> mytc;
  mytc.functest1();

  TC<int, int> mytc2;
  mytc2.functest1();
}

template <typename U> struct TC<float, U> {
  TC() {
    std::cout << "TC<float, U> partial specialization constructor" << std::endl;
  }

  void functest1();
};

template <typename U> void TC<float, U>::functest1() {
  std::cout << "TC<float, U>::functest1 partial specialization version"
            << std::endl;
}

void partial_specialization_test2() {
  TC<float, int> mytc4;
  mytc4.functest1();
}

// Template Template Parameters
template <typename T, template <class> class Container = std::vector>
class my_class {
public:
  void func();

  my_class() {
    for (int i = 0; i < 10; ++i) {
      myc.push_back(i);
    }
  }

public:
  Container<T> myc;
};

template <typename T, template <class> class Container>
void my_class<T, Container>::func() {
  std::cout << "good!" << std::endl;
}

#if 0
void template_template_parameter_test() {
  my_class<double> mylistobj2;
  mylistobj2.func();
}
#endif

template <typename T> class B_friend;

template <typename T> class A_friend {
  // 特定类型的B_friend<long> 的实例化版本作为class A_friend 的友元
  friend class B_friend<long>;

  // 把整个类模版B_friend 作为类模版A_friend 的友元模版
  template <typename> friend class B_friend;

private:
  int data;
};

template <typename T> class B_friend {
public:
  void callBAF() {
    A_friend<int> atmpobj{};
    atmpobj.data = 5;
    std::cout << "atmpobj.data = " << atmpobj.data << std::endl;
  }
};

template <typename T> class A2_friend {
  // 声明 模版参数T 作为类模版 A2_friend 的友元类
  friend T;

private:
  int data;
};

class CF {
public:
  void callCFAF() {
    A2_friend<CF> aobj1{};
    aobj1.data = 12;

    std::cout << "aobj1.data = " << aobj1.data << std::endl;
  }
};

template <typename U, typename V> void func(U, V);

template <typename T> class Men {
  // Define friend function
  template <typename U, typename V> friend void func(U, V);

  friend void func2(Men<T> &tmpmen) { tmpmen.funcmen(); }

private:
  void funcmen() const { std::cout << "Men::funcmen is called\n"; }
};

template <typename U, typename V> void func(U val1, V val2) {
  Men<int> mymen{};
  mymen.funcmen();
}

void test_template_class_friend() {
  B_friend<long> bobj;
  bobj.callBAF();

  B_friend<int> bobj_int;
  bobj_int.callBAF();

  CF mycfobj{};
  mycfobj.callCFAF();

  // friend function test
  func(2, 3);
  func<float>(4.6f, 5);
  func<int, float>(4, 5.8f);

  Men<double> mymen2;
  func2(mymen2);
  Men<int> mymen3;
  func2(mymen3);
}

// Variadic Templates
template <typename... ClassPList> class MyClassT5 : public ClassPList... {
public:
  MyClassT5() : ClassPList()... {
    std::cout
        << "MyClassT5::MyClassT5." /* << " this = " << this */ << std::endl;
  }
};

class PA1 {
public:
  PA1() { std::cout << "PA1::PA1. " /* << "this = " << this */ << std::endl; }

private:
  char m_s1[100];
};

class PA2 {
public:
  PA2() { std::cout << "PA2::PA2." /* << " this = " << this */ << std::endl; }

private:
  char m_s1[200];
};

class PA3 {
public:
  PA3() { std::cout << "PA3::PA3." /* << " this = " << this */ << std::endl; }

private:
  char m_s1[300];
};

// Variadic Template Partial Specialization
template <typename... Args> class MyClassT6 {
public:
  MyClassT6() {
    std::cout
        << "MyClassT6 generic version is called." /* << " this = " << this */
        << ", sizeof...(Args) = " << sizeof...(Args) << std::endl;
  }
};

template <typename First, typename... Others>
class MyClassT6<First, Others...> {
public:
  MyClassT6() {
    std::cout << "MyClassT6<First, Others...> partial specialization version "
                 "is called."
              /* << ", this = " << this */
              << ", sizeof...(Others) = " << sizeof...(Others) << std::endl;
  }
};

template <typename Arg> class MyClassT6<Arg> {
public:
  MyClassT6() {
    std::cout << "MyClassT6<Arg> partial specialization version is called."
              /*<< ", this = " << this */
              << std::endl;
  }
};

template <typename Arg1, typename Arg2> class MyClassT6<Arg1, Arg2> {
public:
  MyClassT6() {
    std::cout
        << "MyClassT6<Arg1, Arg2> partial specialization version is called. "
        /* << "this = " << this */
        << std::endl;
  }
};

template <typename... Args> class MyClassT;

template <typename First, typename... Others>
class MyClassT<First, Others...> : private MyClassT<Others...> {
public:
  MyClassT() : m_i(0) {
    std::cout
        << "MyClassT::MyclassT() partial specialization constructor is called."
        << ", sizeof...(Others) = " << sizeof...(Others) << std::endl;
  }

  MyClassT(First parf, Others... paro)
      : m_i(parf), MyClassT<Others...>(paro...) {
    std::cout << "-------------------- BEGIN ------------------" << std::endl;
    std::cout << "MyClassT::MyClassT(parf, ...paro) constructor is called."
              << std::endl;
    std::cout << "m_i = " << m_i << std::endl;
    std::cout << "--------------------- END -------------------" << std::endl;
  }

private:
  First m_i;
};

template <typename... Args> class MyClassT {
public:
  MyClassT() {
    std::cout << "MyClassT::MyClassT() generic version constructor is called."
              << std::endl;
  }
};

template <int... FTArgs> class MyClassT2 {
public:
  MyClassT2() {
    std::cout << "MyClassT2::MyClassT2() generic verion constructor is called."
              << std::endl;
  }
};

template <int First, int... Others>
class MyClassT2<First, Others...> : private MyClassT2<Others...> {
public:
  MyClassT2() {
    std::cout << "MyClassT2::MyClassT2() partial specialization version "
                 "constructor is called."
              << " sizeof...(Others) = " << sizeof...(Others)
              << ", First = " << First << std::endl;
  }
};

template <typename T, template <typename> typename... Container>
class ParentMM {
public:
  ParentMM() {
    std::cout << "ParentMM::ParentMM() generic version constructor is called."
              << std::endl;
  }
};

template <typename T, template <typename> typename FirstContainer,
          template <typename> typename... OtherContainers>
class ParentMM<T, FirstContainer, OtherContainers...>
    : private ParentMM<T, OtherContainers...> {
public:
  ParentMM() {
    std::cout << "parentMM::ParentMM() partial specialization version "
                 "constructor is called."
              << ", sizeof...(OtherContainers) = " << sizeof...(OtherContainers)
              << std::endl;
    m_container.push_back(12);
  }

private:
  FirstContainer<T> m_container;
};

template <typename T, template <typename> typename... Container>
class MyClassT3 : private ParentMM<T, Container...> {
public:
  MyClassT3() {
    std::cout << "MyClassT3::MyClassT3() constructor is called."
              << " The Type(T) is " << typeid(T).name()
              << ", sizeof...(Container) = " << sizeof...(Container)
              << std::endl;
  }
};

void test_base_inherit_pack_expansion() {
  MyClassT5<PA1, PA2, PA3> obj;
  std::cout << "sizeof(obj) = " << sizeof(obj) << std::endl; // 600

  MyClassT6<int> myc1;
  MyClassT6<int, float> myc2;
  MyClassT6<int, double> myc3;
  MyClassT6<int, double, char> myc4;
  MyClassT6<> myc5;

  MyClassT<int, float, double> myc;
  MyClassT<int, float, double> myc_2(12, 13.5, 23);

  MyClassT2<12, 18, 23> myc2_1;

  // MyClassT3<int, std::vector, std::list, std::deque> myc3_1;
}

} // namespace class_variable_alias_templates

TEST(class_template, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  class_variable_alias_templates::test_class_template();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "myfunc() is called.\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(deduction_guide, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  class_variable_alias_templates::deduction_guide_test();

  auto act_output = testing::internal::GetCapturedStdout();

  oss << "A::A(T val, T val2) is called.\n"
      << "A::A(T val) is called.\n";

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(partial_specialization, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  class_variable_alias_templates::partial_specialization_test1();

  oss << "TC general constructor function\n"
         "functest1 general version\n"
         "TC<int, int> fully partial specialization version\n"
         "functest1 partial specialization version\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

#if 0
TEST(partial_specialization, test2) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  class_variable_alias_templates::partial_specialization_test2();

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
#endif

TEST(friend_template, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  class_variable_alias_templates::test_template_class_friend();

  oss << "atmpobj.data = 5\n"
      << "atmpobj.data = 5\n"
      << "aobj1.data = 12\n"
      << "Men::funcmen is called\n"
      << "Men::funcmen is called\n"
      << "Men::funcmen is called\n"
      << "Men::funcmen is called\n"
      << "Men::funcmen is called\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

TEST(variadic_template, test1) {
  std::stringstream oss;
  testing::internal::CaptureStdout();

  class_variable_alias_templates::test_base_inherit_pack_expansion();

  oss << "PA1::PA1. \n"
         "PA2::PA2.\n"
         "PA3::PA3.\n"
         "MyClassT5::MyClassT5.\n"
         "sizeof(obj) = 600\n"
         "MyClassT6<Arg> partial specialization version is called.\n"
         "MyClassT6<Arg1, Arg2> partial specialization version is called. \n"
         "MyClassT6<Arg1, Arg2> partial specialization version is called. \n"
         "MyClassT6<First, Others...> partial specialization version is "
         "called., sizeof...(Others) = 2\n"
         "MyClassT6 generic version is called., sizeof...(Args) = 0\n"
         "MyClassT::MyClassT() generic version constructor is called.\n"
         "MyClassT::MyclassT() partial specialization constructor is called., "
         "sizeof...(Others) = 0\n"
         "MyClassT::MyclassT() partial specialization constructor is called., "
         "sizeof...(Others) = 1\n"
         "MyClassT::MyclassT() partial specialization constructor is called., "
         "sizeof...(Others) = 2\n"
         "MyClassT::MyClassT() generic version constructor is called.\n"
         "-------------------- BEGIN ------------------\n"
         "MyClassT::MyClassT(parf, ...paro) constructor is called.\n"
         "m_i = 23\n"
         "--------------------- END -------------------\n"
         "-------------------- BEGIN ------------------\n"
         "MyClassT::MyClassT(parf, ...paro) constructor is called.\n"
         "m_i = 13.5\n"
         "--------------------- END -------------------\n"
         "-------------------- BEGIN ------------------\n"
         "MyClassT::MyClassT(parf, ...paro) constructor is called.\n"
         "m_i = 12\n"
         "--------------------- END -------------------\n"
         "MyClassT2::MyClassT2() generic verion constructor is called.\n"
         "MyClassT2::MyClassT2() partial specialization version constructor is "
         "called. sizeof...(Others) = 0, First = 23\n"
         "MyClassT2::MyClassT2() partial specialization version constructor is "
         "called. sizeof...(Others) = 1, First = 18\n"
         "MyClassT2::MyClassT2() partial specialization version constructor is "
         "called. sizeof...(Others) = 2, First = 12\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}

namespace variadic_template_test {

template <typename... Args>
class Base {
public:
  Base() {
    std::cout << "Base ctor is called, sizeof...(Args) = " << sizeof...(Args)
              << std::endl;
  }
};

template <typename... Args>
class Derived1 : public Base<Args...> {
public:
  Derived1() {
    std::cout << "Derived1 ctor is called, sizeof...(Args) = "
              << sizeof...(Args) << std::endl;
  }
};

template <typename... Args>
class Derived2 : public Base<Args>... {
public:
  Derived2() {
    std::cout << "Derived2 ctor is called, sizeof...(Args) = "
              << sizeof...(Args) << std::endl;
  }
};

template <typename... Args>
class Derived3 : public Base<Args, char>... {
public:
  Derived3() {
    std::cout << "Derived3 ctor is called, sizeof...(Args) = "
              << sizeof...(Args) << std::endl;
  }
};

template <typename... Args>
class Derived4 : public Base<Args, Args...>... {
public:
  Derived4() {
    std::cout << "Derived4 ctor is called, sizeof...(Args) = "
              << sizeof...(Args) << std::endl;
  }
};

void test_variadic_template_inherit() {
  Derived1<double, float, int> temp_obj1{};
  Derived2<double, float, int> temp_obj2{};
  Derived3<double, float, int> temp_obj3{};
  Derived4<double, float, int> temp_obj4{};
}

} // namespace variadic_template_test

TEST(variadic_template_inherience, test1) {
  std::stringstream oss;

  testing::internal::CaptureStdout();

  variadic_template_test::test_variadic_template_inherit();

  oss << "Base ctor is called, sizeof...(Args) = 3\n"
         "Derived1 ctor is called, sizeof...(Args) = 3\n"
         "Base ctor is called, sizeof...(Args) = 1\n"
         "Base ctor is called, sizeof...(Args) = 1\n"
         "Base ctor is called, sizeof...(Args) = 1\n"
         "Derived2 ctor is called, sizeof...(Args) = 3\n"
         "Base ctor is called, sizeof...(Args) = 2\n"
         "Base ctor is called, sizeof...(Args) = 2\n"
         "Base ctor is called, sizeof...(Args) = 2\n"
         "Derived3 ctor is called, sizeof...(Args) = 3\n"
         "Base ctor is called, sizeof...(Args) = 4\n"
         "Base ctor is called, sizeof...(Args) = 4\n"
         "Base ctor is called, sizeof...(Args) = 4\n"
         "Derived4 ctor is called, sizeof...(Args) = 3\n";

  auto act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  debug_msg(oss, act_output);
#endif

  EXPECT_TRUE(oss.str() == act_output);
}