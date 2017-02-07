#include "constraint_solver/routing.h"

#include "vrp.h"

namespace ort = operations_research;

NAN_MODULE_INIT(VRP::Init) {
  const auto whoami = Nan::New("VRP").ToLocalChecked();

  auto fnTp = Nan::New<v8::FunctionTemplate>(New);
  fnTp->SetClassName(whoami);
  fnTp->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(fnTp, "Solve", Solve);

  const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
  constructor().Reset(fn);

  Nan::Set(target, whoami, fn);
}

NAN_METHOD(VRP::New) {
  if (info.IsConstructCall()) {
    auto* self = new VRP();

    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    auto init = Nan::New(constructor());
    info.GetReturnValue().Set(init->NewInstance());
  }
}

NAN_METHOD(VRP::Solve) {
  auto* const self = Nan::ObjectWrap::Unwrap<VRP>(info.Holder());
  (void)self;

  // TODO: implement
}

Nan::Persistent<v8::Function>& VRP::constructor() {
  static Nan::Persistent<v8::Function> init;
  return init;
}
