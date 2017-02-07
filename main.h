#ifndef NODE_OR_TOOLS_4ECE4BE8F47A_H
#define NODE_OR_TOOLS_4ECE4BE8F47A_H

#include <memory>

#include <nan.h>

class Solver : public Nan::ObjectWrap {
public:
  static NAN_MODULE_INIT(Init);

private:
  static NAN_METHOD(New);

  static NAN_METHOD(TSP);

  static Nan::Persistent<v8::Function>& constructor();

  // Wrapped Object

  Solver() = default;

  // Non-Copyable
  Solver(const Solver&) = delete;
  Solver& operator=(const Solver&) = delete;

  // Non-Moveable
  Solver(Solver&&) = delete;
  Solver& operator=(Solver&&) = delete;
};

NODE_MODULE(node_or_tools, Solver::Init)

#endif
