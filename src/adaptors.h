#ifndef NODE_OR_TOOLS_ADAPTORS_F1FF74E9681C_H
#define NODE_OR_TOOLS_ADAPTORS_F1FF74E9681C_H

#include <nan.h>

#include "types.h"

// We adapt matrices and vectors for or-tools since it expects them to have specific signatures and types.

// Matrix to operator()(NodeIndex, NodeIndex);
template <typename T> auto makeBinaryAdaptor(const T& m) {
  return [&](NodeIndex from, NodeIndex to) -> int64 { return m.at(from.value(), to.value()); };
}

// Vector to operator()(NodeIndex);
template <typename T> auto makeUnaryAdaptor(const T& v) {
  return [&](NodeIndex idx) -> int64 { return v.at(idx.value()); };
}

// Adaptors (see above) to callback
template <typename Adaptor> auto makeCallback(const Adaptor& adaptor) {
  return NewPermanentCallback(&adaptor, &Adaptor::operator());
}

// Caches user provided Function(s, t) -> Number into Matrix
template <typename Matrix> inline auto makeMatrixFromFunction(int n, v8::Local<v8::Function> fn) {
  Nan::Callback callback{fn};

  Matrix matrix{n};

  for (auto fromIdx = 0; fromIdx < n; ++fromIdx) {
    for (auto toIdx = 0; toIdx < n; ++toIdx) {
      const auto argc = 2u;
      v8::Local<v8::Value> argv[argc] = {Nan::New(fromIdx), Nan::New(toIdx)};

      auto cost = callback.Call(argc, argv);

      if (!cost->IsNumber())
        throw std::runtime_error{"Expected function signature: Number fn(Number from, Number to)"};

      matrix.at(fromIdx, toIdx) = Nan::To<int>(cost).FromJust();
    }
  }

  return matrix;
}

// Caches user provided Function(node) -> [start, stop] into TimeWindows
inline auto makeTimeWindowsFromFunction(int n, v8::Local<v8::Function> fn) {
  Nan::Callback callback{fn};

  TimeWindows timeWindows{n};

  for (auto atIdx = 0; atIdx < n; ++atIdx) {
    const auto argc = 1u;
    v8::Local<v8::Value> argv[argc] = {Nan::New(atIdx)};

    auto interval = callback.Call(argc, argv);

    if (!interval->IsArray())
      throw std::runtime_error{"Expected function signature: Array fn(Number at)"};

    auto intervalArray = interval.As<v8::Array>();

    if (intervalArray->Length() != 2)
      throw std::runtime_error{"Expected interval array of shape [start, stop]"};

    auto start = Nan::Get(intervalArray, 0).ToLocalChecked();
    auto stop = Nan::Get(intervalArray, 1).ToLocalChecked();

    if (!start->IsNumber() || !stop->IsNumber())
      throw std::runtime_error{"Expected interval start and stop of type Number"};

    Interval out{Nan::To<int>(start).FromJust(), Nan::To<int>(stop).FromJust()};
    timeWindows.at(atIdx) = std::move(out);
  }

  return timeWindows;
}

#endif
