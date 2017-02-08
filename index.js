'use strict';

const node_or_tools = require('.');

if (require.main === module) {
  console.log('self-testing module');


  // Arc costs callback for the solvers.
  // Args: `from` and `to` indices (Number)
  // Returns: costs for traversing arc (Number)
  // Note: indices are [0, numNodes - 1]
  const cost = (from, to) => { return from + to; };


  // Traveling Salesman Problem

  const solverOpts = {
    numNodes: 10,
    costFunction: cost
  };

  const TSP = new node_or_tools.TSP(solverOpts);

  const searchOpts = {
    timeLimit: 1000,
    depotNode: 0
  };

  const solution = TSP.Solve(searchOpts);
  console.log(solution);


  // Vehicle Routing Problem

  const VRP = new node_or_tools.VRP();
  const VRPOpts = {}
  VRP.Solve(VRPOpts);
}

module.exports = node_or_tools;
