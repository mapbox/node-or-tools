#include "constraint_solver/routing.h"

#include "tsp.h"

namespace ort = operations_research;

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

NAN_METHOD(TSP::New) {
  if (info.IsConstructCall()) {
    auto* self = new TSP();

    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    auto init = Nan::New(constructor());
    info.GetReturnValue().Set(init->NewInstance());
  }
}

NAN_METHOD(TSP::Solve) {
  if (info.Length() != 1 || !info[0]->IsObject())
    return Nan::ThrowTypeError("Single object argument expected: TSP options");

  auto opts = info[0].As<v8::Object>();

  auto maybeNumNodes = Nan::Get(opts, Nan::New("numNodes").ToLocalChecked());
  auto maybeCostFunc = Nan::Get(opts, Nan::New("costFunction").ToLocalChecked());

  auto numNodesOk = !maybeNumNodes.IsEmpty() && maybeNumNodes.ToLocalChecked()->IsNumber();
  auto costFuncOk = !maybeCostFunc.IsEmpty() && maybeCostFunc.ToLocalChecked()->IsFunction();

  if (!numNodesOk || !costFuncOk)
    return Nan::ThrowTypeError("TSP options expects 'numNodes' (Number) and 'CostFunc' (Function)");

  // TODO: overflow
  auto numNodes = Nan::To<int>(maybeNumNodes.ToLocalChecked()).FromJust();
  Nan::Callback costFunc(maybeCostFunc.ToLocalChecked().As<v8::Function>());

  // See routing_parameters.proto and routing_enums.proto
  auto modelParams = ort::RoutingModel::DefaultModelParameters();

  const auto numVehicles = 1;
  const auto vehicleDepot = ort::RoutingModel::NodeIndex{0};

  ort::RoutingModel model{numNodes, numVehicles, vehicleDepot, modelParams};

  struct Cost {
    int64 operator()(ort::RoutingModel::NodeIndex from, ort::RoutingModel::NodeIndex to) const {
      auto fromIdx = from.value();
      auto toIdx = to.value();

      const auto argc = 2;
      v8::Local<v8::Value> argv[argc] = {Nan::New(fromIdx), Nan::New(toIdx)};

      auto cost = callback.Call(argc, argv);

      // TODO: throw on !cost->IsNumber()

      // TODO: overflow
      return Nan::To<int>(cost).FromJust();
    }

    Nan::Callback& callback;
  } const costs{costFunc};

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

Nan::Persistent<v8::Function>& TSP::constructor() {
  static Nan::Persistent<v8::Function> init;
  return init;
}
