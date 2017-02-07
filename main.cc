#include "constraint_solver/routing.h"

#include "main.h"

namespace ort = operations_research;

NAN_MODULE_INIT(Solver::Init) {
  const auto whoami = Nan::New("Solver").ToLocalChecked();

  auto fnTp = Nan::New<v8::FunctionTemplate>(New);
  fnTp->SetClassName(whoami);
  fnTp->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(fnTp, "TSP", TSP);

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

NAN_METHOD(Solver::TSP) {
  auto* const self = Nan::ObjectWrap::Unwrap<Solver>(info.Holder());
  (void)self;

  // See routing_parameters.proto and routing_enums.proto
  auto modelParams = ort::RoutingModel::DefaultModelParameters();

  const auto numNodes = 10;
  const auto numVehicles = 1;
  const auto vehicleDepot = ort::RoutingModel::NodeIndex{0};

  ort::RoutingModel model{numNodes, numVehicles, vehicleDepot, modelParams};

  struct Cost {
    int64 operator()(ort::RoutingModel::NodeIndex from, ort::RoutingModel::NodeIndex to) const { return 10; }
  } const costs;

  const auto costEvaluator = NewPermanentCallback(&costs, &Cost::operator());
  model.SetArcCostEvaluatorOfAllVehicles(costEvaluator);

  // See routing_parameters.proto and routing_enums.proto
  auto searchParams = ort::RoutingModel::DefaultSearchParameters();

  // TODO: Make configurable from outside?
  auto firstSolutionStrategy = ort::FirstSolutionStrategy::AUTOMATIC;
  auto metaHeuristic = ort::LocalSearchMetaheuristic::AUTOMATIC;

  searchParams.set_first_solution_strategy(firstSolutionStrategy);
  searchParams.set_local_search_metaheuristic(metaHeuristic);
  searchParams.set_time_limit_ms(1000);

  // TODO: Nan::AsyncWorker
  const auto* solution = model.SolveWithParameters(searchParams);

  if (!solution)
    return;

  const auto cost = solution->ObjectiveValue();
  (void)cost; // TODO: use

  std::vector<std::vector<ort::RoutingModel::NodeIndex>> routes;
  model.AssignmentToRoutes(*solution, &routes);

  auto jsRoutes = Nan::New<v8::Array>(routes.size());

  for (int i = 0; i < routes.size(); ++i) {
    const auto& route = routes[i];

    auto jsNodes = Nan::New<v8::Array>(route.size());

    for (int j = 0; j < route.size(); ++j)
      (void)Nan::Set(jsNodes, j, Nan::New<v8::Number>(route[j].value()));

    (void)Nan::Set(jsRoutes, i, jsNodes);
  }

  info.GetReturnValue().Set(jsRoutes);
}

Nan::Persistent<v8::Function>& Solver::constructor() {
  static Nan::Persistent<v8::Function> init;
  return init;
}
