#include "internal_check_conds.h"

#include <gtest/gtest.h>
#include <iostream>

namespace crtp {
// Returning Proxies for multidimensional vectors.

template <typename Matrix, typename Result> struct BracketProxy {
  BracketProxy(Matrix &A, size_t r) : A(A), r(r) {}

  Result &operator[](size_t c) { return A[r][c]; }

private:
  Matrix &A;
  size_t r;
};

template <typename Matrix, typename Result> class MatrixCRTP {
  using ConstProxy = BracketProxy<const Matrix, const Result>;

  BracketProxy<Matrix, Result> operator[](size_t r) {
    return {static_cast<Matrix &>(*this), r};
  }

  ConstProxy operator[](size_t r) const {
    return {static_cast<const Matrix &>(*this), r};
  }
};

class Matrix : public MatrixCRTP<Matrix, double> {
  // ...
};

} // namespace crtp