#include "my_timer.h"
#include <gtest/gtest.h>
#include <future>
#include <random>

static std::set<int> make_sorted_random(const size_t num_elems) {
  std::set<int> retval;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, num_elems - 1);

  std::generate_n(std::inserter(retval, retval.end()), num_elems, [&]() { return dis(gen); });

  return retval;
}

/*
 * async & future
 *    当我们需要开启一个异步任务并在之后的某个时刻获取结果时，可以使用std::async,其参数类似于
 *    std::thread, 但是它返回的时一个future对象，最终将包含任务执行的结果。在需要获取异步结果
 *    的地方，调用std::future::get, 该方法将阻塞直到进入ready状态并返回结果。
 */
TEST(std_future_test, basic_test) {
  {
    Timer("std::future");
    auto f1 = std::async(make_sorted_random, 1000000);
    f1.get();
    std::cout << std::async(make_sorted_random, 1000000).get().size() << '\n';
  }
}

static int add(int a, int b) {
  std::cout << "work thread = " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  return a + b;
}

/*
 * @brief std::launch::deferred 任务测试
 *        add 任务创建后，一直在挂起状态，直到std::future.get() 执行的时候才执行。
 *        由于在main & add 两个线程中分别等待了3s，因此整个程序执行需要6s。
 */
TEST(std_future_test, deferred_task_test) {
  Timer("future deferred task test.");
  std::cout << "main thread = " << std::this_thread::get_id() << std::endl;
  std::future<int> result = std::async(std::launch::deferred, add, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  std::cout << result.get() << std::endl;
}

/*
 * @brief 这个case可以保证，add任务启动后就开始异步执行，main thread中等待了3s，
 *        add任务其实也已经算完了。因此整个任务只耗时3s。
 */
TEST(std_future_test, async_task_test) {
  Timer("future deferred task test.");
  std::cout << "main thread = " << std::this_thread::get_id() << std::endl;
  std::future<int> result = std::async(std::launch::async, add, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  std::cout << result.get() << std::endl;
}

static int do_something(char c) {
  // random-number generator (use c as seed to get different sequences)
  std::default_random_engine dre(c);
  std::uniform_int_distribution<int> id(10, 1000);

  // loop to print character after a random period of time.
  for (unsigned i = 0; i < 10; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
    std::cout.put(c).flush();
  }

  return c;
}

static int func1() { return do_something('.'); }
static int func2() { return do_something('+'); }

TEST(std_future_test, async_task_test2) {
  std::cout << "starting func1() in background"
            << " and func2() in foreground: " << std::endl;

  // start func1() asynchronously (now or later or never).
  std::future<int> result1(std::async(func1));
  int result2 = func2(); // call func2() synchronously (here and now)

  // print result (wait for func1() to finish and add its result to result2)
  int result = result1.get() + result2;
  std::cout << "\nresult of func1() + func2(): " << result << std::endl;
}