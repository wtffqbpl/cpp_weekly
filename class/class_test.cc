#include <gtest/gtest.h>
#include <iostream>

class Father {
public:
  virtual void show() { std::cout << "This is father." << std::endl; }
};

class Son : public Father {
public:
  void show() override { std::cout << "This is son." << std::endl; }
};

/*
 * Ref:
 * https://eli.thegreenplace.net/2012/12/17/dumping-a-c-objects-memory-layout-with-clang/
 * memory layout of structure and classes, the c/c++ operators `sizeof` and
 * `offsetof` are very useful. However large c++ class hierarchies are involved,
 * using these operators becomes tedious. Luckily, clang has a very handly
 * command-line flag to dump object layouts in a useful manner.
 *
 * clang -cc1 -fdump-record-layouts class_test.cc
 *
 * The results are as follows.
 *
 * *** Dumping AST Record Layout
 *        0 | class Father
 *        0 |   (Father vtable pointer)
 *          | [sizeof=8, dsize=8, align=8,
 *          |  nvsize=8, nvalign=8]
 *
 * *** Dumping AST Record Layout
 *        0 | class Son
 *        0 |   class Father (primary base)
 *        0 |     (Father vtable pointer)
 *          | [sizeof=8, dsize=8, align=8,
 *          |  nvsize=8, nvalign=8]
 */
TEST(inherience, derive_call_base_virtual_func) {
  Father *F = new Son{};

  std::stringstream oss;
  testing::internal::CaptureStdout();

  F->show();
  oss << "This is son.\n";
  // 如果想调用父类的虚函数，就需要加上父类的class名称，然后再加上父类的虚函数名称。
  F->Father::show();
  oss << "This is father.\n";

  std::string act_output = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << oss.str() << "Actual output:\n"
            << act_output << std::endl;
#endif

  EXPECT_TRUE(oss.str() == act_output);
}