var node_or_tools = require('.');
var util = require('util')


// Traveling Salesman Problem
function tsp() {
  var costs = function (from, to) { return from + to; };

  var tspSolverOpts = {
    numNodes: 10,
    costs: costs
  };

  var TSP = new node_or_tools.TSP(tspSolverOpts);

  var tspSearchOpts = {
    computeTimeLimit: 1000,
    depotNode: 0
  };

  TSP.Solve(tspSearchOpts, function (err, solution) {
    if (err) return console.log(err);
    console.log(solution);
  });
}


// Vehicle Routing Problem with Time Windows
function vrp() {
  var Seconds = function (v) { return v; };
  var Minutes = function (v) { return Seconds(v * 60); };
  var Hours = function (v) { return Minutes(v * 60); }

  var depotNode = 0;

  var costs = function (from, to) { return from + to; };
  var durations = function (from, to) { return Minutes(5) + Minutes(10 * from + 10 * to); };
  var timeWindows = function (at) { return [Hours(0), Hours(5)]; };
  var demands = function (from, to) { return from == depotNode ? 0 : 1; }

  var vrpSolverOpts = {
    numNodes: 10,
    costs: costs,
    durations: durations,
    timeWindows: timeWindows,
    demands: demands
  };

  var VRP = new node_or_tools.VRP(vrpSolverOpts);

  var vrpSearchOpts = {
    computeTimeLimit: 1000,
    numVehicles: 3,
    depotNode: depotNode,
    timeHorizon: Hours(9),
    vehicleCapacity: 3
  };

  VRP.Solve(vrpSearchOpts, function (err, solution) {
    if (err) return console.log(err);
    console.log(util.inspect(solution, {showHidden: false, depth: null}));
  });
}


if (require.main === module) {
  console.log('self-testing module');

  tsp();
  vrp();
}

module.exports = node_or_tools;
