#include <gtest/gtest.h>

// 使用C++线程库启动线程，可以归结为构造std::thread对象
// 当线程启动后，一定要在和线程相关联的std::thread对象销毁前,
// 对线程运用join()或者detach()方法。
// join()与detach()都是std::thread类的成员函数，是两种线程阻塞方法,
// 两者的区别是是否等待子线程执行结束。
// 等待调用线程运行结束后当前线程再继续运行，例如，主函数中有一条语句th1.join(),
// 那么执行到这里，主函数阻塞，直到线程th1运行结束，主函数再继续运行。
// 一提到join,你脑海中就想起两个字，"等待"，而不是"加入"，这样就很容易理解join的功能。

#include <iostream>
#include <thread>

namespace create_thread {

void proc(int &a) {
  std::cout << "我是子线程, 传入参数为: " << a << '\n';
  std::cout << "子线程中显示子线程 ID: " << std::this_thread::get_id() << '\n';
}

} // end of namespace create_thread

TEST(concurrent_create_thread, basic_test) {
  std::cout << "我是主线程" << '\n';

  int a = 9;

  std::thread thread2(create_thread::proc, std::ref(a));
  std::cout << "主线程中显示子线程ID: " << thread2.get_id() << '\n';
  // 不要在创建完线程后马上join,应该在程序结束前join
  thread2.join();
}