var tap = require('tap');
var ortools = require('..')


// Locations in a grid and Manhattan Distance for costs

var locations = [[0, 0], [0, 1], [0, 2], [0, 3],
                 [1, 0], [1, 1], [1, 2], [1, 3],
                 [2, 0], [2, 1], [2, 2], [2, 3],
                 [3, 0], [3, 1], [3, 2], [3, 3]];

var depot = 0;

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

function manhattanDistance(lhs, rhs) {
  return Math.abs(lhs[0] - rhs[0]) + Math.abs(lhs[1] - rhs[1]);
}

function costs(s, t) {
  return manhattanDistance(locations[s], locations[t]);
}

function Seconds(v) { return v; };
function Minutes(v) { return Seconds(v * 60); }
function Hours(v)   { return Minutes(v * 60); }

function durations(s, t) {
  var serviceTime = Minutes(3);
  var travelTime = Minutes(costs(s, t));

  return serviceTime + travelTime;
}

function timeWindows(at) {
  if (at == depot)
    return [dayStarts, dayEnds];

  var earliest = dayStarts;
  var latest = dayEnds - Hours(1);

  var start = rand() * (latest - earliest) + earliest;
  var stop = rand() * (latest - start) + start;

  return [start, stop];
}

function demands(s, t) {
  if (s === depot)
    return 0;
  else
    return 1;
}


tap.test('Test VRP', function(assert) {
  var solverOpts = {
    numNodes: locations.length,
    costs: costs,
    durations: durations,
    timeWindows: timeWindows,
    demands: demands
  };

  var VRP = new ortools.VRP(solverOpts);

  var numVehicles = 10;
  var timeHorizon = dayEnds - dayStarts;
  var vehicleCapacity = 10;

  var searchOpts = {
    computeTimeLimit: 1000,
    numVehicles: numVehicles,
    depotNode: depot,
    timeHorizon: timeHorizon,
    vehicleCapacity: vehicleCapacity
  };

  VRP.Solve(searchOpts, function (err, solution) {
    assert.ifError(err, 'Solution can be found');

    assert.type(solution, Object, 'Solution is Object with properties');
    assert.type(solution.routes, Array, 'Routes in solution is Array with routes per vehicle');
    assert.type(solution.times, Array, 'Times in solution is Array with time windows per vehicle');

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

    assert.end();
  });
});
