#ifndef NODE_OR_TOOLS_TSP_WORKER_3C61C259EF2C_H
#define NODE_OR_TOOLS_TSP_WORKER_3C61C259EF2C_H

#include <nan.h>

#include "adaptors.h"
#include "types.h"

#include <memory>
#include <utility>
#include <vector>

struct TSPWorker final {

  TSPWorker(std::shared_ptr<const CostMatrix> costs_, Nan::Callback* callback, const RoutingModelParameters& modelParams_,
            const RoutingSearchParameters& searchParams_, std::int32_t numNodes, std::int32_t numVehicles,
            std::int32_t vehicleDepot)
      : mCallback(callback), costs{std::move(costs_)}, model{numNodes, numVehicles, NodeIndex{vehicleDepot}, modelParams_},
        modelParams{modelParams_}, searchParams{searchParams_}
    {
        Execute();
    }

  void Execute() {
    auto costAdaptor = makeBinaryAdaptor(*costs);
    auto costEvaluator = makeCallback(costAdaptor);

    model.SetArcCostEvaluatorOfAllVehicles(costEvaluator);

    const auto* assignment = model.SolveWithParameters(searchParams);

//    if (!assignment || (model.status() != RoutingModel::Status::ROUTING_SUCCESS))
//      SetErrorMessage("Unable to find a solution");

    model.AssignmentToRoutes(*assignment, &routes);

//    if (routes.size() != 1)
//      SetErrorMessage("Expected route for one vehicle");
      
    HandleOKCallback();
  }

  void HandleOKCallback() {
    Nan::HandleScope scope;

    const auto& route = routes.front();

    auto jsRoute = Nan::New<v8::Array>(route.size());

    for (std::size_t j = 0; j < route.size(); ++j)
      (void)Nan::Set(jsRoute, j, Nan::New<v8::Number>(route[j].value()));

    const auto argc = 2u;
    v8::Local<v8::Value> argv[argc] = {Nan::Null(), jsRoute};

    mCallback->Call(argc, argv);
  }

  std::shared_ptr<const CostMatrix> costs; // inc ref count to keep alive for async cb

  RoutingModel model;
  RoutingModelParameters modelParams;
  RoutingSearchParameters searchParams;

  Nan::Callback* mCallback;
  // Stores solution until we can translate back to v8 objects
  std::vector<std::vector<NodeIndex>> routes;
};

#endif
