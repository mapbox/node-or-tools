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

var dayStarts = Hours(0);
var dayEnds = Hours(3);


var seed = 2147483650;

function ParkMillerRNG(seed) {
  var modulus = 2147483647;
  var multiplier = 48271;
  var increment = 0;
  var state = seed;

  return function() {
    state = (multiplier * state + increment) % modulus;
    return state / modulus;
  };
}

var rand = ParkMillerRNG(seed);


function Seconds(v) { return v; };
function Minutes(v) { return Seconds(v * 60); }
function Hours(v)   { return Minutes(v * 60); }


var durationMatrix = new Array(locations.length);

for (var from = 0; from < locations.length; ++from) {
  durationMatrix[from] = new Array(locations.length);

  for (var to = 0; to < locations.length; ++to) {
    var serviceTime = Minutes(3);
    var travelTime = Minutes(costMatrix[from][to]);

    durationMatrix[from][to] = serviceTime + travelTime;
  }
}


var timeWindows = new Array(locations.length);

for (var at = 0; at < locations.length; ++at) {
  if (at === depot) {
    timeWindows[at] = [dayStarts, dayEnds];
    continue;
  }

  var earliest = dayStarts;
  var latest = dayEnds - Hours(1);

  var start = rand() * (latest - earliest) + earliest;
  var stop = rand() * (latest - start) + start;

  timeWindows[at] = [start, stop];
}


var demandMatrix = new Array(locations.length);

for (var from = 0; from < locations.length; ++from) {
  demandMatrix[from] = new Array(locations.length);

  for (var to = 0; to < locations.length; ++to) {
    if (from === depot)
      demandMatrix[from][to] = 0
    else
      demandMatrix[from][to] = 1
  }
}

var numVehicles = 10;
var timeHorizon = dayEnds - dayStarts;
var vehicleCapacity = 10;

// Dummy lock to let vehicle 0 go to location 2 and 3 first - to test route locks
var routeLocks = new Array(numVehicles);

for (var vehicle = 0; vehicle < numVehicles; ++vehicle) {
  if (vehicle === 0)
    routeLocks[vehicle] = [2, 3];
  else
    routeLocks[vehicle] = [];
}

test('Test VRP', function(assert) {

  var solverOpts = {
    numNodes: locations.length,
    costs: costMatrix,
    durations: durationMatrix,
    timeWindows: timeWindows,
    demands: demandMatrix
  };

  var VRP = new ortools.VRP(solverOpts);

  var searchOpts = {
    computeTimeLimit: 1000,
    numVehicles: numVehicles,
    depotNode: depot,
    timeHorizon: timeHorizon,
    vehicleCapacity: vehicleCapacity,
    routeLocks: routeLocks,
    pickups: [4, 12],
    deliveries: [9, 8]
  };

  VRP.Solve(searchOpts, function (err, solution) {
    assert.ifError(err, 'Solution can be found');

    assert.ok(solution instanceof Object, 'Solution is Object with properties');
    assert.ok(solution.routes instanceof Array, 'Routes in solution is Array with routes per vehicle');
    assert.ok(solution.times instanceof Array, 'Times in solution is Array with time windows per vehicle');

    assert.equal(solution.routes.length, numVehicles, 'Number of routes is number of vehicles');
    assert.equal(solution.times.length, numVehicles, 'Number of time windows in number of vehicles');

    function used(v) { return v.length == 0 ? 0 : 1; }
    function addition(l, r) { return l + r; }

    var numVehiclesUsed = solution.routes.map(used).reduce(addition, 0);

    assert.ok(numVehiclesUsed > 0, 'Solution uses vehicles');
    assert.ok(numVehiclesUsed <= numVehiclesUsed, 'Solution uses up to number of vehicles');

    function checkRoute(v) {
      var depotInRoute = v.find(function (u) { return u == depot; });
      assert.ok(!depotInRoute, 'Depot is not in route');
    }

    function checkTimeWindows(v) {
      v.forEach(function (u) {
        assert.ok(u[0] < u[1], 'Valid Time Window');
      });
    }

    solution.routes.forEach(checkRoute);
    solution.times.forEach(checkTimeWindows);

    // We locked vehicle 0 to go to location 2 and 3 first
    assert.equal(solution.routes[0][0], 2);
    assert.equal(solution.routes[0][1], 3);

    assert.end();
  });
});

test('Should throw on empty constructor params', (t) => {

  try {
    var VRP1 = ortools.VRP();
    t.fail('Should never get here');
  }
  catch (err) {
    t.ok(err, 'Should throw an error if no args');
  }

  try {
    var VRP1 = new ortools.VRP();
    t.fail('Should never get here');
  }
  catch (err) {
    t.ok(err, 'Should throw an error if no args');
  }
  t.end();
  
});

test('Alternative constructor call', (t) => {
  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows,
      demands: demandMatrix
    };

    var VRP = ortools.VRP(solverOpts);
    t.ok('Should be constructable without new');
  }
  catch (err) {
    t.fail(err);
  }
  t.end();
});

