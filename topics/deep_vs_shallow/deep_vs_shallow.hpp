#pragma once

#include <cassert>
#include <iostream>
#include <string>

class Shallow {
private:
  int *data_ = nullptr;
  constexpr static size_t capacity_ = 10;
  size_t num_elements_ = 0;

public:
  explicit Shallow() { data_ = new int[capacity_]; }

  Shallow(const Shallow &rhs) {
    data_ = rhs.data_;
    num_elements_ = rhs.num_elements_;
  }
  ~Shallow() {
    delete[] data_;
    data_ = nullptr;
  }

  [[nodiscard]] int *begin() { return data_; }
  [[nodiscard]] int *end() { return data_ + num_elements_; }

  void add(int elem) {
    if (num_elements_ >= capacity_) {
      std::cerr << "Full buffer, exiting.\n";
      return;
    }
    data_[num_elements_++] = elem;
  }
  int &getElemAt(unsigned idx) {
    assert(idx < num_elements_ && "index is out of range");
    return data_[idx];
  }
};

class Deep {
private:
  int *data_ = nullptr;
  constexpr static size_t capacity_ = 10;
  size_t num_elements_ = 0;

public:
  explicit Deep() { data_ = new int[capacity_](); }
  Deep(const Deep &rhs) {
    data_ = new int[capacity_];
    for (size_t i = 0; i < capacity_; ++i)
      data_[i] = rhs.data_[i];
    num_elements_ = rhs.num_elements_;
  }
  ~Deep() { delete[] data_; }
  void add(int elem) {
    if (num_elements_ >= capacity_) {
      std::cerr << "Full buffer, exiting.\n";
      return;
    }
    data_[num_elements_++] = elem;
  }
  int &getElemAt(unsigned idx) {
    assert(idx < num_elements_ && "index is out of range.");
    return data_[idx];
  }

  [[nodiscard]] int *begin() { return data_; }
  [[nodiscard]] int *end() { return data_ + num_elements_; }
};