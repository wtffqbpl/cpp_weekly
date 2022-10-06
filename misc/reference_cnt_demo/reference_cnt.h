#pragma once
#include <atomic>
#include <iostream>

namespace reference_count {

class Object {
public:
  /// Default constructor
  Object() = default;

  /// Copy constructor
  Object(const Object &) : ref_count_(0) {}

  /// Return the current reference count
  [[nodiscard]] int getRefCount() const { return ref_count_; };

  /// Increase the object's reference count by one
  void incRef() const { ++ref_count_; }

  void decRef(bool dealloc = true) const noexcept {
    --ref_count_;
    if (ref_count_ == 0 && dealloc)
      delete this;
    else if (ref_count_ < 0) {
      std::cerr << "Internal error: Object reference count < 0!\n";
    }
  }

protected:
  virtual ~Object() = default;

private:
  mutable std::atomic<int> ref_count_{0};
};

template <typename T> class ref {
public:
  /// Create a ``nullptr``-valued reference
  ref() = default;

  /// Construct a reference from a pointer
  explicit ref(T *ptr) : ptr_(ptr) {
    if (ptr_)
      ((Object *)ptr_)->incRef();
  }

  /// Copy constructor
  ref(const ref &r) : ptr_(r.ptr_) {
    if (ptr_)
      ((Object *)ptr_)->incRef();
  }

  /// Move constructor
  ref(ref &&r) noexcept : ptr_(r.ptr_) { r.ptr_ = nullptr; }

  /// Destroy this reference
  ~ref() {
    if (ptr_)
      ((Object *)ptr_)->decRef();
  }

  /// Move another reference into the current one
  ref &operator=(ref &&r) noexcept {
    if (&r != this) {
      if (ptr_)
        ((Object *)ptr_)->decRef();
      ptr_ = r.ptr_;
      r.ptr_ = nullptr;
    }
    return *this;
  }

  /// Overwrite this reference with another reference
  ref &operator=(const ref &r) noexcept {
    if (ptr_ != r.ptr_) {
      if (r.ptr_)
        ((Object *)r.ptr_)->incRef();
      if (ptr_)
        ((Object *)ptr_)->decRef();
      ptr_ = r.ptr_;
    }
    return *this;
  }

  /// Overwrite this reference with a pointer to another object
  ref &operator=(T *ptr) noexcept {
    if (ptr_ != ptr) {
      if (ptr)
        ((Object *)ptr)->incRef();
      if (ptr_)
        ((Object *)ptr_)->decRef();
      ptr_ = ptr;
    }
    return *this;
  }

  /// Compare this reference with another reference
  bool operator==(const ref &r) const { return ptr_ == r.ptr_; }

  /// Compare this reference with another reference
  bool operator!=(const ref &r) const { return ptr_ != r.ptr_; }

  /// Compare this reference with a pointer
  bool operator==(const T *ptr) const { return ptr_ == ptr; }

  /// Compare this reference with a pointer
  bool operator!=(const T *ptr) const { return ptr_ != ptr; }

  /// Access the object referenced by this reference
  T *operator->() { return ptr_; }

  /// Access the object referenced by this reference
  const T *operator->() const { return ptr_; }

  /// Return a C++ reference to the referenced object
  T &operator*() { return *ptr_; }

  /// Return a const C++ reference to the referenced object
  const T &operator*() const { return *ptr_; }

  /// Return a pointer to the referenced object
  explicit operator T *() { return ptr_; }

  /// Return a const pointer to the referenced object
  T *get() { return ptr_; }

  /// Return a pointer to the referenced object
  const T *get() const { return ptr_; }

  /// Check if the object is defined
  explicit operator bool() const { return ptr_ != nullptr; }

private:
  T *ptr_ = nullptr;
};

} // end of namespace reference_count