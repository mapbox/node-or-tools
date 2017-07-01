#ifndef NODE_OR_TOOLS_TYPES_CD4622E664A7_H
#define NODE_OR_TOOLS_TYPES_CD4622E664A7_H

#include "ortools/constraint_solver/routing.h"

#include <cstdint>

#include "matrix.h"
#include "vector.h"

// Convenience types. Sshould be good enough for most of TSP / VRP related problems.

// Newtype via phantom type scheme. Allows type system to distinguish between tagged types.
// Note: NewType<T, struct Tag0>::Type != NewType<T, struct Tag1>::Type
template <typename T, typename Tag> struct NewType {
  struct Type : T {
    using T::T;
  };
};

using CostMatrix = NewType<Matrix<std::int32_t>, struct CostMatrixTag>::Type;
using DurationMatrix = NewType<Matrix<std::int32_t>, struct DurationMatrixTag>::Type;
using DemandMatrix = NewType<Matrix<std::int32_t>, struct DemandMatrixTag>::Type;

struct Interval {
  Interval() : start{0}, stop{0} {}

  Interval(std::int32_t start_, std::int32_t stop_) : start{start_}, stop{stop_} {
    if (start < 0 || stop < 0 || stop < start)
      throw std::runtime_error{"Negative intervals not supported"};
  }

  std::int32_t start;
  std::int32_t stop;
};

using TimeWindows = NewType<Vector<Vector<Interval>>, struct TimeWindowsTag>::Type;

namespace ort = operations_research;

// See routing.h
using RoutingModel = ort::RoutingModel;
using NodeIndex = ort::RoutingModel::NodeIndex;

// See routing_parameters.proto
using RoutingModelParameters = ort::RoutingModelParameters;
using RoutingSearchParameters = ort::RoutingSearchParameters;

// See routing_enums.proto
using FirstSolutionStrategy = ort::FirstSolutionStrategy;
using LocalSearchMetaheuristic = ort::LocalSearchMetaheuristic;

// See search_limit.proto
using SearchLimitParameters = ort::SearchLimitParameters;

// See constraint_solver.h
using Solver = ort::Solver;

// Locks: for locking (sub-) routes into place:
//  - locks[i] holds the lock chain for vehicle i (can be empty)
//  - lock chain is ordered list of node indices for vehicle i (must not contain depots)
using LockChain = std::vector<NodeIndex>;
using RouteLocks = std::vector<LockChain>;

// Pickup and Delivery constraints expressed as parallel arrays
// e.g. pickups: [1, 2], deliveries: [5, 6] means
//  - pick up at node 1 and deliver to node 5
//  - pick up at node 2 and deliver to node 6
using Pickups = NewType<Vector<NodeIndex>, struct PickupsTag>::Type;
using Deliveries = NewType<Vector<NodeIndex>, struct DeliveriesTag>::Type;

#endif
