#include "constraint_solver/routing.h"

#include "tsp.h"

#include <cstddef>

#include <utility>
#include <vector>

namespace ort = operations_research;

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

NAN_METHOD(TSP::New) {
  // Handle `new T()` as well as `T()`
  if (!info.IsConstructCall()) {
    auto init = Nan::New(constructor());
    info.GetReturnValue().Set(init->NewInstance());
    return;
  }

  if (info.Length() != 1 || !info[0]->IsObject())
    return Nan::ThrowTypeError("Single object argument expected: SolverOptions");

  auto opts = info[0].As<v8::Object>();

  auto maybeNumNodes = Nan::Get(opts, Nan::New("numNodes").ToLocalChecked());
  auto maybeCostFunc = Nan::Get(opts, Nan::New("costFunction").ToLocalChecked());

  auto numNodesOk = !maybeNumNodes.IsEmpty() && maybeNumNodes.ToLocalChecked()->IsNumber();
  auto costFuncOk = !maybeCostFunc.IsEmpty() && maybeCostFunc.ToLocalChecked()->IsFunction();

  if (!numNodesOk || !costFuncOk)
    return Nan::ThrowTypeError("SolverOptions expects 'numNodes' (Number), 'costFunc' (Function)");

  // TODO: overflow
  auto numNodes = Nan::To<int>(maybeNumNodes.ToLocalChecked()).FromJust();
  Nan::Callback costFunc(maybeCostFunc.ToLocalChecked().As<v8::Function>());

  CostMatrix costs{numNodes};

  for (int fromIdx = 0; fromIdx < numNodes; ++fromIdx) {
    for (int toIdx = 0; toIdx < numNodes; ++toIdx) {
      const auto argc = 2u;
      v8::Local<v8::Value> argv[argc] = {Nan::New(fromIdx), Nan::New(toIdx)};

      auto cost = costFunc.Call(argc, argv);

      if (!cost->IsNumber())
        return Nan::ThrowTypeError("Expected function signature: Number costFunc(Number from, Number to)");

      costs(fromIdx, toIdx) = Nan::To<int>(cost).FromJust();
    }
  }

  auto* self = new TSP(std::move(costs));

  self->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(TSP::Solve) {
  auto* const self = Nan::ObjectWrap::Unwrap<TSP>(info.Holder());

  if (info.Length() != 2 || !info[0]->IsObject() || !info[1]->IsFunction())
    return Nan::ThrowTypeError("Two arguments expected: SearchOptions (Object) and callback (Function)");

  auto opts = info[0].As<v8::Object>();
  auto callback = info[1].As<v8::Function>();

  auto maybeTimeLimit = Nan::Get(opts, Nan::New("timeLimit").ToLocalChecked());
  auto maybeDepotNode = Nan::Get(opts, Nan::New("depotNode").ToLocalChecked());

  auto timeLimitOk = !maybeTimeLimit.IsEmpty() && maybeTimeLimit.ToLocalChecked()->IsNumber();
  auto depotNodeOk = !maybeDepotNode.IsEmpty() && maybeDepotNode.ToLocalChecked()->IsNumber();

  if (!timeLimitOk || !depotNodeOk)
    return Nan::ThrowTypeError("SearchOptions expects 'timeLimit' (Number), 'depotNode' (Number)");

  // TODO: put in its own file
  struct Worker final : Nan::AsyncWorker {
    using Base = Nan::AsyncWorker;

    Worker(std::shared_ptr<const CostMatrix> costs_, Nan::Callback* callback, int numNodes, int numVehicles, int vehicleDepot,
           const ort::RoutingModelParameters& modelParams_, const ort::RoutingSearchParameters& searchParams_)
        : Base(callback), costs{std::move(costs_)},
          model(numNodes, numVehicles, ort::RoutingModel::NodeIndex{vehicleDepot}, modelParams_), modelParams(modelParams_),
          searchParams(searchParams_), routes{} {}

    void Execute() override {
      struct CostMatrixAdapter {
        int64 operator()(ort::RoutingModel::NodeIndex from, ort::RoutingModel::NodeIndex to) const {
          return costMatrix(from.value(), to.value());
        }

        const CostMatrix& costMatrix;
      } const costAdapter{*costs};

      const auto costEvaluator = NewPermanentCallback(&costAdapter, &CostMatrixAdapter::operator());
      model.SetArcCostEvaluatorOfAllVehicles(costEvaluator);

      const auto* solution = model.SolveWithParameters(searchParams);

      if (!solution)
        SetErrorMessage("Unable to find a solution");

      const auto cost = solution->ObjectiveValue();
      (void)cost; // TODO: use

      model.AssignmentToRoutes(*solution, &routes);
    }

    void HandleOKCallback() override {
      Nan::HandleScope scope;

      auto jsRoutes = Nan::New<v8::Array>(routes.size());

      for (std::size_t i = 0; i < routes.size(); ++i) {
        const auto& route = routes[i];

        auto jsNodes = Nan::New<v8::Array>(route.size());

        for (std::size_t j = 0; j < route.size(); ++j)
          (void)Nan::Set(jsNodes, j, Nan::New<v8::Number>(route[j].value()));

        (void)Nan::Set(jsRoutes, i, jsNodes);
      }

      const auto argc = 2u;
      v8::Local<v8::Value> argv[argc] = {Nan::Null(), jsRoutes};

      callback->Call(argc, argv);
    }

    std::shared_ptr<const CostMatrix> costs; // inc ref count to keep alive for async cb

    ort::RoutingModel model;
    ort::RoutingModelParameters modelParams;
    ort::RoutingSearchParameters searchParams;

    std::vector<std::vector<ort::RoutingModel::NodeIndex>> routes;
  };

  const auto numNodes = self->costs->dim();
  const auto numVehicles = 1; // Always one for TSP

  // TODO: overflow
  auto timeLimit = Nan::To<int>(maybeTimeLimit.ToLocalChecked()).FromJust();
  auto depotNode = Nan::To<int>(maybeDepotNode.ToLocalChecked()).FromJust();

  // See routing_parameters.proto and routing_enums.proto
  auto modelParams = ort::RoutingModel::DefaultModelParameters();
  auto searchParams = ort::RoutingModel::DefaultSearchParameters();

  // TODO: Make configurable from Js?
  auto firstSolutionStrategy = ort::FirstSolutionStrategy::AUTOMATIC;
  auto metaHeuristic = ort::LocalSearchMetaheuristic::AUTOMATIC;

  searchParams.set_first_solution_strategy(firstSolutionStrategy);
  searchParams.set_local_search_metaheuristic(metaHeuristic);
  searchParams.set_time_limit_ms(timeLimit);

  auto* worker = new Worker{self->costs,                 //
                            new Nan::Callback{callback}, //
                            numNodes,                    //
                            numVehicles,                 //
                            depotNode,                   //
                            modelParams,                 //
                            searchParams};               //

  Nan::AsyncQueueWorker(worker);
}

Nan::Persistent<v8::Function>& TSP::constructor() {
  static Nan::Persistent<v8::Function> init;
  return init;
}
