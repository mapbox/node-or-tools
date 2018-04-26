var test = require('tape');
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

var costMatrix = new Array(locations.length);

for (var from = 0; from < locations.length; ++from) {
  costMatrix[from] = new Array(locations.length);

  for (var to = 0; to < locations.length; ++to) {
    costMatrix[from][to] = manhattanDistance(locations[from], locations[to]);
  }
}


test('Test TSP', function(assert) {

  var solverOpts = {
    numNodes: locations.length,
    costs: costMatrix
  };

  var TSP = new ortools.TSP(solverOpts);

  var searchOpts = {
    computeTimeLimit: 1000,
    depotNode: depot
  };

  TSP.Solve(searchOpts, function (err, solution) {
    assert.ifError(err, 'Solution can be found');

    assert.ok(solution instanceof Array, 'Solution is Array of locations');
    assert.equal(solution.length, locations.length - 1, 'Number of locations in route is number of locations without depot');

    assert.ok(!solution.find(function (v) { return v == depot; }), 'Depot is not in routes');

    function adjacentCost(acc, v) { return { cost: acc.cost + costMatrix[acc.at][v], at: v }; }
    var route = solution.reduce(adjacentCost, { cost: 0, at: depot });
    assert.equal(route.cost, locations.length - 1, 'Costs are minimum Manhattan Distance in location grid');

    assert.end();
  });

});

test('Should throw on empty constructor params', (t) => {

  try {
    var TSP = ortools.TSP();
    t.fail('Should never get here');
  }
  catch (err) {
    t.ok(err, 'Should throw an error if no args');
  }

  try {
    var TSP = new ortools.TSP();
    t.fail('Should never get here');
  }
  catch (err) {
    t.ok(err, 'Should throw an error if no args');
  }
  t.end();
  
});

test('Alternative TSP object constructor', function(t) {
  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix
    };

    var TSP = ortools.TSP(solverOpts);
    t.ok('Should construct without `new`');
  }
  catch (err) {
    console.log(err);
    t.fail(err, 'Should not fail');
  }
  t.end();
});