#include "tsp.h"
#include "tsp_params.h"
#include "tsp_worker.h"

#include <cstddef>

#include <utility>
#include <vector>

TSP::TSP(CostMatrix costs_) : costs{std::make_shared<const CostMatrix>(std::move(costs_))} {}

NAN_MODULE_INIT(TSP::Init) {
  const auto whoami = Nan::New("TSP").ToLocalChecked();

  auto fnTp = Nan::New<v8::FunctionTemplate>(New);
  fnTp->SetClassName(whoami);
  fnTp->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(fnTp, "Solve", Solve);

  const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
  constructor().Reset(fn);

  Nan::Set(target, whoami, fn);
}

NAN_METHOD(TSP::New) try {
  // Handle `new T()` as well as `T()`
  if (!info.IsConstructCall()) {
    auto init = Nan::New(constructor());
    info.GetReturnValue().Set(init->NewInstance());
    return;
  }

  TSPSolverParams userParams{info};

  const auto bytesChange = getBytes(userParams.costs);
  Nan::AdjustExternalMemory(bytesChange);

  auto* self = new TSP{std::move(userParams.costs)};

  self->Wrap(info.This());

  info.GetReturnValue().Set(info.This());

} catch (const std::exception& e) {
  return Nan::ThrowError(e.what());
}

NAN_METHOD(TSP::Solve) try {
  auto* const self = Nan::ObjectWrap::Unwrap<TSP>(info.Holder());

  TSPSearchParams userParams{info};

  // See routing_parameters.proto and routing_enums.proto
  auto modelParams = RoutingModel::DefaultModelParameters();
  auto searchParams = RoutingModel::DefaultSearchParameters();

  auto firstSolutionStrategy = FirstSolutionStrategy::AUTOMATIC;
  auto metaHeuristic = LocalSearchMetaheuristic::AUTOMATIC;

  searchParams.set_first_solution_strategy(firstSolutionStrategy);
  searchParams.set_local_search_metaheuristic(metaHeuristic);
  searchParams.set_time_limit_ms(userParams.computeTimeLimit);

  const std::int32_t numNodes = self->costs->dim();
  const std::int32_t numVehicles = 1; // Always one for TSP

  auto worker = new TSPWorker{self->costs,                            //
                               new Nan::Callback{userParams.callback}, //
                               modelParams,                            //
                               searchParams,                           //
                               numNodes,                               //
                               numVehicles,                            //
                               userParams.depotNode};                  //

} catch (const std::exception& e) {
  return Nan::ThrowError(e.what());
}

Nan::Persistent<v8::Function>& TSP::constructor() {
  static Nan::Persistent<v8::Function> init;
  return init;
}
