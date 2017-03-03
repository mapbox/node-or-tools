#ifndef NODE_OR_TOOLS_TYPES_CD4622E664A7_H
#define NODE_OR_TOOLS_TYPES_CD4622E664A7_H

#include "constraint_solver/routing.h"

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

using CostMatrix = NewType<Matrix<std::int64_t>, struct CostMatrixTag>::Type;
using DurationMatrix = NewType<Matrix<std::int64_t>, struct DurationMatrixTag>::Type;
using DemandMatrix = NewType<Matrix<std::int64_t>, struct DemandMatrixTag>::Type;

struct Interval {
  Interval() : start{0}, stop{0} {}

  Interval(std::int64_t start_, std::int64_t stop_) : start{start_}, stop{stop_} {
    if (start < 0 || stop < 0 || stop < start)
      throw std::runtime_error{"Negative intervals not supported"};
  }

  std::int64_t start;
  std::int64_t stop;
};

using TimeWindows = NewType<Vector<Interval>, struct TimeWindowsTag>::Type;

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

#endif
