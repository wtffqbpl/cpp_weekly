#include "internal_check_conds.h"
#include <gtest/gtest.h>

namespace override_overload_overwrite_test {

namespace override_test {

class Base {
public:
  virtual void some_func() {}
  virtual void foo(int x) {}
  virtual void bar() const {}
  void baz() {}
};

class Derived : public Base {
public:
  // These four functions are not override functions.
  virtual void sone_func() {}
  virtual void foo(int &x) {}
  virtual void bar() {}
  virtual void baz() {}
};

class Derived_final : public Base {
public:
  void foo(int x) final{};
};

class Derived2 : public Derived_final {
  // cannot override function foo
  // void foo(int x) {};
};

class BaseFile {
public:
  virtual void log(const char *) const { /* ... */
  }
  virtual void foo(int x) {}
};

class BaseWithFileLog : public BaseFile {
public:
  void log(const char *) const final { /* ... */
  }
};

class Derived_override_final : public BaseWithFileLog {
public:
  void foo(int x) override {}
};

class BaseFinal final {
public:
  virtual void foo(int x) {}
};

// class Derived_class_final : public BaseFinal {
// public:
//   void foo(int x) {}
// };

} // namespace override_test

} // namespace override_overload_overwrite_test

TEST(override_overload_overwrite_test, basic_test) {
  std::cout << "override_overload_overwrite_test test" << std::endl;
}