#include <iostream>

class stone {
public:
  stone(int _w, int _h, int _we=0) : w_(_w), h_(_h), weight_(_we) {}
  stone() = default;

  // step 3: stone的 < 符号来进行判断.
  bool operator<(const stone &rhs) const {
    return weight_ < rhs.weight_;
  }

  int width() const { return w_; }
  int height() const { return h_; }
  int weight() const { return weight_; }

private:
  int w_, h_, weight_;
};

template<typename T>
inline const T& min(const T& a, const T& b) {
  // step 2: 实参推导结果为 T 即stone，于是调用 stone::operator<
  return b < a ? b : a;
}

int main() {
  stone r1(2, 3), r2(3, 3), r3;

  // step 1: compiler会实参推导 (argument deduction)
  r3 = min(r1, r2);

  std::cout << "(width, height, weight) = (" <<
    r3.width() << ", " << r3.height() << ", " << r3.weight() << ")"
    << std::endl;

  return 0;
}
