#include <gtest/gtest.h>
// 生产者-消费者模型是经典的多线程并发协作模型。
// 生产者用于生产数据，生产一个就往共享数据区存一个，如果共享数据区已满的话,生产者就暂停生产,
// 等待消费者的通知后再启动。
//
// 消费者用于消费数据，一个一个的从共享数据区取，如果共享数据区为空的话,消费者就暂停取数据,
// 等待生产者的通知后再启动。
//
// 生产者与消费者不能直接交互,它们之间所共享的数据使用队列结构来实现;

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace producer_consumer {

uint64_t num = 0;
class ProducerConsumer {
private:
  std::queue<uint64_t> queue_{};
  std::mutex mutex_lock_{};
  std::condition_variable condition_{};
  std::unique_ptr<std::thread> producer_;
  std::unique_ptr<std::thread> consumer_;
  bool produce_is_not_finished_flag = true;
  bool consume_is_not_finished_flag = true;
  static constexpr uint64_t total_production_count = 100;
  static constexpr uint64_t max_buffer_size = 10;

public:
  ProducerConsumer() {
    producer_ = std::make_unique<std::thread>(
        std::thread(std::bind(&ProducerConsumer::produce, this)));
    consumer_ = std::make_unique<std::thread>(
        std::thread(std::bind(&ProducerConsumer::consume, this)));
  }

  ~ProducerConsumer() {
    if (producer_->joinable())
      producer_->join();

    if (consumer_->joinable())
      consumer_->join();

    produce_is_not_finished_flag = false;
    std::cout << "ProducerConsumer end\n";
  }

  void produce() {
    int produced_production_cnt = 0;
    while (produce_is_not_finished_flag) {
      std::unique_lock<std::mutex> lock_guard{mutex_lock_};
      while (queue_.size() >= max_buffer_size)
        condition_.wait(lock_guard);

      uint64_t data = num++;
      queue_.push(data);
      std::cout << "task data = " << data << " produced\n";
      produced_production_cnt++;
      if (produced_production_cnt > total_production_count)
        produce_is_not_finished_flag = false;

      lock_guard.unlock();
      condition_.notify_all();
    }
  }

  void consume() {
    while (consume_is_not_finished_flag) {
      std::unique_lock<std::mutex> lock_guard{mutex_lock_};
      while (queue_.empty())
        condition_.wait(lock_guard);

      uint64_t data = queue_.front();
      queue_.pop();
      std::cout << "task data = " << data << " has been consumed.\n";
      if (!produce_is_not_finished_flag && queue_.empty())
        consume_is_not_finished_flag = false;

      lock_guard.unlock();
      condition_.notify_all();
    }
  }
};

} // end of namespace producer_consumer

TEST(producer_consumer, basic_test) {
  auto *pc = new producer_consumer::ProducerConsumer{};
  delete pc;
}
