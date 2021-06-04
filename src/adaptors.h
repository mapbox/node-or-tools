#ifndef NODE_OR_TOOLS_ADAPTORS_F1FF74E9681C_H
#define NODE_OR_TOOLS_ADAPTORS_F1FF74E9681C_H

#include <nan.h>
#include <iostream>
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

// Adaptors to callback. Note: ownership is bound to the underlying storage.
template <typename Adaptor> auto makeCallback(const Adaptor& adaptor) {
  return NewPermanentCallback(&adaptor, &Adaptor::operator());
}

// Caches user provided Function(s, t) -> Number into Matrix
template <typename Matrix> inline auto makeMatrixFromFunction(std::int32_t n, v8::Local<v8::Function> fn) {
  if (n < 0)
    throw std::runtime_error{"Negative dimension"};

  Nan::Callback callback{fn};

  Matrix matrix{n};

  for (std::int32_t fromIdx = 0; fromIdx < n; ++fromIdx) {
    for (std::int32_t toIdx = 0; toIdx < n; ++toIdx) {
      const auto argc = 2u;
      v8::Local<v8::Value> argv[argc] = {Nan::New(fromIdx), Nan::New(toIdx)};

      auto cost = callback.Call(argc, argv);

      if (!cost->IsNumber())
        throw std::runtime_error{"Expected function signature: Number fn(Number from, Number to)"};

      matrix.at(fromIdx, toIdx) = Nan::To<std::int32_t>(cost).FromJust();
    }
  }

  return matrix;
}

// Caches user provided Function(node) -> [start, stop] into TimeWindows
inline auto makeTimeWindowsFromFunction(std::int32_t n, v8::Local<v8::Function> fn) {
  if (n < 0)
    throw std::runtime_error{"Negative size"};

  Nan::Callback callback{fn};

  TimeWindows timeWindows{n};

  for (std::int32_t atIdx = 0; atIdx < n; ++atIdx) {
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

    Interval out{Nan::To<std::int32_t>(start).FromJust(), Nan::To<std::int32_t>(stop).FromJust()};
    timeWindows.at(atIdx) = std::move(out);
  }

  return timeWindows;
}

// Caches user provided Function(vehicle) -> [node0, node1, ..] into RouteLocks
inline auto makeRouteLocksFromFunction(std::int32_t n, v8::Local<v8::Function> fn) {
  if (n < 0)
    throw std::runtime_error{"Negative size"};

  Nan::Callback callback{fn};

  // Note: use (n) for construction because RouteLocks is a weak alias to a std::vector.
  // Using vec(n) creates a vector of n items, using vec{n} creates a vector with a single element n.
  RouteLocks routeLocks(n);

  for (std::int32_t atIdx = 0; atIdx < n; ++atIdx) {
    const auto argc = 1u;
    v8::Local<v8::Value> argv[argc] = {Nan::New(atIdx)};

    auto locks = callback.Call(argc, argv);

    if (!locks->IsArray())
      throw std::runtime_error{"Expected function signature: Array fn(Number vehicle)"};

    auto locksArray = locks.As<v8::Array>();

    LockChain lockChain(locksArray->Length());

    for (std::int32_t lockIdx = 0; lockIdx < (std::int32_t)locksArray->Length(); ++lockIdx) {
      auto node = Nan::Get(locksArray, lockIdx).ToLocalChecked();

      if (!node->IsNumber())
        throw std::runtime_error{"Expected lock node of type Number"};

      lockChain.at(lockIdx) = Nan::To<std::int32_t>(node).FromJust();
    }

    routeLocks.at(atIdx) = std::move(lockChain);
  }

  return routeLocks;
}

// Caches user provided Js Array into a Vector
template <typename Vector> inline auto makeVectorFromJsNumberArray(v8::Local<v8::Array> array) {
  const std::int32_t len = array->Length();

  Vector vec(len);

  for (std::int32_t atIdx = 0; atIdx < len; ++atIdx) {
    auto num = Nan::Get(array, atIdx).ToLocalChecked();

    if (!num->IsNumber())
      throw std::runtime_error{"Expected array element of types Number"};
      //std::cout<<"hello"<<std::endl;
    vec.at(atIdx) = Nan::To<std::int32_t>(num).FromJust();
  }

  return vec;
}

// our function for changing js array into  vector of long long int

template <typename Vector> inline auto makeVectorFromJsNumberArray1(v8::Local<v8::Array> array) {
  const int64 len = array->Length();

  Vector vec(len);

  for (int64 atIdx = 0; atIdx < len; ++atIdx) {
    auto num = Nan::Get(array, atIdx).ToLocalChecked();

    if (!num->IsNumber())
      throw std::runtime_error{"Expected array element of types Number"};
      //std::cout<<"hello"<<std::endl;
    vec.at(atIdx) = Nan::To<int64_t>(num).FromJust();
  }

  return vec;
}

#endif
