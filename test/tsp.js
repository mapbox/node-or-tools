var tap = require('tap');
var ortools = require('..')


// Locations in a grid and Manhattan Distance for costs

var locations = [[0, 0], [0, 1], [0, 2], [0, 3],
                 [1, 0], [1, 1], [1, 2], [1, 3],
                 [2, 0], [2, 1], [2, 2], [2, 3],
                 [3, 0], [3, 1], [3, 2], [3, 3]];

var depot = 0;

function manhattanDistance(lhs, rhs) {
  return Math.abs(lhs[0] - rhs[0]) + Math.abs(lhs[1] - rhs[1]);
}

function costs(s, t) {
  return manhattanDistance(locations[s], locations[t]);
}


tap.test('Test TSP', function(assert) {
  var solverOpts = {
    numNodes: locations.length,
    costs: costs
  };

  var TSP = new ortools.TSP(solverOpts);

  var searchOpts = {
    computeTimeLimit: 1000,
    depotNode: depot
  };

  TSP.Solve(searchOpts, function (err, solution) {
    assert.ifError(err, 'Solution can be found');

    assert.type(solution, Array, 'Solution is Array with routes per vehicle');
    assert.equal(solution.length, 1, 'Solution has one route Array with locations');
    assert.type(solution[0], Array, 'Route is Array of locations');
    assert.equal(solution[0].length, locations.length - 1, 'Number of locations in route is number of locations without depot');

    assert.ok(!solution[0].find(function (v) { return v == depot; }), 'Depot is not in routes');

    function adjacentCost(acc, v) { return { cost: acc.cost + costs(acc.at, v), at: v }; }
    var route = solution[0].reduce(adjacentCost, { cost: 0, at: depot });
    assert.equal(route.cost, locations.length - 1, 'Costs are minimum Manhattan Distance in location grid');

    assert.end();
  });

});
