#pragma once

// DESCRIPTION:
//  Illustrates a basic memory manager with a move-constructor and move-assign
//  operator. As per:
//    https://stackoverflow.com/questions/18085383/c11-rvalue-reference-calling-copy-constructor-too
//  move-constructor and move-assign-operator need to be marked as noexcept.
//  It's a good exercise to remove noexcept and re-run this code (i.e. the
//  move special functions).
//
//  Originally published here:
//    https://docs.microsoft.com/en-us/cpp/cpp/move-constructors-and-move-assignment-operators-cpp?view=vs-2017

#include <algorithm>
#include <iostream>

class MemoryBlock {
private:
  size_t length_; // The length of the resources.
  int *data_;     // The resource

public:
  explicit MemoryBlock(size_t len);
  // copy constructor
  MemoryBlock(const MemoryBlock &mem_obj);
  // copy assignment operator
  MemoryBlock &operator=(const MemoryBlock &mem_obj);
  // move constructor
  MemoryBlock(MemoryBlock &&mem_obj) noexcept;
  // move assignment operator
  MemoryBlock &operator=(MemoryBlock &&mem_obj) noexcept;
  // destructor
  ~MemoryBlock();

  // retrieves the length of the data resource.
  [[nodiscard]] size_t length() const { return length_; }
};

MemoryBlock::MemoryBlock(size_t len) : length_(len), data_(new int[len]) {
  std::cout << "In MemoryBlock(size_t). length = " << length_ << ".\n";
}

MemoryBlock::~MemoryBlock() {
  std::cout << "In ~MemoryBlock(). length = " << length_ << ".\n";
  if (data_ != nullptr)
    std::cout << "Deleting resource.\n";
  delete data_;
}

MemoryBlock &MemoryBlock::operator=(const MemoryBlock &mem_obj) {
  std::cout << "In operator=(const MemoryBlock &). length = "
            << mem_obj.length() << ". "
            << "Copying resource.\n";
  if (this == &mem_obj)
    return *this;

  // free the existing resource.
  delete[] data_;

  length_ = mem_obj.length_;
  data_ = new int[length_];
  std::copy(mem_obj.data_, mem_obj.data_ + length_, data_);

  return *this;
}

MemoryBlock::MemoryBlock(MemoryBlock &&mem_obj) noexcept
    : length_(0), data_(nullptr) {
  std::cout << "In MemoryBlock(MemoryBlock &&). length = " << mem_obj.length_
            << ". Moving resource.\n";

  // Copy the data pointer and its length from the source object.
  data_ = mem_obj.data_;
  length_ = mem_obj.length_;

  // release the data pointer from the source object so that the destructor
  // does not free the memory multiple times.
  mem_obj.data_ = nullptr;
  mem_obj.length_ = 0;
}

MemoryBlock &MemoryBlock::operator=(MemoryBlock &&mem_obj) noexcept {
  std::cout << "In operator=(MemoryBlock &&). length = " << mem_obj.length_
            << ".\n";

  // is this is the specified MemoryBlock object, return this directly.
  if (this == &mem_obj)
    return *this;

  // free the existing resource.
  delete[] data_;

  // copy the data pointer and its length from the source object.
  data_ = mem_obj.data_;
  length_ = mem_obj.length_;

  // release the data pointer from the source object so that the destructor
  // does not free the memory multiple times.
  mem_obj.data_ = nullptr;
  mem_obj.length_ = 0;

  return *this;
}