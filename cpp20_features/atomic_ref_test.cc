#if __cplusplus >= 202002L

#ifdef __APPLE__

#include <algorithm>
#include <array>
#include <atomic>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <stop_token>
#include <thread>
#include <vector>

using namespace std::literals; // for duration literals

namespace {

// std::atomic_ref
//  * `std::atomic_ref` is a template class introduced in C++20, and it is part
//  of
//    the `<atomic>` header.
//  * It is designed to provide atomic-like operations on non-atomic variables
//    or non-atomic parts of atomic variables.
//  * Unlike `std::atomic`, it does not introduce any overhead or memory
//    synchronization, making it useful for fine-grained atomic-like operations
//    within a single thread.
// It's important to note that `std::atomic_ref` does not provide atomic
// guarantees across threads. It is meant for cases where atomic-like operations
// are needed within a single thread, without the need for full memory
// synchronization.
// `std::atomic` is used for creating variables with atomic operations and
// synchronization across threads, while `std::atomic_ref` is used for
// atomic-like operations on non-atomic variables within a single thread
// without introducing the synchronization overhead. The introduction of
// `std::atomic_ref` in C++20 provides a way to perform certain atomic-like
// operations more efficiently when full atomicity and synchronization are not
// necessary.
//
// `std::atomic_ref` is a C++20 feature that provides atomic-like operations on
// non-atomic variables or non-atomic parts of atomic variables within a single
// thread. Here's a simple example using `std::atomic_ref`:

void basic_test() {
  int value = 42;

  // Create an atomic reference to a non-atomic variable
}

} // namespace

#endif // __APPLE__

#endif // __cplusplus >= 202002L
