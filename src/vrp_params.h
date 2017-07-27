#ifndef NODE_OR_TOOLS_VRP_PARAMS_0BED4C140464_H
#define NODE_OR_TOOLS_VRP_PARAMS_0BED4C140464_H

#include <nan.h>

#include <stdexcept>

#include "params.h"

struct VRPSolverParams {
  VRPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

  std::int32_t numNodes;

  CostMatrix costs;
  DurationMatrix durations;
  TimeWindows timeWindows;
  DemandMatrix demands;
};

struct VRPSearchParams {
  VRPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

  std::int32_t computeTimeLimit;
  std::int32_t numVehicles;
  std::int32_t depotNode;
  std::int32_t timeHorizon;
  std::vector<int64> vehicleCapacity;   // type changed to vector 

  RouteLocks routeLocks;

  Pickups pickups;
  Deliveries deliveries;

  v8::Local<v8::Function> callback;
};

// Caches user provided 2d Array of [Number, Number] into Vectors of Intervals
inline auto makeTimeWindowsFrom2dArray(std::int32_t n, v8::Local<v8::Array> array) {
  if (n < 0)
    throw std::runtime_error{"Negative size"};

  if (static_cast<std::int32_t>(array->Length()) != n)
    throw std::runtime_error{"Array dimension do not match size"};

  TimeWindows timeWindows(n);

  for (std::int32_t atIdx = 0; atIdx < n; ++atIdx) {
    auto inner = Nan::Get(array, atIdx).ToLocalChecked();

    if (!inner->IsArray())
      throw std::runtime_error{"Expected Array of Arrays"};

    auto innerArray = inner.As<v8::Array>();

    if (static_cast<std::int32_t>(innerArray->Length()) != 2)
      throw std::runtime_error{"Expected interval Array of shape [start, stop]"};

    auto start = Nan::Get(innerArray, 0).ToLocalChecked();
    auto stop = Nan::Get(innerArray, 1).ToLocalChecked();

    if (!start->IsNumber() || !stop->IsNumber())
      throw std::runtime_error{"Expected interval start and stop of type Number"};

    auto startValue = Nan::To<std::int32_t>(start).FromJust();
    auto stopValue = Nan::To<std::int32_t>(stop).FromJust();

    Interval out{startValue, stopValue};

    timeWindows.at(atIdx) = std::move(out);
  }

  return timeWindows;
}

// Caches user provided 2d Array of [Number, ..] into Vectors
inline auto makeRouteLocksFrom2dArray(std::int32_t n, v8::Local<v8::Array> array) {
  if (n < 0)
    throw std::runtime_error{"Negative size"};

  if (static_cast<std::int32_t>(array->Length()) != n)
    throw std::runtime_error{"Array dimension do not match size"};

  // Note: use (n) for construction because RouteLocks is a weak alias to a std::vector.
  // Using vec(n) creates a vector of n items, using vec{n} creates a vector with a single element n.
  RouteLocks routeLocks(n);

  for (std::int32_t atIdx = 0; atIdx < n; ++atIdx) {
    auto inner = Nan::Get(array, atIdx).ToLocalChecked();

    if (!inner->IsArray())
      throw std::runtime_error{"Expected Array of Arrays"};

    auto innerArray = inner.As<v8::Array>();

    LockChain lockChain(innerArray->Length());

    for (std::int32_t lockIdx = 0; lockIdx < static_cast<std::int32_t>(innerArray->Length()); ++lockIdx) {
      auto node = Nan::Get(innerArray, lockIdx).ToLocalChecked();

      if (!node->IsNumber())
        throw std::runtime_error{"Expected lock node of type Number"};

      lockChain.at(lockIdx) = Nan::To<std::int32_t>(node).FromJust();
    }

    routeLocks.at(atIdx) = std::move(lockChain);
  }

  return routeLocks;
}

// Impl.

