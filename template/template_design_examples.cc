#include "internal_check_conds.h"
#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>

namespace mtl {
template <typename T> class dense2D {
public:
  using value_type = T;
  using size_type = std::size_t;

  dense2D() = default;

  dense2D(size_type max_r, size_type max_c) : row_(max_r), column_(max_c) {
    matrix_.resize(row_);
    for (auto &row_vec : matrix_)
      row_vec.resize(column_);
  }

  dense2D(const std::initializer_list<std::initializer_list<T>> &list) {
    for (auto &line : list)
      matrix_.emplace_back(line);
    row_ = matrix_.size();
    column_ = matrix_[0].size();
  }

  value_type &operator()(size_type r, size_type c) { return matrix_[r][c]; }

private:
  std::vector<std::vector<T>> matrix_;
  size_type row_, column_;
};

template <typename Matrix> class transpose_view {
public:
  using value_type = typename Matrix::value_type;
  using size_type = typename Matrix::size_type;

  explicit transpose_view(Matrix &A) : ref_(A) {}

  value_type &operator()(size_type r, size_type c) { return ref_(c, r); }
  const value_type &operator()(size_type r, size_type c) const {
    return ref_(c, r);
  }

private:
  Matrix &ref_;
};

template <typename Matrix> inline transpose_view<Matrix> trans(Matrix &A) {
  return transpose_view<Matrix>(A);
}

// If we construct a constant matrix, then the following code would be failed
// during compile-time.
/// \code
/// dense2D<float> A{{2, 3, 4}, {5, 6, 7}, {8, 9, 10}};
/// const dense2D<float> B(A);
/// std::cout << "trans(B)(2, 0) = " << trans(B)(2, 0) << '\n'; // compile
/// failed. \endcode

// An alternative way to solve this problem is that we define a
// const_transpose_view class for const

template <typename Matrix> class const_transpose_view {
public:
  using value_type = typename Matrix::value_type;
  using size_type = typename Matrix::size_type;

  explicit const_transpose_view(const Matrix &A) : ref(A) {}

  const value_type &operator()(size_type r, size_type c) const {
    return ref(c, r);
  }

private:
  const Matrix &ref;
};

template <typename Matrix>
inline const_transpose_view<Matrix> trans(const Matrix &A) {
  return const_transpose_view<Matrix>(A);
}

// Some utilities:
template <typename Matrix> struct transpose_view_f;

// is_matrix declaration
template <typename T> struct is_matrix : std::false_type {};

// specialize template for const specifier.
template <typename T> struct is_matrix<const T> : is_matrix<T> {};

// partial-specialization for matrix
template <typename Value> struct is_matrix<dense2D<Value>> : std::true_type {};

// partial-specialization for transpose_view_f class
template <typename Matrix>
struct is_matrix<transpose_view_f<Matrix>> : is_matrix<Matrix> {};

// This is redundant, because the difference between const_transpose_view and
// transpose_view is no `operator(size_type, size_type) const` function only.

// In this situation, we can use `Compile-time Branching` technique:
// std::conditional The final transpose_view definition:

template <typename Matrix> class transpose_view_f {
  static_assert(is_matrix<Matrix>::value,
                "Argument of this view must be a matrix!");

public:
  using value_type = typename Matrix::value_type;
  using size_type = typename Matrix::size_type;

private:
  using vref_type = std::conditional_t<std::is_const_v<Matrix>,
                                       const value_type &, value_type &>;

public:
  explicit transpose_view_f(Matrix &A) : ref(A) {}

  vref_type operator()(size_type r, size_type c) { return ref(r, c); }
  const value_type &operator()(size_type r, size_type c) const {
    return ref(r, c);
  }

private:
  vref_type ref;
};

} // namespace mtl