#include "constraint_solver/routing.h"

#include "main.h"

NAN_MODULE_INIT(Solver::Init) {
  const auto whoami = Nan::New("Solver").ToLocalChecked();

  auto fnTp = Nan::New<v8::FunctionTemplate>(New);
  fnTp->SetClassName(whoami);
  fnTp->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(fnTp, "tsp", tsp);

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

NAN_METHOD(Solver::tsp) {
  auto* const self = Nan::ObjectWrap::Unwrap<Solver>(info.Holder());
  (void)self;

  using Model = operations_research::RoutingModel;

  // See routing_parameters.proto and routing_enums.proto
  auto modelParams = Model::DefaultModelParameters();

  const auto numNodes = 10;
  const auto numVehicles = 1;
  const auto vehicleDepot = Model::NodeIndex{0};

  Model model{numNodes, numVehicles, vehicleDepot, modelParams};

  struct Cost {
    int64 operator()(Model::NodeIndex from, Model::NodeIndex to) const { return 10; }
  } const costs;

  const auto costEvaluator = NewPermanentCallback(&costs, &Cost::operator());
  model.SetArcCostEvaluatorOfAllVehicles(costEvaluator);

  // See routing_parameters.proto and routing_enums.proto
  auto searchParams = Model::DefaultSearchParameters();
  searchParams.set_time_limit_ms(1000);

  // TODO: Nan::AsyncWorker
  const auto* solution = model.SolveWithParameters(searchParams);

  if (!solution)
    return;

  const auto cost = solution->ObjectiveValue();
  (void)cost; // TODO: use

  std::vector<std::vector<Model::NodeIndex>> routes;
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
