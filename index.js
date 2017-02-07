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

  const TSP = new node_or_tools.TSP();

  const TSPOpts = {
    numNodes: 10,
    costFunction: cost
  };

  const solution = TSP.Solve(TSPOpts);
  console.log(solution);


  // Vehicle Routing Problem

  const VRP = new node_or_tools.VRP();
  const VRPOpts = {}
  VRP.Solve(VRPOpts);
}

module.exports = node_or_tools;
