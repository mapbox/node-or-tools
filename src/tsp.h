#ifndef NODE_OR_TOOLS_TSP_17907AD93A08_H
#define NODE_OR_TOOLS_TSP_17907AD93A08_H

#include <nan.h>

#include "matrix.h"

#include <memory>

class TSP : public Nan::ObjectWrap {
public:
  static NAN_MODULE_INIT(Init);

  using CostMatrix = Matrix<std::int64_t>;

private:
  static NAN_METHOD(New);

  static NAN_METHOD(Solve);

  static Nan::Persistent<v8::Function>& constructor();

  // Wrapped Object

  TSP(CostMatrix costs);

  std::shared_ptr<const CostMatrix> costs;
};

#endif
