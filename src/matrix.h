#ifndef NODE_OR_TOOLS_MATRIX_F83F49233E85_H
#define NODE_OR_TOOLS_MATRIX_F83F49233E85_H

#include <cstdint>
#include <vector>

template <typename T> class Matrix {
  static_assert(std::is_arithmetic<T>::value, "matrix<T> requires T to be integral or floating point");

public:
  Matrix(std::size_t n_) : n{n_} { data.resize(n * n); }

  std::size_t dim() const { return n; }

  T& operator()(std::size_t x, std::size_t y) { return data.at(y * n + x); }
  const T& operator()(std::size_t x, std::size_t y) const { return data.at(y * n + x); }

private:
  std::size_t n;
  std::vector<T> data;
};

#endif
