#ifndef NODE_OR_TOOLS_VECTOR_82AFACC9EA9A_H
#define NODE_OR_TOOLS_VECTOR_82AFACC9EA9A_H

#include <cstdint>
#include <vector>

template <typename T> class Vector {
public:
  Vector() = default;
  Vector(int n) { data.resize(n); }

  int size() const { return data.size(); }

  T& at(int x) { return data.at(x); }
  const T& at(int x) const { return data.at(x); }

private:
  std::vector<T> data;
};

#endif
