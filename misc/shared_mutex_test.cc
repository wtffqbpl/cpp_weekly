#include <gtest/gtest.h>

// std::shared_mutex
// 读写锁把对共享资源的访问划分成读者和写者，读者只对共享资源进行读访问，写者则需要对共享
// 资源进行写操作。c++17开始，标准库提供了shared_mutex 类。
// 和其他便于独占访问的互斥类型不同，shared_mutex 拥有两个访问级别:
// * 共享: 多个线程能共享同一个互斥的所有权(如配合 shared_lock)
// * 独占: 仅有一个线程能占有互斥(如配合lock_guard, unique_lock)
// shared_mutex 通常用于多个读线程能同时访问同一资源而导致数据竞争，但只有一个写线程
// 能访问的情况。

#include <iostream>
#include <mutex> // std::unique_lock
#include <shared_mutex>
#include <thread>

class ThreadSafeCounter {
private:
  mutable std::shared_mutex mutex_;
  unsigned int value_ = 0;

public:
  ThreadSafeCounter() = default;

  // 多个线程/读者能同时读计数器的值
  unsigned int get() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return value_;
  }

  // 只有一个线程/写者能增加/写线程的值
  void increment() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    ++value_;
  }

  // 只有一个线程/写者能重置/写线程的值
  void reset() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    value_ = 0;
  }
};

TEST(shared_mutex, basic_test) {
  ThreadSafeCounter counter;
  auto increment_and_print = [&counter]() {
    for (unsigned i = 0; i < 3; ++i) {
      counter.increment();
      std::cout << std::this_thread::get_id() << '\t' << counter.get() << '\n';
    }
  };

  std::thread thread1(increment_and_print);
  std::thread thread2(increment_and_print);

  thread1.join();
  thread2.join();
}