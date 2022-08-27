#include <gtest/gtest.h>
#include <initializer_list>

class IntArray {
public:
  IntArray() = default;
  explicit IntArray(int Length) : Length_(Length), Data_{new int[Length]{}} {}
  IntArray(std::initializer_list<int> List) // allow IntArray to be initialized
                                            // via list initialization.
      : IntArray(static_cast<int>(List.size())) { // use delegating constructor
                                                  // to set up initial array
    // Now initialize our array from the list.

    int Count{0};
    for (auto Ele : List)
      Data_[Count++] = Ele;
  }

  ~IntArray() {
    delete[] Data_;
    // we don't need to set Data_ to nullptr or Length_ to 0 here, since the
    // object will be destroyed immediately after this function anyway.
  }

  IntArray(const IntArray &) = delete;                // to avoid shallow copies
  IntArray &operator=(const IntArray &list) = delete; // to avoid shallow copies

  int &operator[](int Index) {
    assert(Index >= 0 && Index < Length_);
    return Data_[Index];
  }

  [[nodiscard]] int getLength() const { return Length_; }

  // iterator.
  int *begin() { return Data_; }
  int *end() { return Data_ + Length_; }

private:
  int Length_{};
  int *Data_{};
};

TEST(initialize_list, basic_test) {
  IntArray Array{5, 4, 3, 2, 1}; // initializer list.

  std::ostringstream Oss;

  testing::internal::CaptureStdout();
  std::copy(Array.begin(), Array.end(),
            std::ostream_iterator<int>(std::cout, " "));

  Oss << "5 4 3 2 1 ";
  std::string ActOutput = testing::internal::GetCapturedStdout();

#ifndef NDEBUG
  std::cout << "Expected output:\n"
            << Oss.str() << '\n'
            << "Actual output:\n"
            << ActOutput << '\n';
#endif

  EXPECT_TRUE(Oss.str() == ActOutput);
}