#include "counter.h"
#include <gtest/gtest.h>

// inline variable
// inline variable 和 inline function 或者 template 类似，只要 Translation unit
// 有用到该 inline variable, 编译器产生的object file就会包含一份完整的定义.
// 然而编译器会将 inline variable
// 标上特别的记号,当链接器看到该记号，就会将同名的 inline variable
// 再合并为一个变量。这也保证每一个有external linkage 的 inline variable
// 都有一个地址。

namespace inline_variable {

extern void inline_variable_test();

void inline_variable_main() {
  std::cout << "inline_variable_main: value = " << g_counter << ", "
            << "ptr = " << static_cast<const void *>(&g_counter) << '\n';
  dump();
  std::cout << "inline_variable_main: value = " << g_counter << ", "
            << "ptr = " << static_cast<const void *>(&g_counter) << '\n';
  inline_variable_test();
  std::cout << "inline_variable_main: value = " << g_counter << ", "
            << "ptr = " << static_cast<const void *>(&g_counter) << '\n';
}

} // end of namespace inline_variable

TEST(inline_variable, basic_test) { inline_variable::inline_variable_main(); }

// inline variable 初始化顺序
// 在同一个translation
// unit之内，一般的全局变量会在main函数开始执行前会依次从上到下 进行初始化.
// 然而C++标准不保证inline variable 与 非 inline variable 之间的先后 顺序。
// inline variable 与 inline variable 之间的顺序也取决于是不是所有 translation
// unit 都以相同的顺序定义二者.

namespace inline_variable_initialize {
Example main0{"main0"};
inline Example b{"b"};
inline Example a{"a"};

Example main1{"main1"};

} // end of namespace inline_variable_initialize

TEST(inline_variable, initialize_test) {
  using namespace inline_variable_initialize;
  std::cout << "main0 = " << static_cast<const void *>(&main0) << '\n';
  std::cout << "b = " << static_cast<const void *>(&b) << '\n';
  std::cout << "a = " << static_cast<const void *>(&a) << '\n';
  std::cout << "main1 = " << static_cast<const void *>(&main1) << '\n';
}