VRPSolverParams::VRPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1 || !info[0]->IsObject())
    throw std::runtime_error{"Single object argument expected: SolverOptions"};

  auto opts = info[0].As<v8::Object>();

  auto maybeNumNodes = Nan::Get(opts, Nan::New("numNodes").ToLocalChecked());
  auto maybeCostMatrix = Nan::Get(opts, Nan::New("costs").ToLocalChecked());
  auto maybeDurationMatrix = Nan::Get(opts, Nan::New("durations").ToLocalChecked());
  auto maybeTimeWindowsVector = Nan::Get(opts, Nan::New("timeWindows").ToLocalChecked());
  auto maybeDemandMatrix = Nan::Get(opts, Nan::New("demands").ToLocalChecked());

  auto numNodesOk = !maybeNumNodes.IsEmpty() && maybeNumNodes.ToLocalChecked()->IsNumber();
  auto costMatrixOk = !maybeCostMatrix.IsEmpty() && maybeCostMatrix.ToLocalChecked()->IsArray();
  auto durationMatrixOk = !maybeDurationMatrix.IsEmpty() && maybeDurationMatrix.ToLocalChecked()->IsArray();
  auto timeWindowsVectorOk = !maybeTimeWindowsVector.IsEmpty() && maybeTimeWindowsVector.ToLocalChecked()->IsArray();
  auto demandMatrixOk = !maybeDemandMatrix.IsEmpty() && maybeDemandMatrix.ToLocalChecked()->IsArray();


  if(!numNodesOk ) throw std::runtime_error{"ERROR TYPE 1"};
    else if(!costMatrixOk) throw std::runtime_error{"ERROR TYPE 2"};
      else if(!durationMatrixOk) throw std::runtime_error{"ERROR TYPE 3"};
        else if(!timeWindowsVectorOk) throw std::runtime_error{"ERROR TYPE 4"};
          else if(!demandMatrixOk) throw std::runtime_error{"ERROR TYPE 5"};
  
  if (!numNodesOk || !costMatrixOk || !durationMatrixOk || !timeWindowsVectorOk || !demandMatrixOk)
    throw std::runtime_error{"SolverOptions expects"
                             " 'numNodes' (Number),"
                             " 'costs' (Array),"
                             " 'durations' (Array),"
                             " 'timeWindows' (Array),"
                             " 'demands' (Array)"};

  numNodes = Nan::To<std::int32_t>(maybeNumNodes.ToLocalChecked()).FromJust();

  auto costMatrix = maybeCostMatrix.ToLocalChecked().As<v8::Array>();
  auto durationMatrix = maybeDurationMatrix.ToLocalChecked().As<v8::Array>();
  auto timeWindowsVector = maybeTimeWindowsVector.ToLocalChecked().As<v8::Array>();
  auto demandMatrix = maybeDemandMatrix.ToLocalChecked().As<v8::Array>();

  costs = makeMatrixFrom2dArray<CostMatrix>(numNodes, costMatrix);
  durations = makeMatrixFrom2dArray<DurationMatrix>(numNodes, durationMatrix);
  timeWindows = makeTimeWindowsFrom2dArray(numNodes, timeWindowsVector);
  demands = makeMatrixFrom2dArray<DemandMatrix>(numNodes, demandMatrix);
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
  auto maybeRouteLocks = Nan::Get(opts, Nan::New("routeLocks").ToLocalChecked());
  auto maybePickups = Nan::Get(opts, Nan::New("pickups").ToLocalChecked());
  auto maybeDeliveries = Nan::Get(opts, Nan::New("deliveries").ToLocalChecked());

  auto computeTimeLimitOk = !maybeComputeTimeLimit.IsEmpty() && maybeComputeTimeLimit.ToLocalChecked()->IsNumber();
  auto numVehiclesOk = !maybeNumVehicles.IsEmpty() && maybeNumVehicles.ToLocalChecked()->IsNumber();
  auto depotNodeOk = !maybeDepotNode.IsEmpty() && maybeDepotNode.ToLocalChecked()->IsNumber();
  auto timeHorizonOk = !maybeTimeHorizon.IsEmpty() && maybeTimeHorizon.ToLocalChecked()->IsNumber();
  auto vehicleCapacityOk = !maybeVehicleCapacity.IsEmpty() && maybeVehicleCapacity.ToLocalChecked()->IsArray();  // IsNumber() changed to IsArray()
  auto routeLocksOk = !maybeRouteLocks.IsEmpty() && maybeRouteLocks.ToLocalChecked()->IsArray();
  auto pickupsOk = !maybePickups.IsEmpty() && maybePickups.ToLocalChecked()->IsArray();
  auto deliveriesOk = !maybeDeliveries.IsEmpty() && maybeDeliveries.ToLocalChecked()->IsArray();

  // TODO: this is getting out of hand, clean up, or better think about generic parameter parsing
  if (!computeTimeLimitOk || !numVehiclesOk || !depotNodeOk || !timeHorizonOk || !vehicleCapacityOk || !routeLocksOk ||
      !pickupsOk || !deliveriesOk)
    throw std::runtime_error{"SearchOptions expects"
                             " 'computeTimeLimit' (Number),"
                             " 'numVehicles' (Number),"
                             " 'depotNode' (Number),"
                             " 'timeHorizon' (Number),"
                             " 'vehicleCapacity' (Number),"
                             " 'routeLocks' (Array),"
                             " 'pickups' (Array),"
                             " 'deliveries' (Array)"};

  computeTimeLimit = Nan::To<std::int32_t>(maybeComputeTimeLimit.ToLocalChecked()).FromJust();
  numVehicles = Nan::To<std::int32_t>(maybeNumVehicles.ToLocalChecked()).FromJust();
  depotNode = Nan::To<std::int32_t>(maybeDepotNode.ToLocalChecked()).FromJust();
  timeHorizon = Nan::To<std::int32_t>(maybeTimeHorizon.ToLocalChecked()).FromJust();
  //vehicleCapacity = Nan::To<std::int32_t>(maybeVehicleCapacity.ToLocalChecked()).FromJust();

  

  auto routeLocksArray = maybeRouteLocks.ToLocalChecked().As<v8::Array>();
  routeLocks = makeRouteLocksFrom2dArray(numVehicles, routeLocksArray);

  auto pickupsArray = maybePickups.ToLocalChecked().As<v8::Array>();
  pickups = makeVectorFromJsNumberArray<Pickups>(pickupsArray);

  auto deliveriesArray = maybeDeliveries.ToLocalChecked().As<v8::Array>();
  deliveries = makeVectorFromJsNumberArray<Deliveries>(deliveriesArray);

  auto vehicleCapacityArray = maybeVehicleCapacity.ToLocalChecked().As<v8::Array>();
  vehicleCapacity = makeVectorFromJsNumberArray1<std::vector<int64> >(vehicleCapacityArray);
  // new function call added to make vehicle capacity vector  calls  function in adapter.h

  callback = info[1].As<v8::Function>();
}

#endif
