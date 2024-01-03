#if __cplusplus >= 202002L

#ifdef __APPLE__

#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>
#include <stop_token>
#include <future>

namespace {

// Semaphores are a synchronization mechanism used to control concurrent access
// to a shared resource. A counting semaphore is a semaphore is a semaphore that
// has a counter that is bigger than zero. The constructor initializes the
// counter. Acquiring the semaphore decreases the counter, and releasing the
// semaphore increases the counter. If a thread tries to acquire the semaphore
// when the counter is zero, the thread will block until another thread
// increments the counter by releasing the semaphore.
//
// Edsger W. Dijkstra presented in 1965 the concept of a semaphore. A semaphore
// is a data structure with a queue and a counter. The counter is initialized to
// a value equal to or greater than zero. It supports the two operations `wait`
// and `signal`. Operation `wait` acquires the semaphore and decreases the
// counter. It blocks the thread from acquiring the semaphore if the counter is
// zero. Operation `signal` releases the semaphore and increases the counter.
// Blocked threads are added to the queues to avoid starvation.
// Originally, a semaphore was a railway signal.
// std::binary_semaphore, which is an alias for a std::counting_semaphore<1>.
// In contrast to a std::mutex, a std::counting_semaphore is not bound to a
// thread. This means that the acquiring and releasing of a semaphore call can
// happen on different threads.
/// \code
/// using binary_semaphore = std::counting_semaphore<1>;
/// \endcode
// Semaphores are typically used in sender-receiver workflows. For example,
// initializing the semaphore `sem` with 0 will block the receiver's
// `sem.acquire()` call until the sender calls `sem.release()`. Consequently,
// the receiver waits for the notification of the sender. On-time
// synchronization of threads can easily be implemented using semaphores.

// clang-format off
// Member functions of a std::counting_semaphore sem
// ------------------------------------------------------------------------------------------------------------------------------------
// |    Member Function                 |         Description                                                                         |
// -------------------------------------+---------------------------------------------------------------------------------------------+
// | std::semaphore sem{num}            | Creates a semaphore with the counter num.                                                   |
// | sem.max() (static)                 | Returns the maximum value of the counter.                                                   |
// | sem.release(upd = 1)               | Increases counter by `upd` and subsequently unblocks threads acquiring the semaphore sem.   |
// | sem.acquire()                      | Decrements the `counter` by 1 or blocks until the `counter` is greater than 0.              |
// | sem.try_acquire()                  | Tries to decrement the `counter` by 1 if it is greater than 0.                              |
// | sem.try_acquire_for(rel_time)      | Tries to decrement the `counter` by 1 or blocks for at most `rel_time` if the counter is 0. |
// | sem.try_acquire_until(abs_time)    | Tries to decrement the counter by 1 or blocks at most until `abs_time` if the counter is 0. |
// ------------------------------------------------------------------------------------------------------------------------------------
// clang-format on

void semaphores_basic_test() {
  std::queue<char> values; // queue of values
  std::mutex values_mtx; // mutex to protect access to the queue

  // initialize a queue with multiple sequences from 'a' to 'z':
  for (int i = 0; i < 1000; ++i) {
    values.push(static_cast<char>('a' + (i % ('z' - 'a'))));
  }

  // create a pool of num_threads threads:
  // - limit their availability with a semaphore (initially none available):
  constexpr int num_threads = 10;
  std::counting_semaphore<num_threads> enabled{0};

  // create and start all threads of the pool:
  std::vector<std::thread> pool;
  for (int idx = 0; idx < num_threads; ++idx) {
    // std::thread t{[idx, &enabled, &values, &values_mtx] (std::stop_token st) { }};
    // pool.push_back(std::move(t));
  }
}

// Allow up to 3 threads to access the resource
std::counting_semaphore<3> resource_semaphore(3);

void worker_thread(int id) {
  using namespace std::chrono_literals;

  auto time_start = std::chrono::system_clock::now();

  while (true) {
    // Acquire a permit to access the resource
    resource_semaphore.acquire();

    // Simulate using the shared resource
    std::cout << "Thread " << id << " is using the resource." << std::endl;
    std::this_thread::sleep_for(200ms);

    // Release the permit
    resource_semaphore.release();

    // Exit threads
    if ((std::chrono::system_clock::now() - time_start) > 5s)
      break;
  }
}

void counting_semaphore_test() {
  std::vector<std::thread> threads;

  for (int i = 1; i <= 5; ++i)
    threads.emplace_back(worker_thread, i);

  for (auto &thread : threads)
    thread.join();
}

std::vector<int> my_vec{};

std::counting_semaphore<1> prepare_signal(0);

void prepare_work() {
  my_vec.insert(my_vec.end(), {0, 1, 0, 3});
  std::cout << "Sender: Data prepared." << '\n';
  prepare_signal.release();
}

void complete_work() {
  std::cout << "Waiter: Waiting for data." << '\n';
  prepare_signal.acquire();

  my_vec[2] = 2;
  std::cout << "Waiter: Complete the work." << '\n';
  std::copy(my_vec.begin(), my_vec.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
}

void counting_semaphore_test2() {
  std::thread t1(prepare_work);
  std::thread t2(complete_work);

  t1.join();
  t2.join();
}

} // namespace

TEST(semaphore_test, test1) {
  counting_semaphore_test();
}

TEST(semaphore_test, test2) {
  counting_semaphore_test2();
}

#endif // __APPLE__

#endif // __cplusplus >= 202002L
