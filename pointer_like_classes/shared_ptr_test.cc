#include <gtest/gtest.h>
#include <iostream>

// 通过这种方式来代理原来的指针。
template <typename T> class my_shared_ptr {
public:
  T &operator*() const { return *px; }
  // 指针符号-> 可以一直传递下去, 这样就可以理解 sp-> 这个符号指向到Foo
  // 对象的函数
  T *operator->() const { return px; }

  my_shared_ptr(T *_p) : px(_p) {}

private:
  T *px;
  long *pn;
};

struct Foo {
  void method(void) { std::cout << "Foo method." << std::endl; }
};

TEST(PointerLikeClass, shared_ptrTest) {
  my_shared_ptr<Foo> sp(new Foo);
  std::string expected_str = "Foo method.\n";

  testing::internal::CaptureStdout();
  sp->method();
  std::string act_output = testing::internal::GetCapturedStdout();

  EXPECT_TRUE(expected_str == act_output);
}
