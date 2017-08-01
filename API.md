# Table of Contents
- [Travelling Salesman Problem (TSP)](#tsp)
- [Vehicle Routing Problem (VRP)](#vrp)


# TSP

Heuristically solves the Travelling Salesman Problem (TSP) with a single vehicle.
For multiple vehicles or additional constraints see the [Vehicle Routing Problem (VRP)](#vrp).
The TSP solver is split into two: the `TSP` constructor taking constructor-specific options and the asynchronous `Solve` function taking search-specific options.

Note: even though the bindings take JavaScript **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** types internally the solver works with integral types.
Make sure to convert your floating point types into integral types, otherwise truncation will happen transparently.


## Constructor

Constructs a TSP solver object.
Initializes and caches user data internally for efficiency.


**Parameters**

**[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)** with solver-specific options:
- `numNodes` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** Number of locations in the problem ("nodes").
- `costs` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** Cost array the solver minimizes in optimization. Can for example be duration, distance but does not have to be. Two-dimensional with `costs[from][to]` being a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the cost for traversing the arc from `from` to `to`.


**Examples**

```javascript
var costs = [[0, 10, 10],
             [10, 0, 10],
             [10, 10, 0]];

var tspSolverOpts = {
  numNodes: 3,
  costs: costs
};

var TSP = new node_or_tools.TSP(tspSolverOpts);
```


## Solve

Runs the TSP solver asynchronously to search for a solution.


**Parameters**

- `computeTimeLimit` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** Time limit in milliseconds for the solver. In general the longer you run the solver the better the solution (if there is any) will be. The solver will never run longer than this time limit but can finish earlier.
- `depotNode` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The depot node index in the range `[0, numNodes - 1]` where all vehicles start and end at.


**Examples**

```javascript
var tspSearchOpts = {
  computeTimeLimit: 1000,
  depotNode: depotNode
};

TSP.Solve(tspSearchOpts, function (err, solution) {
  if (err) return console.log(err);
  console.log(util.inspect(solution, {showHidden: false, depth: null}));
});
```

**Result**

**[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** with **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** indices into the locations for the vehicle to visit in order.

**Examples**

```javascript
[ 4, 8, 12, 13, 14, 15, 11, 10, 9, 5, 6, 7, 3, 2, 1 ]
```


# VRP

Heuristically solves the Vehicle Routing Problem (VRP) with multiple vehicles and constraints (time windows, capacities and more).
The VRP solver is split into two: the `VRP` constructor taking constructor-specific options and the asynchronous `Solve` function taking search-specific options.

Note: even though the bindings take JavaScript **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** types internally the solver works with integral types.
Make sure to convert your floating point types into integral types, otherwise truncation will happen transparently.

## Constructor

Constructs a VRP solver object.
Initializes and caches user data internally for efficiency.


**Parameters**

**[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)** with solver-specific options:
- `numNodes` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** Number of locations in the problem ("nodes").
- `costs` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** Cost array the solver minimizes in optimization. Can for example be duration, distance but does not have to be. Two-dimensional with `costs[from][to]` being a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the cost for traversing the arc from `from` to `to`.
- `durations` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** Duration array the solver uses for time constraints. Two-dimensional with `durations[from][to]` being a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the duration for servicing node `from` plus the time for traversing the arc from `from` to `to`.
- `timeWindows` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** Time window array the solver uses for time constraints. Two-dimensional with `timeWindows[at]` being an **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** of two **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the start and end time point of the time window when servicing the node `at` is allowed. The solver starts from time point `0` (you can think of this as the start of the work day) and the time points need to be positive offsets to this time point.
- `demands` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** Demands array the solver uses for vehicle capacity constraints. Two-dimensional with `demands[from][to]` being a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the demand at node `from`, for example number of packages to deliver to this location. The `to` node index is unused and reserved for future changes; set `demands[at]` to a constant array for now. The depot should have a demand of zero.


**Examples**

```javascript
var vrpSolverOpts = {
  numNodes: 3,
  costs: [[0, 10, 10], [10, 0, 10], [10, 10, 0]],
  durations: [[0, 2, 2], [2, 0, 2], [2, 2, 0]],
  timeWindows: [[0, 9], [2, 3], [2, 3]],
  demands: [[0, 0, 0], [1, 1, 1], [1, 1, 1]]
};

var VRP = new node_or_tools.VRP(vrpSolverOpts);
```


## Solve

Runs the VRP solver asynchronously to search for a solution.


**Parameters**

- `computeTimeLimit` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** Time limit in milliseconds for the solver. In general the longer you run the solver the better the solution (if there is any) will be. The solver will never run longer than this time limit but can finish earlier.
- `numVehicles` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The number of vehicles for servicing nodes.
- `depotNode` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The depot node index in the range `[0, numNodes - 1]` where all vehicles start and end at.
- `timeHorizon` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The last time point the solver uses for time constraints. The solver starts from time point `0` (you can think of this as the start of the work day) and and ends at `timeHorizon` (you can think of this as the end of the work day).
- `vehicleCapacity` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The maximum capacity for goods each vehicle can carry. Demand at nodes decrease the capacity.
- `routeLocks` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** Route locks array the solver uses for locking (sub-) routes into place, per vehicle. Two-dimensional with `routeLocks[vehicle]` being an **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** with node indices `vehicle` has to visit in order. Can be empty. Must not contain the depots.
- `pickups` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** with node indices for picking up good. The corresponding delivery node index is in the `deliveries` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** at the same position (parallel arrays). For a pair of pickup and delivery indices: pickup location comes before the corresponding delivery location and is served by the same vehicle.
- `deliveries` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** with node indices for delivering picked up goods. The corresponding pickup node index is in the `pickups` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** at the same position (parallel arrays). For a pair of pickup and delivery indices: pickup location comes before the corresponding delivery location and is served by the same vehicle.

**Examples**

```javascript
var vrpSearchOpts = {
  computeTimeLimit: 1000,
  numVehicles: 3,
  depotNode: depotNode,
  timeHorizon: 9 * 60 * 60,
  vehicleCapacity: 3,
  routeLocks: [[], [3, 4], []]
  pickups: [4, 9],
  deliveries: [12, 8]
};

VRP.Solve(vrpSearchOpts, function (err, solution) {
  if (err) return console.log(err);
  console.log(util.inspect(solution, {showHidden: false, depth: null}));
});
```

**Result**

**[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)** with:
- `cost` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** internal objective to optimize for.
- `routes` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** with **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** indices into the locations for the vehicle to visit in order. Per vehicle.
- `times` **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** with **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** `[earliest, latest]` service times at the locations for the vehicle to visit in order. Per vehicle. The solver starts from time point `0` (you can think of this as the start of the work day) and the time points are positive offsets to this time point.

**Examples**

```javascript
{ cost: 90,
  routes: [ [ 4, 5, 9 ], [ 3, 7, 8 ], [ 1, 2, 6 ] ],
  times: 
   [ [ [ 2700, 3600 ], [ 8400, 9300 ], [ 17100, 18000 ] ],
     [ [ 2100, 2400 ], [ 8400, 8700 ], [ 17700, 18000 ] ],
     [ [ 900, 10800 ], [ 3000, 12900 ], [ 8100, 18000 ] ] ]}
```
