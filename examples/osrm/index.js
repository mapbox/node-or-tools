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

const depotIndex = 0


osrm.table({coordinates: locations}, (err, resp) => {
  if (err)
    return;

  const solverOpts = {
    numNodes: resp.durations.length,
    costFunction: (s, t) => { return resp.durations[s][t]; }
  };

  const TSP = new Solver.TSP(solverOpts);

  const searchOpts = {
    timeLimit: 1000,
    depotNode: depotIndex
  };

  const routes = TSP.Solve(searchOpts);

  for (const route of routes) {
    const depot = locations[depotIndex];

    console.log(depot);

    for (const node of route) {
      console.log(locations[node]);
    }
  }

});
