#ifndef NODE_OR_TOOLS_TSP_17907AD93A08_H
#define NODE_OR_TOOLS_TSP_17907AD93A08_H

#include <nan.h>

class TSP : public Nan::ObjectWrap {
public:
  static NAN_MODULE_INIT(Init);

private:
  static NAN_METHOD(New);

  static NAN_METHOD(Solve);

  static Nan::Persistent<v8::Function>& constructor();

  // Wrapped Object

  TSP() = default;

  // Non-Copyable
  TSP(const TSP&) = delete;
  TSP& operator=(const TSP&) = delete;

  // Non-Moveable
  TSP(TSP&&) = delete;
  TSP& operator=(TSP&&) = delete;
};

#endif
