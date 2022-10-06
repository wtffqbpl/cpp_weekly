#include "reference_cnt.h"
#include <gtest/gtest.h>

class MyClass : public reference_count::Object {
private:
  std::string name_;

public:
  explicit MyClass(std::string name) : name_(std::move(name)) {}
  std::string name() const { return name_; }

  friend std::ostream &operator<<(std::ostream &os, const MyClass &obj) {
    os << "Name: " << obj.name() << '\n';
    return os;
  }
};

TEST(reference_count, basic_test) {
  using namespace reference_count;

  ref<MyClass> obj{new MyClass{"Ren"}};

  auto ref_cnt = obj->getRefCount();
  EXPECT_EQ(ref_cnt, 1);

  obj->incRef();
  EXPECT_EQ(obj->getRefCount(), 2);
}