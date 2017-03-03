#ifndef NODE_OR_TOOLS_VECTOR_82AFACC9EA9A_H
#define NODE_OR_TOOLS_VECTOR_82AFACC9EA9A_H

#include <cstdint>
#include <vector>

template <typename T> class Vector {
public:
  Vector() = default;
  Vector(std::int64_t n) { data.resize(n); }

  std::int64_t size() const { return data.size(); }

  T& at(std::int64_t x) { return data.at(x); }
  const T& at(std::int64_t x) const { return data.at(x); }

private:
  std::vector<T> data;
};

#endif
