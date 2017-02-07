'use strict';

const Solver = require('../../');
const OSRM = require('osrm');

const osrm = new OSRM('monaco-latest.osrm');

const locations = [
  [7.417059, 43.728220],
  [7.414699, 43.734050],
  [7.422380, 43.732344],
  [7.419891, 43.737461]
];

osrm.table({coordinates: locations}, (err, resp) => {
  if (err)
    return;

  const TSP = new Solver.TSP();

  const SolverOpts = {
    numNodes: resp.durations.length,
    costFunction: (s, t) => { return resp.durations[s][t]; }
  };

  const routes = TSP.Solve(SolverOpts);

  for (const route of routes) {
    const depot = locations[0]; // depot is always 0 at the moment; configurable

    console.log(depot);

    for (const node of route) {
      console.log(locations[node]);
    }
  }

});
