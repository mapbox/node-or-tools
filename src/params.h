#ifndef NODE_OR_TOOLS_PARAMS_90D22929381A_H
#define NODE_OR_TOOLS_PARAMS_90D22929381A_H

#include <nan.h>

#include <cstdint>

#include <stdexcept>

// Caches user provided 2d Array of Numbers into Matrix
template <typename Matrix>
inline auto makeMatrixFrom2dArray(std::int32_t n, v8::Local<v8::Array> array) {
    if (n < 0)
        throw std::runtime_error{"Negative dimension"};

    if (static_cast<std::int32_t>(array->Length()) != n)
        throw std::runtime_error{"Array dimension do not match size"};

    Matrix matrix(n);

    for (std::int32_t fromIdx = 0; fromIdx < n; ++fromIdx) {
        auto inner = Nan::Get(array, fromIdx).ToLocalChecked();

        if (!inner->IsArray())
            throw std::runtime_error{"Expected Array of Arrays"};

        auto innerArray = inner.As<v8::Array>();

        if (static_cast<std::int32_t>(array->Length()) != n)
            throw std::runtime_error{"Inner Array dimension do not match size"};

        for (std::int32_t toIdx = 0; toIdx < n; ++toIdx) {
            auto num = Nan::Get(innerArray, toIdx).ToLocalChecked();

            if (!num->IsNumber())
                throw std::runtime_error{"Expected 2d Array of Numbers"};

            auto value = Nan::To<std::int32_t>(num).FromJust();

            matrix.at(fromIdx, toIdx) = value;
        }
    }

    return matrix;
}

#endif
