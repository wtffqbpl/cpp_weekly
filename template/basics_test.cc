#include "internal_check_conds.h"
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

void template_template_parameter_test() {
  my_class<double, std::list> mylistobj2;
  mylistobj2.func();
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