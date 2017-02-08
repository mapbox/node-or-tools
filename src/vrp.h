#ifndef NODE_OR_TOOLS_VRP_D6C3BEA8CF7E_H
#define NODE_OR_TOOLS_VRP_D6C3BEA8CF7E_H

#include <nan.h>

class VRP : public Nan::ObjectWrap {
public:
  static NAN_MODULE_INIT(Init);

private:
  static NAN_METHOD(New);

  static NAN_METHOD(Solve);

  static Nan::Persistent<v8::Function>& constructor();

  // Wrapped Object

  VRP() = default;

  // Non-Copyable
  VRP(const VRP&) = delete;
  VRP& operator=(const VRP&) = delete;

  // Non-Moveable
  VRP(VRP&&) = delete;
  VRP& operator=(VRP&&) = delete;
};

#endif
