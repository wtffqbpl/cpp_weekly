#include <iostream>
#include <cmath>

void diag_prec(int size, double *x, double *y) {
  y[0] = x[0];

  for (int i = 0; i < size; ++i)
    y[i] = 0.5 * x[i];
}

double one_norm(int size, double *vp) {
  double sum = 0;
  for (int i = 0; i < size; ++i)
    sum += fabs(vp[i]);

  return sum;
}

double dot(int size, double *vp, double *wp) {
  double sum = 0;
  for (int i = 0; i < size; ++i)
    sum += vp[i] * wp[i];

  return sum;
}

int cg(int size, double *x, double *b, void (*prec)(int, double *, double *), double eps) {

  int i, j, iter = 0;
  double rho, rho_1, alpha;
  double *p = new double[size];
  double *q = new double[size];
  double *r = new double[size];
  double *z = new double[size];

  // r = A * x;
  r[0] = 2.0 * x[0] - x[1];
  for (int i = 1; i < size - 1; ++i)
    r[i] = 2.0 * x[i] - x[i - 1] - x[i + 1];
  r[size - 1] = 2.0 * x[size - 1] - x[size - 2];

  // r = b - A * x;
  for (i = 0; i < size; ++i)
    r[i] = b[i] - r[i];

  while (one_norm(size, r) > eps) {
    prec(size, r, z);
    rho = dot(size, r, z);
    if (!iter) {
      for (i = 0; i < size; ++i)
        p[i] = z[i];
    } else {
      for (i = 0; i < size; ++i)
        p[i] = z[i] + rho / rho_1 * p[i];
    }

    // q = A * p;
    q[0] = 2.0 * p[0] - p[1];
    for (int i = 1; i < size - 1; ++i)
      q[i] = 2.0 * p[i] - p[i - 1] - p[i + 1];
    q[size - 1] = 2.0 * p[size - 1] - p[size - 2];

    alpha  = rho / dot(size, p, q);
    // x += alpha * p; r -= alpha * q;
    for (i = 0; i < size; ++i) {
      x[i] += alpha * p[i];
      r[i] -= alpha * q[i];
    }

    iter++;
  }

  delete[] q;
  delete[] p;
  delete[] r;
  delete[] z;

  return iter;
}

void ic_0(int size, double *out, double *in) { 
  /* ... */
}

template <typename Matrix, typename Vector, typename Preconditioner>
int conjugate_gradient(const Matrix &A, Vector &x, const Vector &b,
                       const Preconditioner &L, const double &eps) {
  typedef typename Collection<Vector>::value_type Scalar;
  Scalar rho = 0, rho_1 = 0, alpha = 0;
  Vector p(resource(x)), q(resource(x)), r(resource(x)), z(resource(x));

  r = b - A * x;
  int iter = 0;

  while (one_norm(size, r) >= eps) {
    prec(r, z);

    ++iter;
  }
}

int main() {
  int size = 100;

  double *x = new double[size];
  double *b = new double[size];

  // init.
  std::fill(b, b + size, 1.0);
  std::fill(x, x + size, 0.0);

  cg(size, x, b, diag_prec, 1e-9);

  return 0;
}
