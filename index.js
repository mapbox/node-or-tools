var node_or_tools = require('.');

if (require.main === module) {
  console.log('self-testing module');


  // Arc costs callback for the solvers.
  // Args: `from` and `to` indices (Number)
  // Returns: costs for traversing arc (Number)
  // Note: indices are [0, numNodes - 1]
  var cost = function (from, to) { return from + to; };


  // Traveling Salesman Problem

  var solverOpts = {
    numNodes: 10,
    costFunction: cost
  };

  var TSP = new node_or_tools.TSP(solverOpts);

  var searchOpts = {
    timeLimit: 1000,
    depotNode: 0
  };

  TSP.Solve(searchOpts, function (err, solution) {
    if (err) return;
    console.log(solution);
  });


  // Vehicle Routing Problem

  var VRP = new node_or_tools.VRP();
  var VRPOpts = {}
  VRP.Solve(VRPOpts);
}

module.exports = node_or_tools;
