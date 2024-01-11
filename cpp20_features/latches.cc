#if __cplusplus >= 202002L

#ifdef __APPLE__

#include <gtest/gtest.h>
#include <array>
#include <thread>
#include <latch>

namespace {
using namespace std::literals; // for duration literals

void loop_over(char c) {
  // loop over printing the char c:
  for (int j = 0; j < c / 2; ++j) {
    std::cout.put(c).flush();
    std::this_thread::sleep_for(100ms);
  }
}

void latch_basic_test() {
  std::array tags{'.', '?', '8', '+', '-'}; // tags we have to perform a task for

  // initialize latch to react when all tasks are done:
  std::latch all_done{tags.size()}; // initialize countdown with number of tasks.

  // start two threads dealing with every second tag:
  std::thread t1{[tags, &all_done] {
    for (unsigned  i = 0; i < tags.size(); i += 2) { // even indexes
      loop_over(tags[i]);
      // signal that the task is done:
      all_done.count_down(); // atomically decrement counter of latch.
    }
  }};

  std::thread t2{[tags, &all_done] {
    for (unsigned  i = 1; i < tags.size(); i += 2) { // odd indexes
      loop_over(tags[i]);
      // signal that the task is done:
      all_done.count_down(); // atomically decrement counter of latch.
    }
  }};

  // wait until all tasks are done:
  std::cout << "\nwaiting until all tasks are done\n";
  all_done.wait(); // wait until counter of latch is zero
  std::cout << "\nall tasks done\n"; // note: threads might still run

  t1.join();
  t2.join();
}

// A boss-worker workflow using two std::latch

std::latch work_done{6};
std::latch go_home{1};

std::mutex cout_mutex;

void synchronized_out(const std::string &s) {
  std::lock_guard<std::mutex> lo(cout_mutex);
  std::cout << s;
}

class Worker {
public:
  explicit Worker(std::string n) : name(std::move(n)) {}

  void operator()() {
    // Notify the boss when work is done
    synchronized_out(name + ": " + "Work done!\n");
    work_done.count_down();

    // Waiting before going home
    go_home.wait();
    synchronized_out(name + ": " + "Good bye!\n");
  }

private:
  std::string name;
};

void boss_worker_workflow_test() {
  std::cout << "BOSS: START WORKING! " << '\n';

  std::vector<std::string> worker_name_list{
      " Herb",      "  Scott",     "   Bjarne",
      "    Andrei", "     Andrew", "      David",
  };

  std::vector<std::thread> worker_list;
  worker_list.reserve(worker_name_list.size());

  for (const auto &name : worker_name_list)
    worker_list.emplace_back(Worker{name});

  work_done.wait();

  go_home.count_down();

  for (auto &t : worker_list)
    t.join();
}

std::latch work_done_self_managed{6};

class WorkerSelfManaged {
public:
  explicit WorkerSelfManaged(std::string n) : name(std::move(n)) {}

  void operator()() {
    synchronized_out(name + ": " + "Work done!\n");
    work_done_self_managed.arrive_and_wait();
    synchronized_out(name + ": " + "See you tomorrow!\n");
  }

private:
  std::string name;
};

void worker_self_managed_test() {
  std::vector<std::string> worker_name_list{
      " Herb",      "  Scott",     "   Bjarne",
      "    Andrei", "     Andrew", "      David",
  };

  std::vector<std::thread> worker_list;
  worker_list.reserve(worker_name_list.size());

  for (const auto &name : worker_name_list)
    worker_list.emplace_back(WorkerSelfManaged{name});

  for (auto &t : worker_list)
    t.join();
}
}

TEST(latch_test, basic_test) { latch_basic_test(); }

TEST(latch_test, test2) { boss_worker_workflow_test(); }

TEST(latch_test, test3) { worker_self_managed_test(); }
#endif // __APPLE__

#endif // __cplusplus >= 202002L
