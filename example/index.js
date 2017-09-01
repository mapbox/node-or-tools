#!/usr/bin/env node

'use strict';

// Query the Mapbox Distance Matrix API, run VRP solver on its output.
//
// The API returns a matrix with
// - the duration between location i and j
// - thus `0` on its diagonal i == j
// - and `null` if a route can not be found between i and j
//
// https://github.com/mapbox/mapbox-sdk-js/blob/master/API.md#getdistances
// https://www.mapbox.com/api-documentation/#directions-matrix
// https://www.mapbox.com/api-documentation/#directions
//
// Example output solution:
// http://bl.ocks.org/d/d0e91bc26f437aba812c554f7a5b1c2b

var util = require('util');

var Solver = require('../');
var Mapbox = require('mapbox');
var turf = {
  feature: require('@turf/helpers').feature,
  point: require('@turf/helpers').point,
  featureCollection: require('@turf/helpers').featureCollection
};
var fs = require('fs');
var async = {
  eachOf: require('async').eachOf
};
var colors = require('colorbrewer').Dark2[8];


// Here are all the tunables you might be interested in


var locations = [
  [13.414649963378906, 52.522905940278065],
  [13.363409042358397, 52.549218541178455],
  [13.394737243652344, 52.55062769982075],
  [13.426065444946289, 52.54640008814808],
  [13.375682830810547, 52.536534077147714],
  [13.39010238647461, 52.546191306649376],
  [13.351736068725584, 52.50754964045259],
  [13.418254852294922, 52.52927670688215],
];

var depotIndex = 0;
var numVehicles = 3;
var vehicleCapacity = 3;
var computeTimeLimit = 1000;
var profile = 'driving';

// that was a lie, there are more tunables below - start with the ones above first.


var MbxToken = process.env.MAPBOX_ACCESS_TOKEN;

if (!MbxToken) {
  console.error('Please set your Mapbox API Token: export MAPBOX_ACCESS_TOKEN=YourToken');
  process.exit(1);
}

var MbxClient = new Mapbox(MbxToken)


function hasNoRouteFound(matrix) {
  matrix.some(function (inner) {
    return inner.some(function (v) {
      return v === null;
    });
  });
}


MbxClient.getMatrix(locations.map(function (coord) { return { longitude: coord[0], latitude: coord[1] } }), {profile: profile}, function(err, results) {
  if (err) {
    console.error('Error: ' + err.message);
    process.exit(1);
  }

  if (hasNoRouteFound(results.durations)) {
    console.error('Error: distance matrix is not complete');
    process.exit(1);
  }


  // 9am -- 5pm
  var dayStarts = 0;
  var dayEnds = 8 * 60 * 60;

  var costs = results.durations;

  // Dummy durations, no service times included
  var durations = results.durations;

  // Dummy time windows for the full day
  var timeWindows = new Array(results.durations.length);

  for (var at = 0; at < results.durations.length; ++at)
    timeWindows[at] = [dayStarts, dayEnds];

  // Dummy demands of one except at the depot
  var demands = new Array(results.durations.length);

  for (var from = 0; from < results.durations.length; ++from) {
    demands[from] = new Array(results.durations.length);

    for (var to = 0; to < results.durations.length; ++to) {
      demands[from][to] = (from === depotIndex) ? 0 : 1;
    }
  }

  // No route locks per vehicle, let solver decide freely
  var routeLocks = new Array(numVehicles);
  routeLocks.fill([]);


  var solverOpts = {
    numNodes: results.durations.length,
    costs: costs,
    durations: durations,
    timeWindows: timeWindows,
    demands: demands
  };


  var VRP = new Solver.VRP(solverOpts);

  var timeHorizon = dayEnds - dayStarts;

  var searchOpts = {
    computeTimeLimit: computeTimeLimit,
    numVehicles: numVehicles,
    depotNode: depotIndex,
    timeHorizon: timeHorizon,
    vehicleCapacity: vehicleCapacity,
    routeLocks: routeLocks,
    pickups: [],
    deliveries: []
  };

  VRP.Solve(searchOpts, function (err, result) {
    if (err) {
      console.error('Error: ' + err.message);
      process.exit(1);
    }

    var solutionFeatures = [];
    // add depot to solution file
    solutionFeatures.push(turf.point(locations[0], {
      'marker-color': '#333',
      'marker-symbol': 'building'
    }));

    console.log(util.inspect(result, {showHidden: false, depth: null}));

    // add route points to solution file
    result.routes.forEach(function (route, routeIndex) {
      route.forEach(function (stop, stopIndex) {
        solutionFeatures.push(turf.point(locations[stop], {
          route: routeIndex + 1,
          stop: stopIndex + 1,
          'marker-color': colors[routeIndex % colors.length],
          'marker-symbol': stopIndex + 1
        }));
      });
    });

    // Now that we have the location orders per vehicle make route requests to extract their geometry
    async.eachOf(result.routes, function (route, index, callback) {

      // Unused vehicle
      if (route.length === 0)
        callback();

      var waypoints = route.map(function(idx) {
        return {'longitude': locations[idx][0], 'latitude': locations[idx][1]};
      });

      // Add depot explicitly as start and end
      waypoints.unshift({'longitude': locations[depotIndex][0], 'latitude': locations[depotIndex][1]});
      waypoints.push({'longitude': locations[depotIndex][0], 'latitude': locations[depotIndex][1]});


      MbxClient.getDirections(waypoints, {profile: profile, alternatives: false}, function(err, results) {
        if (err) {
          console.error('Error: ' + err.message);
          process.exit(1);
        }

        // add this route to the solution file
        solutionFeatures.push(turf.feature(results.routes[0].geometry, {
          route: index + 1,
          distance: results.routes[0].distance,
          duration: results.routes[0].duration,
          stroke: colors[index % colors.length],
          'stroke-width': 4
        }));

        callback();
      });
    }, function (err) {
      // write the solution.geojson file
      var solution = turf.featureCollection(solutionFeatures);
      fs.writeFileSync('solution.geojson', JSON.stringify(solution, null, 4));
    });
  });
});
