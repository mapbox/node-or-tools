#ifndef NODE_OR_TOOLS_MATRIX_F83F49233E85_H
#define NODE_OR_TOOLS_MATRIX_F83F49233E85_H

#include <cstdint>
#include <vector>

template <typename T> class Matrix {
  static_assert(std::is_arithmetic<T>::value, "Matrix<T> requires T to be integral or floating point");

public:
  using Value = T;

  Matrix() = default;
  Matrix(std::int32_t n_) : n{n_} {
    if (n < 0)
      throw std::runtime_error{"Negative dimension"};

    data.resize(n * n);
  }

  std::int32_t dim() const { return n; }
  std::int32_t size() const { return dim() * dim(); }

  T& at(std::int32_t x, std::int32_t y) { return data.at(y * n + x); }
  const T& at(std::int32_t x, std::int32_t y) const { return data.at(y * n + x); }

private:
  std::int32_t n;
  std::vector<T> data;
};

#endif
