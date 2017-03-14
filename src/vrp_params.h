#ifndef NODE_OR_TOOLS_VRP_PARAMS_0BED4C140464_H
#define NODE_OR_TOOLS_VRP_PARAMS_0BED4C140464_H

#include <nan.h>

#include <stdexcept>

struct VRPSolverParams {
  VRPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

  std::int32_t numNodes;

  v8::Local<v8::Function> costFunc;
  v8::Local<v8::Function> durationFunc;
  v8::Local<v8::Function> timeWindowFunc;
  v8::Local<v8::Function> demandFunc;
};

struct VRPSearchParams {
  VRPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

  std::int32_t computeTimeLimit;
  std::int32_t numVehicles;
  std::int32_t depotNode;
  std::int32_t timeHorizon;
  std::int32_t vehicleCapacity;

  v8::Local<v8::Function> callback;
};

// Impl.

VRPSolverParams::VRPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1 || !info[0]->IsObject())
    throw std::runtime_error{"Single object argument expected: SolverOptions"};

  auto opts = info[0].As<v8::Object>();

  auto maybeNumNodes = Nan::Get(opts, Nan::New("numNodes").ToLocalChecked());
  auto maybeCostFunc = Nan::Get(opts, Nan::New("costs").ToLocalChecked());
  auto maybeDurationFunc = Nan::Get(opts, Nan::New("durations").ToLocalChecked());
  auto maybeTimeWindowFunc = Nan::Get(opts, Nan::New("timeWindows").ToLocalChecked());
  auto maybeDemandFunc = Nan::Get(opts, Nan::New("demands").ToLocalChecked());

  auto numNodesOk = !maybeNumNodes.IsEmpty() && maybeNumNodes.ToLocalChecked()->IsNumber();
  auto costFuncOk = !maybeCostFunc.IsEmpty() && maybeCostFunc.ToLocalChecked()->IsFunction();
  auto durationFuncOk = !maybeDurationFunc.IsEmpty() && maybeDurationFunc.ToLocalChecked()->IsFunction();
  auto timeWindowFuncOk = !maybeTimeWindowFunc.IsEmpty() && maybeTimeWindowFunc.ToLocalChecked()->IsFunction();
  auto demandFuncOk = !maybeDemandFunc.IsEmpty() && maybeDemandFunc.ToLocalChecked()->IsFunction();

  if (!numNodesOk || !costFuncOk || !durationFuncOk || !timeWindowFuncOk || !demandFuncOk)
    throw std::runtime_error{"SolverOptions expects"
                             " 'numNodes' (Number),"
                             " 'costs' (Function),"
                             " 'durations' (Function),"
                             " 'timeWindows' (Function),"
                             " 'demands' (Function)"};

  numNodes = Nan::To<std::int32_t>(maybeNumNodes.ToLocalChecked()).FromJust();
  costFunc = maybeCostFunc.ToLocalChecked().As<v8::Function>();
  durationFunc = maybeDurationFunc.ToLocalChecked().As<v8::Function>();
  timeWindowFunc = maybeTimeWindowFunc.ToLocalChecked().As<v8::Function>();
  demandFunc = maybeDemandFunc.ToLocalChecked().As<v8::Function>();
}

VRPSearchParams::VRPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 2 || !info[0]->IsObject() || !info[1]->IsFunction())
    throw std::runtime_error{"Two arguments expected: SearchOptions (Object) and callback (Function)"};

  auto opts = info[0].As<v8::Object>();

  auto maybeComputeTimeLimit = Nan::Get(opts, Nan::New("computeTimeLimit").ToLocalChecked());
  auto maybeNumVehicles = Nan::Get(opts, Nan::New("numVehicles").ToLocalChecked());
  auto maybeDepotNode = Nan::Get(opts, Nan::New("depotNode").ToLocalChecked());
  auto maybeTimeHorizon = Nan::Get(opts, Nan::New("timeHorizon").ToLocalChecked());
  auto maybeVehicleCapacity = Nan::Get(opts, Nan::New("vehicleCapacity").ToLocalChecked());

  auto computeTimeLimitOk = !maybeComputeTimeLimit.IsEmpty() && maybeComputeTimeLimit.ToLocalChecked()->IsNumber();
  auto numVehiclesOk = !maybeNumVehicles.IsEmpty() && maybeNumVehicles.ToLocalChecked()->IsNumber();
  auto depotNodeOk = !maybeDepotNode.IsEmpty() && maybeDepotNode.ToLocalChecked()->IsNumber();
  auto timeHorizonOk = !maybeTimeHorizon.IsEmpty() && maybeTimeHorizon.ToLocalChecked()->IsNumber();
  auto vehicleCapacityOk = !maybeVehicleCapacity.IsEmpty() && maybeVehicleCapacity.ToLocalChecked()->IsNumber();

  if (!computeTimeLimitOk || !numVehiclesOk || !depotNodeOk || !timeHorizonOk || !vehicleCapacityOk)
    throw std::runtime_error{"SearchOptions expects"
                             " 'computeTimeLimit' (Number),"
                             " 'numVehicles' (Number),"
                             " 'depotNode' (Number),"
                             " 'timeHorizon' (Number),"
                             " 'vehicleCapacity' (Number)"};

  computeTimeLimit = Nan::To<std::int32_t>(maybeComputeTimeLimit.ToLocalChecked()).FromJust();
  numVehicles = Nan::To<std::int32_t>(maybeNumVehicles.ToLocalChecked()).FromJust();
  depotNode = Nan::To<std::int32_t>(maybeDepotNode.ToLocalChecked()).FromJust();
  timeHorizon = Nan::To<std::int32_t>(maybeTimeHorizon.ToLocalChecked()).FromJust();
  vehicleCapacity = Nan::To<std::int32_t>(maybeVehicleCapacity.ToLocalChecked()).FromJust();

  callback = info[1].As<v8::Function>();
}

#endif