test('Invalid parameter handling', (t) => {

  try {
    reversedTimeWindows = JSON.parse(JSON.stringify(timeWindows)).map(n => n.reverse());
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: reversedTimeWindows,
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception should be thrown for negative time window intervals');
  }

  try {
    nonNumericDurations = durationMatrix.map(m => m.map(n => 'a'));
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: nonNumericDurations,
      timeWindows: timeWindows,
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception should be thrown when durations are non-numeric');
  }

  try {
    var solverOpts = {
      numNodes: -1,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows,
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception should be thrown when numNodes is negative');
  }

  try {
    var solverOpts = {
      numNodes: 1,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows,
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when numNodes doesn\'t match actual data sizes');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix.map(m => 1),
      durations: durationMatrix,
      timeWindows: timeWindows,
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when parmeter isn\'t an array of arrays');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix.map(n => [1,2]),
      timeWindows: timeWindows,
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when inner arrays sizes don\'t match outer');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows.map(n => n.map(m => 'a')),
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when inner arrays don\'t contain numbers');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: [],
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when time windows array isn\'t the same size as numNodes');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows.map(n => 'a'),
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when time windows array isn\'t an array of arrays');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows.map(n => n.map(m => [])),
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when time windows inner arrays aren\'t of length 2');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows.map(n => n.map(m => ['a','b'])),
      demands: demandMatrix
    };

    var VRP = new ortools.VRP(solverOpts);
    t.fail('Should not get here');
  } catch (err) {
    t.ok(err, 'Exception thrown when time windows inner arrays arent numeric');
  }

  try {
    var solverOpts = {
      numNodes: locations.length,
      costs: costMatrix,
      durations: durationMatrix,
      timeWindows: timeWindows.map(n => n.map(m => ['a','b'])),
      demands: demandMatrix
    };

    for (var i in solverOpts) {
      try {
        var o = JSON.parse(JSON.stringify(solverOpts));
        delete o[i];
        var VRP = new ortools.VRP(o);
        t.fail('Should not get here');
      } catch(err) {
        t.ok(err, `Exception when VRP build with missing parameter ${i}`);
      }
    }
  }
  catch (err) {
    t.fail('Should not fail here');
  }


  var validSolverOpts = {
    numNodes: locations.length,
    costs: costMatrix,
    durations: durationMatrix,
    timeWindows: timeWindows,
    demands: demandMatrix
  };
  var validVRP = new ortools.VRP(validSolverOpts);

  try {
    validVRP.Solve();
    t.fail('Should not get here');
  } catch(err) {
    t.ok(err, 'Exception when not enough parameters to Solve() call (no params)');
  }

  try {
    validVRP.Solve({});
    t.fail('Should not get here');
  } catch(err) {
    t.ok(err, 'Exception when not enough parameters to Solve() call (only one param)');
  }

  try {
    validVRP.Solve(1,2);
    t.fail('Should not get here');
  } catch(err) {
    t.ok(err, 'Exception when Solve() parameter types are not correct (wrong param types)');
  }

  try {

    var searchOpts = {
      computeTimeLimit: 1000,
      numVehicles: numVehicles,
      depotNode: depot,
      timeHorizon: timeHorizon,
      vehicleCapacity: vehicleCapacity,
      routeLocks: routeLocks,
      pickups: [4, 12],
      deliveries: [9, 8]
    };
    // Delete each option, make sure they're all required
    for (var i in searchOpts) {
      try {
        var o = JSON.parse(JSON.stringify(searchOpts));
        delete o[i];
        validVRP.Solve(o, () => { t.fail('Should not callback'); });
        t.fail('Should not get here');
      } catch(err) {
        t.ok(err, `Exception when Solve() parameter is missing ${i}`);
      }
    }
  }
  catch (erro) {
    t.fail('Should not fail here');
  }

  try {

    var searchOpts = {
      computeTimeLimit: 1000,
      numVehicles: numVehicles,
      depotNode: depot,
      timeHorizon: timeHorizon,
      vehicleCapacity: vehicleCapacity,
      routeLocks: [],
      pickups: [4, 12],
      deliveries: [9, 8]
    };
    validVRP.Solve(searchOpts, () => { t.fail('Should not callback'); });
    t.fail('Should not get here');
  }
  catch (err) {
    t.ok(err, 'Exception when calling Solve() and there aren\'t enough routelocks');
  }

  try {

    var searchOpts = {
      computeTimeLimit: 1000,
      numVehicles: numVehicles,
      depotNode: depot,
      timeHorizon: timeHorizon,
      vehicleCapacity: vehicleCapacity,
      routeLocks: routeLocks.map(n => [1000,1000]),
      pickups: [4, 12],
      deliveries: [9, 8]
    };
    validVRP.Solve(searchOpts, () => { t.fail('Should not callback'); });
    t.fail('Should not get here');
  }
  catch (err) {
    t.ok(err, 'Exception when calling Solve() and routeLocks refer to invalid nodes');
  }

  try {

    var searchOpts = {
      computeTimeLimit: 1000,
      numVehicles: numVehicles,
      depotNode: depot,
      timeHorizon: timeHorizon,
      vehicleCapacity: vehicleCapacity,
      routeLocks: routeLocks.map(n => [depot,depot]),
      pickups: [4, 12],
      deliveries: [9, 8]
    };
    validVRP.Solve(searchOpts, () => { t.fail('Should not callback'); });
    t.fail('Should not get here');
  }
  catch (err) {
    t.ok(err, 'Exception when calling Solve() and routeLocks refer to depot');
  }

  try {

    var searchOpts = {
      computeTimeLimit: 1000,
      numVehicles: numVehicles,
      depotNode: depot,
      timeHorizon: timeHorizon,
      vehicleCapacity: vehicleCapacity,
      routeLocks: routeLocks,
      pickups: [4, 12],
      deliveries: [1]
    };
    validVRP.Solve(searchOpts, () => { t.fail('Should not callback'); });
    t.fail('Should not get here');
  }
  catch (err) {
    t.ok(err, 'Exception when calling Solve() and pickups/deliveries don\'t match');
  }

  t.end();

});