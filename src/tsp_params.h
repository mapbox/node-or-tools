#ifndef NODE_OR_TOOLS_TSP_PARAMS_D4D3AF3A298F_H
#define NODE_OR_TOOLS_TSP_PARAMS_D4D3AF3A298F_H

#include <nan.h>

#include <stdexcept>

struct TSPSolverParams {
  TSPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

  int numNodes;

  v8::Local<v8::Function> costFunc;
};

struct TSPSearchParams {
  TSPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

  int computeTimeLimit;
  int depotNode;

  v8::Local<v8::Function> callback;
};

// Impl.

TSPSolverParams::TSPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1 || !info[0]->IsObject())
    throw std::runtime_error{"Single object argument expected: SolverOptions"};

  auto opts = info[0].As<v8::Object>();

  auto maybeNumNodes = Nan::Get(opts, Nan::New("numNodes").ToLocalChecked());
  auto maybeCostFunc = Nan::Get(opts, Nan::New("costs").ToLocalChecked());

  auto numNodesOk = !maybeNumNodes.IsEmpty() && maybeNumNodes.ToLocalChecked()->IsNumber();
  auto costFuncOk = !maybeCostFunc.IsEmpty() && maybeCostFunc.ToLocalChecked()->IsFunction();

  if (!numNodesOk || !costFuncOk)
    throw std::runtime_error{"SolverOptions expects 'numNodes' (Number), 'costs' (Function)"};

  // TODO: overflow
  numNodes = Nan::To<int>(maybeNumNodes.ToLocalChecked()).FromJust();
  costFunc = maybeCostFunc.ToLocalChecked().As<v8::Function>();
}

TSPSearchParams::TSPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 2 || !info[0]->IsObject() || !info[1]->IsFunction())
    throw std::runtime_error{"Two arguments expected: SearchOptions (Object) and callback (Function)"};

  auto opts = info[0].As<v8::Object>();

  auto maybeComputeTimeLimit = Nan::Get(opts, Nan::New("computeTimeLimit").ToLocalChecked());
  auto maybeDepotNode = Nan::Get(opts, Nan::New("depotNode").ToLocalChecked());

  auto computeTimeLimitOk = !maybeComputeTimeLimit.IsEmpty() && maybeComputeTimeLimit.ToLocalChecked()->IsNumber();
  auto depotNodeOk = !maybeDepotNode.IsEmpty() && maybeDepotNode.ToLocalChecked()->IsNumber();

  if (!computeTimeLimitOk || !depotNodeOk)
    throw std::runtime_error{"SearchOptions expects 'computeTimeLimit' (Number), 'depotNode' (Number)"};

  // TODO: overflow
  computeTimeLimit = Nan::To<int>(maybeComputeTimeLimit.ToLocalChecked()).FromJust();
  depotNode = Nan::To<int>(maybeDepotNode.ToLocalChecked()).FromJust();
  callback = info[1].As<v8::Function>();
}

#endif
