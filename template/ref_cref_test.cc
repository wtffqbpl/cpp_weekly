#include <gtest/gtest.h>
#include <functional>
#include <iostream>

/*
 * @brief c++本身有引用，使用std::ref 可以在模版传参的时候传入引用，否则只能传值。主要是考虑
 *        到函数式编程(如std::bind) 在使用时，是对参数直接拷贝，而不是引用。
 *
 *   一些说明:
 *      std::ref 并不改变模版函数中参数的处理，而是使用了一个技巧：将传入的参数进行包装，
 *      使得它表现地像个引用。事实上，它创建了一个对象，类型为std::reference_wrapper<>，
 *      指向原来的实参并以值方式传递该对象。该包装或多或少只支持一个操作：隐式类型转换为
 *      原来的类型，得到原来的对象（可以在引用包装上调用get() 方法，将它作为函数对象).
 */
void f(int &n1, int &n2, const int &n3) {
  std::cout << "In function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
  ++n1;
  ++n2;
}

TEST(ref_cref_test, basic_test1) {
  int n1 = 1, n2 = 2, n3 = 3;
  // 这里n1 使用的是拷贝，而不是引用。如果想传入引用，就需要使用std::ref 来将引用传进去。
  // 至于这里std::bind 为什么默认要拷贝而不是引用，可能有一些需求吧。
  std::function<void()> bound_f = std::bind(f, n1, std::ref(n2), std::cref(n3));
  n1 = 10;
  n2 = 11;
  n3 = 12;

  std::stringstream oss;
  testing::internal::CaptureStdout();

  std::cout << "Before function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
  oss << "Before function: 10 11 12\n";

  bound_f();
  oss << "In function: 1 11 12\n";

  std::cout << "After function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
  oss << "After function: 10 12 12\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output: \n" << oss.str()
            << "Actual output: \n" << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}
