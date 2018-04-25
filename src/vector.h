#ifndef NODE_OR_TOOLS_VECTOR_82AFACC9EA9A_H
#define NODE_OR_TOOLS_VECTOR_82AFACC9EA9A_H

#include <cstdint>
#include <vector>

template <typename T>
class Vector {
  public:
    using Value = T;

    Vector() = default;
    Vector(std::int32_t n) { data.resize(static_cast<std::size_t>(n)); }

    std::int32_t size() const { return static_cast<std::int32_t>(data.size()); }

    T& at(std::int32_t x) { return data.at(static_cast<std::size_t>(x)); }
    const T& at(std::int32_t x) const { return data.at(static_cast<std::size_t>(x)); }

  private:
    std::vector<T> data;
};

#endif
