#include "main.h"

NAN_MODULE_INIT(Solver::Init) {
  const auto whoami = Nan::New("Solver").ToLocalChecked();

  auto fnTp = Nan::New<v8::FunctionTemplate>(New);
  fnTp->SetClassName(whoami);
  fnTp->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(fnTp, "fun", fun);

  const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
  constructor().Reset(fn);

  Nan::Set(target, whoami, fn);
}

NAN_METHOD(Solver::New) {
  if (info.IsConstructCall()) {
    auto* self = new Solver();
    (void)self;

    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    auto init = Nan::New(constructor());
    info.GetReturnValue().Set(init->NewInstance());
  }
}

NAN_METHOD(Solver::fun) {
  auto* const self = Nan::ObjectWrap::Unwrap<Solver>(info.Holder());
  (void)self;
}

Nan::Persistent<v8::Function>& Solver::constructor() {
  static Nan::Persistent<v8::Function> init;
  return init;
}
