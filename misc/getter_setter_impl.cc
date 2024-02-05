#include <gtest/gtest.h>

#include <functional>

#include "internal_check_conds.h"

namespace {

template <typename T> class Property {
public:
  Property() = default;

  operator const T &() const {
    // Call override getter if we have it
    if (getter)
      return getter();
    return get();
  }

  bool operator==(const T &other) const {
    // Static cast makes sure our getter operator is called, so we could use overrides if those are
    // in place
    return static_cast<const T &>(*this) == other;
  }

  // Use this to always get without overrides, useful for use with overriding implementations.
  const T &get() const { return t; }

  // Use this to always set without overrides, useful for use with overriding implementations.
  const T &set(const T &other) { return t = other; }

  // Assign getter and setter to these properties
  std::function<const T &()> getter;
  std::function<const T &(const T &)> setter;

private:
  T t;
};

// Basic usage, no override
struct MyTest {
  Property<int> prop;
};

// Override getter and setter
struct TestWithOverride {
  TestWithOverride() {
    prop.setter = [&](const int &other) {
      std::cout << "Custom setter called" << std::endl;
      return prop.set(other);
    };
    prop.setter = std::bind(&TestWithOverride::set_prop, this, std::placeholders::_1);
    prop.getter = std::bind(&TestWithOverride::get_prop, this);
  }

  Property<int> prop;

private:
  const int &get_prop() const {
    std::cout << "Custom getter called" << std::endl;
    return prop.get();
  }

  const int &set_prop(const int &other) {
    std::cout << "Custom setter called" << std::endl;
    return prop.set(other);
  }
};

} // namespace

TEST(getter_setter_test, test1) {
  MyTest t;
  TestWithOverride t1;

  std::cout << "t.prop = " << t.prop << std::endl;
}