#ifndef NODE_OR_TOOLS_ADAPTORS_F1FF74E9681C_H
#define NODE_OR_TOOLS_ADAPTORS_F1FF74E9681C_H

#include <nan.h>

#include "types.h"

// We adapt matrices and vectors for or-tools since it expects them to have specific signatures and types.

// Matrix to operator()(NodeIndex, NodeIndex);
template <typename T>
auto makeBinaryAdaptor(const T& m) {
    return [&](NodeIndex from, NodeIndex to) -> int64 { return m.at(from.value(), to.value()); };
}

// Vector to operator()(NodeIndex);
template <typename T>
auto makeUnaryAdaptor(const T& v) {
    return [&](NodeIndex idx) -> int64 { return v.at(idx.value()); };
}

// Adaptors to callback. Note: ownership is bound to the underlying storage.
template <typename Adaptor>
auto makeCallback(const Adaptor& adaptor) {
    return NewPermanentCallback(&adaptor, &Adaptor::operator());
}

// Caches user provided Js Array into a Vector
template <typename Vector>
inline auto makeVectorFromJsNumberArray(v8::Local<v8::Array> array) {
    const std::uint32_t len = array->Length();

    Vector vec(static_cast<std::int32_t>(len));

    for (std::uint32_t atIdx = 0; atIdx < len; ++atIdx) {
        auto num = Nan::Get(array, atIdx).ToLocalChecked();

        if (!num->IsNumber())
            throw std::runtime_error{"Expected array element of types Number"};

        vec.at(static_cast<std::int32_t>(atIdx)) = Nan::To<std::int32_t>(num).FromJust();
    }

    return vec;
}

#endif
