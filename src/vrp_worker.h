#ifndef NODE_OR_TOOLS_VRP_WORKER_C6DF0F45B324_H
#define NODE_OR_TOOLS_VRP_WORKER_C6DF0F45B324_H

#include <nan.h>

#include "adaptors.h"
#include "types.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

struct RoutingSolution {
  std::int64_t cost;
  std::vector<std::vector<NodeIndex>> routes;
  std::vector<std::vector<Interval>> times;
};

struct VRPWorker final : Nan::AsyncWorker {
  using Base = Nan::AsyncWorker;

  VRPWorker(std::shared_ptr<const CostMatrix> costs_,         //
            std::shared_ptr<const DurationMatrix> durations_, //
            std::shared_ptr<const TimeWindows> timeWindows_,  //
            std::shared_ptr<const DemandMatrix> demands_,     //
            Nan::Callback* callback,                          //
            const RoutingModelParameters& modelParams_,       //
            const RoutingSearchParameters& searchParams_,     //
            std::int32_t numNodes_,                           //
            std::int32_t numVehicles_,                        //
            std::int32_t vehicleDepot_,                       //
            std::int32_t timeHorizon_,                        //
            std::vector<int64> vehicleCapacity_,              //   type changed to vector int64
            RouteLocks routeLocks_,                           //
            Pickups pickups_,                                 //
            Deliveries deliveries_)                           //
      : Base(callback),
        // Cached vectors and matrices
        costs{std::move(costs_)},
        durations{std::move(durations_)},
        timeWindows{std::move(timeWindows_)},
        demands{std::move(demands_)},
        // Search settings
        numNodes{numNodes_},
        numVehicles{numVehicles_},
        vehicleDepot{vehicleDepot_},
        timeHorizon{timeHorizon_},
        vehicleCapacity{std::move(vehicleCapacity_)},
        routeLocks{std::move(routeLocks_)},
        pickups{std::move(pickups_)},
        deliveries{std::move(deliveries_)},
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

    const auto routeLocksOk = (std::int32_t)routeLocks.size() == numVehicles;

    if (!routeLocksOk)
      throw std::runtime_error{"Expected routeLocks size to match numVehicles"};

    for (const auto& locks : routeLocks) {
      for (const auto& node : locks) {
        const auto nodeInBounds = node >= 0 && node < numNodes;

        if (!nodeInBounds)
          throw std::runtime_error{"Expected nodes in route locks to be in [0, numNodes - 1]"};

        const auto nodeIsDepot = node == vehicleDepot;

        if (nodeIsDepot)
          throw std::runtime_error{"Expected depot not to be in route locks"};
      }
    }

    const auto pickupsAndDeliveriesOk = pickups.size() == deliveries.size();

    if (!pickupsAndDeliveriesOk)
      throw std::runtime_error{"Expected pickups and deliveries parallel array sizes to match"};
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

    for (std::int32_t node = 0; node < numNodes; ++node) {
      const auto interval = timeWindows->at(node);
      timeDimension.CumulVar(node)->SetRange(interval.start, interval.stop);
      // At the moment we only support a single interval for time windows.
      // We can support multiple intervals if we sort intervals by start then stop.
      // Then Cumulval(n)->SetRange(minStart, maxStop), then walk over intervals
      // removing intervals between active intervals:
      // CumulVar(n)->RemoveInterval(stop, start).
    }

    // Capacity Dimension

    auto demandAdaptor = makeBinaryAdaptor(*demands);
    auto demandCallback = makeCallback(demandAdaptor);

 
    //std::vector<int64> vehicle_capacities
    // vehicle capacity call back  

  //auto vehicleCapacityAdaptor = makeUnaryAdaptor(vehicleCapacity);
  //  auto vehicleCapacityCallback = makeCallback(vehicleCapacityAdaptor);

// if(model.status() != RoutingModel::Status::ROUTING_SUCCESS)
//       return SetErrorMessage("Unable to find a solution -2");


    const static std::string& kDimensionCapacity = "capacity";

    //function for handling different capacitated vehicles
    model.AddDimensionWithVehicleCapacity(demandCallback, /*slack=*/0, vehicleCapacity, /*fix_start_cumul_to_zero=*/true, kDimensionCapacity);
     const auto& capacityDimension = model.GetDimensionOrDie(kDimensionCapacity);

     // if(model.status() != RoutingModel::Status::ROUTING_SUCCESS)
     //  return SetErrorMessage("Unable to find a solution -1");

     std::cout<<"hello ";
    //old  
    //const static auto kDimensionCapacity = "capacity";

    //model.AddDimension(demandCallback, /*slack=*/0, vehicleCapacity, /*fix_start_cumul_to_zero=*/true, kDimensionCapacity);
    // const auto& capacityDimension = model.GetDimensionOrDie(kDimensionCapacity);



    // Pickup and Deliveries

    auto* solver = model.solver();
    // if(model.status() != RoutingModel::Status::ROUTING_SUCCESS)
    //   return SetErrorMessage("Unable to find a solution 0");

