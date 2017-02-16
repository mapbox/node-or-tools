#ifndef NODE_OR_TOOLS_VRP_WORKER_C6DF0F45B324_H
#define NODE_OR_TOOLS_VRP_WORKER_C6DF0F45B324_H

#include <nan.h>

#include "adaptors.h"
#include "types.h"

#include <memory>
#include <utility>
#include <vector>

struct VRPWorker final : Nan::AsyncWorker {
  using Base = Nan::AsyncWorker;

  VRPWorker(std::shared_ptr<const CostMatrix> costs_,         //
            std::shared_ptr<const DurationMatrix> durations_, //
            std::shared_ptr<const TimeWindows> timeWindows_,  //
            std::shared_ptr<const DemandMatrix> demands_,     //
            Nan::Callback* callback,                          //
            const RoutingModelParameters& modelParams_,       //
            const RoutingSearchParameters& searchParams_,     //
            int numNodes,                                     //
            int numVehicles,                                  //
            int vehicleDepot,                                 //
            int timeHorizon_,                                 //
            int vehicleCapacity_)                             //
      : Base(callback),
        // Cached vectors and matrices
        costs{std::move(costs_)},
        durations{std::move(durations_)},
        timeWindows{std::move(timeWindows_)},
        demands{std::move(demands_)},
        // Search settings
        timeHorizon{timeHorizon_},
        vehicleCapacity{vehicleCapacity_},
        // Setup model
        model{numNodes, numVehicles, NodeIndex{vehicleDepot}, modelParams_},
        modelParams{modelParams_},
        searchParams{searchParams_} {

    const auto costsOk = costs->dim() == numNodes;
    const auto durationsOk = durations->dim() == numNodes;
    const auto timeWindowsOk = timeWindows->size() == numNodes;
    const auto demandsOk = demands->dim() == numNodes;

    if (!costsOk || !durationsOk || !timeWindowsOk || !demandsOk)
      throw std::runtime_error{"Expected costs, durations, timeWindow and demand sizes to match numNodes"};
  }

  void Execute() override {
    auto costAdaptor = makeBinaryAdaptor(*costs);
    auto costCallback = makeCallback(costAdaptor);

    model.SetArcCostEvaluatorOfAllVehicles(costCallback);

    // Time Dimension

    auto durationAdaptor = makeBinaryAdaptor(*durations);
    auto durationCallback = makeCallback(durationAdaptor);

    const static auto kDimensionTime = "time";

    model.AddDimension(durationCallback, timeHorizon, timeHorizon, /*fix_start_cumul_to_zero=*/true, kDimensionTime);
    const auto& timeDimension = model.GetDimensionOrDie(kDimensionTime);

    const auto numNodes = timeWindows->size();

    for (auto node = 0; node < numNodes; ++node) {
      const auto interval = timeWindows->at(node);
      timeDimension.CumulVar(node)->SetRange(interval.start, interval.stop);
      // At the moment we only support a single interval for time windows.
      // We can support multiple intervals if we sort intervals by start then stop.
      // Then Cumulval(n)->SetRange(minStart, maxStop), then walk over intervals and
      // remove intervals between active intervals: CumulVar(n)->RemoveInterval(stop, start).
    }

    // Capacity Dimension

    auto demandAdaptor = makeBinaryAdaptor(*demands);
    auto demandCallback = makeCallback(demandAdaptor);

    const static auto kDimensionCapacity = "capacity";

    model.AddDimension(demandCallback, /*slack=*/0, vehicleCapacity, /*fix_start_cumul_to_zero=*/true, kDimensionCapacity);
    // const auto& capacityDimension = model.GetDimensionOrDie(kDimensionCapacity);

    model.CloseModel();

    const auto* assignment = model.SolveWithParameters(searchParams);

    if (!assignment || (model.status() != RoutingModel::Status::ROUTING_SUCCESS))
      return SetErrorMessage("Unable to find a solution");

    // const auto cost = assignment->ObjectiveValue();

    model.AssignmentToRoutes(*assignment, &routes);
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

  // Shared ownership: keeps objects alive until the last callback is done.
  std::shared_ptr<const CostMatrix> costs;
  std::shared_ptr<const DurationMatrix> durations;
  std::shared_ptr<const TimeWindows> timeWindows;
  std::shared_ptr<const DemandMatrix> demands;

  int timeHorizon;
  int vehicleCapacity;

  RoutingModel model;
  RoutingModelParameters modelParams;
  RoutingSearchParameters searchParams;

  // Stores solution until we can translate back to v8 objects
  std::vector<std::vector<NodeIndex>> routes;
};

#endif
