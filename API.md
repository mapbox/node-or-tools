# VRP

Solves the Vehicle Routing Problem (VRP) with constraints (time windows, capacities and more).
The VRP solver is split into two: the `VRP` constructor taking constructor-specific options and the asynchronous `Solve` function taking search-specific options.


## Constructor

Constructs a VRP solver object. Initializes and caches user data internally for efficiency.


**Parameters**

**[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)** with solver-specific options:
- `numNodes` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** Number of locations ("nodes").
- `costs` **[Function](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function)** Cost function the solver minimizes. Can for example be duration, distance but doesn not have to be. Takes two parameters: `from` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** and `to` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** node indices in the range `[0, numNodes - 1]`. Return value has to be a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the cost for traversing the arc from `from` to `to`.
- `durations` **[Function](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function)** Duration function the solver uses for time constraints. Takes two parameters: `from` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** and `to` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** node indices in the range `[0, numNodes - 1]`. Return value has to be a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the duration for servicing node `from` plus the time for traversing the arc from `from` to `to`.
- `timeWindows` **[Function](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function)** Time window function the solver uses for time constraints. Takes one parameter: `at` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** node index in the range `[0, numNodes - 1]`. Return value has to be an **[Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array)** of two **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the start and end time point of the time window when servicing the node `at` is allowed. The solver starts from time point `0` (you can think of this as the start of the work day) and the time points need to be positive offsets to this time point.
- `demands` **[Function](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Function)** Demands function the solver uses for vehicle capacity constraints. Takes two parameter: `from` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** and `to` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** node indices in the range `[0, numNodes - 1]`. Return value has to be a **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** representing the demand at node `from`, for example number of packages to deliver to this location. The `to` node index is unused and reserved for future changes. The depot should have a demand of zero.


**Examples**

```javascript
var costs = function (from, to) { return distances[from][to]; };
var durations = function (from, to) { return 5 * 60 + duration[from][to]; };
var timeWindows = function (at) { return [0 * 60 * 60, 5 * 60 * 60]; };
var demands = function (from, to) { return from == depotNode ? 0 : 1; }

var vrpSolverOpts = {
  numNodes: 10,
  costs: costs,
  durations: durations,
  timeWindows: timeWindows,
  demands: demands
};

var VRP = new node_or_tools.VRP(vrpSolverOpts);
```


## Solve

Runs the VRP solver asynchronously to search for a solution.


**Parameters**

- `computeTimeLimit` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** Time limit in milliseconds for the solver. In general, the longer you run the solver the better the solution (if there is any) will be. The solver will never run longer than this time limit but can finish earlier.
- `numVehicles` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The number of vehicles for servicing nodes.
- `depotNode` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The depot node index in the range `[0, numNodes - 1]` where all vehicles start and end at.
- `timeHorizon` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The last time point the solver uses for time constraints. The solver starts from time point `0` (you can think of this as the start of the work day) and and ends at `timeHorizon` (you can think of this as the end of the work day).
- `vehicleCapacity` **[Number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number)** The maximum capacity for goods each vehicle can carry. Demand at nodes decrease the capacity.


**Examples**

```javascript
var vrpSearchOpts = {
  computeTimeLimit: 1000,
  numVehicles: 3,
  depotNode: depotNode,
  timeHorizon: 9 * 60 * 60,
  vehicleCapacity: 3
};

VRP.Solve(vrpSearchOpts, function (err, solution) {
  if (err) return console.log(err);
  console.log(util.inspect(solution, {showHidden: false, depth: null}));
});
```