    for (std::int32_t atIdx = 0; atIdx < pickups.size(); ++atIdx) {
      const auto pickupIndex = model.NodeToIndex(pickups.at(atIdx));
      const auto deliveryIndex = model.NodeToIndex(deliveries.at(atIdx));

      auto* sameRouteCt = solver->MakeEquality(model.VehicleVar(pickupIndex),    //
                                               model.VehicleVar(deliveryIndex)); //

      auto* pickupBeforeDeliveryCt = solver->MakeLessOrEqual(timeDimension.CumulVar(pickupIndex),    //
                                                             timeDimension.CumulVar(deliveryIndex)); //

      solver->AddConstraint(sameRouteCt);
      solver->AddConstraint(pickupBeforeDeliveryCt);

      model.AddPickupAndDelivery(pickups.at(atIdx), deliveries.at(atIdx));
    }
    // if(model.status() != RoutingModel::Status::ROUTING_SUCCESS)
    //   return SetErrorMessage("Unable to find a solution 0");

    // Done with modifications to the routing model

    model.CloseModel();

    // if(model.status() != RoutingModel::Status::ROUTING_SUCCESS)
    //   return SetErrorMessage("Unable to find a solution 1");

    // Locking routes into place needs to happen after the model is closed and the underlying vars are established
    const auto validLocks = model.ApplyLocksToAllVehicles(routeLocks, /*close_routes=*/false);

    // if(model.status() != RoutingModel::Status::ROUTING_SUCCESS)
    //   return SetErrorMessage("Unable to find a solution 2");

    if (!validLocks)
      return SetErrorMessage("Invalid locks");

    const auto* assignment = model.SolveWithParameters(searchParams);

    if (!assignment || (model.status() != RoutingModel::Status::ROUTING_SUCCESS))
      return SetErrorMessage("Unable to find a solution");

    const auto cost = static_cast<std::int64_t>(assignment->ObjectiveValue());

    std::vector<std::vector<NodeIndex>> routes;
    model.AssignmentToRoutes(*assignment, &routes);

    std::vector<std::vector<Interval>> times;

    for (const auto& route : routes) {
      std::vector<Interval> routeTimes;

      for (const auto& node : route) {
        const auto index = model.NodeToIndex(node);

        const auto* timeVar = timeDimension.CumulVar(index);

        const auto first = static_cast<std::int32_t>(assignment->Min(timeVar));
        const auto last = static_cast<std::int32_t>(assignment->Max(timeVar));

        routeTimes.push_back(Interval{first, last});
      }

      times.push_back(std::move(routeTimes));
    }

    solution = RoutingSolution{cost, std::move(routes), std::move(times)};
  }

  void HandleOKCallback() override {
    Nan::HandleScope scope;

    auto jsSolution = Nan::New<v8::Object>();

    auto jsCost = Nan::New<v8::Number>(solution.cost);
    auto jsRoutes = Nan::New<v8::Array>(solution.routes.size());
    auto jsTimes = Nan::New<v8::Array>(solution.times.size());

    for (std::size_t i = 0; i < solution.routes.size(); ++i) {
      const auto& route = solution.routes[i];
      const auto& times = solution.times[i];

      auto jsNodes = Nan::New<v8::Array>(route.size());
      auto jsNodeTimes = Nan::New<v8::Array>(times.size());

      for (std::size_t j = 0; j < route.size(); ++j) {
        Nan::Set(jsNodes, j, Nan::New<v8::Number>(route[j].value()));

        auto jsInterval = Nan::New<v8::Array>(2);

        Nan::Set(jsInterval, 0, Nan::New<v8::Number>(times[j].start));
        Nan::Set(jsInterval, 1, Nan::New<v8::Number>(times[j].stop));

        Nan::Set(jsNodeTimes, j, jsInterval);
      }

      Nan::Set(jsRoutes, i, jsNodes);
      Nan::Set(jsTimes, i, jsNodeTimes);
    }

    Nan::Set(jsSolution, Nan::New("cost").ToLocalChecked(), jsCost);
    Nan::Set(jsSolution, Nan::New("routes").ToLocalChecked(), jsRoutes);
    Nan::Set(jsSolution, Nan::New("times").ToLocalChecked(), jsTimes);

    const auto argc = 2u;
    v8::Local<v8::Value> argv[argc] = {Nan::Null(), jsSolution};

    callback->Call(argc, argv);
  }

  // Shared ownership: keeps objects alive until the last callback is done.
  std::shared_ptr<const CostMatrix> costs;
  std::shared_ptr<const DurationMatrix> durations;
  std::shared_ptr<const TimeWindows> timeWindows;
  std::shared_ptr<const DemandMatrix> demands;

  std::int32_t numNodes;
  std::int32_t numVehicles;
  std::int32_t vehicleDepot;
  std::int32_t timeHorizon;
  std::vector<int64> vehicleCapacity;   // changed type of vehicle capacity from int32_t to vector<int64>

  const RouteLocks routeLocks;

  const Pickups pickups;
  const Deliveries deliveries;

  RoutingModel model;
  RoutingModelParameters modelParams;
  RoutingSearchParameters searchParams;

  // Stores solution until we can translate back to v8 objects
  RoutingSolution solution;
};

#endif
