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
var Mapbox = require('@mapbox/mapbox-sdk');
var matrixService = require('@mapbox/mapbox-sdk/services/matrix');
var directionsService = require('@mapbox/mapbox-sdk/services/directions');
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
var d3 = require('d3-queue');

const bboxes = {
    chicago: {
        min: { lat:  41.58463401188338, lng:  -88.39000244140625 },
        max: { lat:  42.87580094787546, lng: -87.69561767578125 }
    },
    atlanta: {
        min: { lat: 33.49788816685207, lng: -84.72656249999999 },
        max: { lat: 34.04583232505719, lng: -84.1387939453125 }
    },
    boston: {
        min: { lat: 42.02889410108475, lng: -71.81488037109375 },
        max: { lat: 42.50450285299051, lng: -70.72998046875 }
    },
    sacramento: {
        min: { lat: 37.374522644077246, lng: -121.62274902343749 },
        max: { lat: 38.955509760012, lng: -121.0308837890625 }
    },
    denver: {
        min: { lat: 39.56970506644249, lng: -105.29571533203125 },
        max: { lat: 40.07912221750036, lng: -104.61044311523438 }
    },
    colorado: {
        min: { lat: 37.16907157713011, lng: -108.929443359375 },
        max: { lat: 40.91351257612758, lng: -102.26074218749999 }
    }
};


// var MbxToken = process.env.MAPBOX_ACCESS_TOKEN;
var MbxToken = 'pk.eyJ1IjoiZ2hvc2hrYWoiLCJhIjoiY2preTIxZXl3MGV4MDN2cWtyaTdmZmY5cSJ9.Zp5aq1qLLtKDdW-th-tV4w';

if (!MbxToken) {
  console.error('Please set your Mapbox API Token: export MAPBOX_ACCESS_TOKEN=YourToken');
  // process.exit(1);
  return queueCallback(new Error('Please set your Mapbox API Token: export MAPBOX_ACCESS_TOKEN=YourToken')); 
}

var MbxMatrixClient = matrixService(Mapbox({ accessToken: MbxToken }));
var MbxDirectionsClient = directionsService(Mapbox({ accessToken: MbxToken }));

var profile = 'driving';

function hasNoRouteFound(matrix) {
  matrix.some(function (inner) {
    return inner.some(function (v) {
      return v === null;
    });
  });
}

function routesToGeojson(routes, locations, depotIndex, geojsonCallback) {

  var solutionFeatures = [];
  // add depot to solution file
  solutionFeatures.push(turf.point(locations[0], {
    'marker-color': '#333',
    'marker-symbol': 'building'
  }));

  // add route points to solution file
  routes.forEach(function (route, routeIndex) {
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
  async.eachOf(routes, function (route, index, callback) {

      // Unused vehicle
      if (route.length === 0)
        return callback();

      var waypoints = route.map(function(idx) {
        return {'coordinates': locations[idx]};
      });

      // Add depot explicitly as start and end
      waypoints.unshift({'coordinates': locations[depotIndex]});
      waypoints.push({'coordinates': locations[depotIndex]});


      MbxDirectionsClient.getDirections({ waypoints, profile: profile, alternatives: false, geometries: 'geojson'}).send()
        .then(function(response) {
        var results = response.body;
        // add this route to the solution file
        solutionFeatures.push(turf.feature(results.routes[0].geometry, {
          route: index + 1,
          distance: results.routes[0].distance,
          duration: results.routes[0].duration,
          stroke: colors[index % colors.length],
          'stroke-width': 4
        }));

        callback();
      })
      .catch(function(err) {
        return callback(err);
       });
    }, function (err) {
      if(err) return geojsonCallback(err);

      // write the solution.geojson file
      var solution = turf.featureCollection(solutionFeatures);
      
      geojsonCallback(null, solution);
    });

}

function vrpSolver(locations, computeTime, queueCallback) {

  var depotIndex = 0;
  var numVehicles = 1;
  var vehicleCapacity = 250;
  var computeTimeLimit = computeTime;

  // that was a lie, there are more tunables below - start with the ones above first.

  var points = locations.map(function (coord) { return { coordinates: coord } });

  MbxMatrixClient.postMatrix({ points: points, profile: profile})
    .send()
    .then(function(response) {
      var results = response.body;

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

      var start = Date.now();
      VRP.Solve(searchOpts, function (err, result) {
        var end = Date.now();
        if (err) {
          console.error('Error in solving: ' + err.message);
          return queueCallback(err); 
        }

        // console.log(util.inspect(result, {showHidden: false, depth: null}));
        var timeTaken = end - start;
        console.log(locations.length + ',' + computeTime  + ',' + timeTaken + ',' + result.cost);
        queueCallback();

        // routesToGeojson(result.routes, locations, depotIndex, function(err, featureCollection) {
        //   if (err) {
        //     console.error('Error in getting directions after VRP has produced solution: ' + err.message);
        //     return queueCallback(err);
        //   }
        //   fs.writeFileSync(locations.length + 'locations' + computeTime + 'ms' + '.geojson', JSON.stringify(featureCollection, null, 4));
        //   queueCallback();
        // });
      });
  }).catch(function(err) {
    if (err) {
      console.error('Error getting matrix: ' + err.message);
       return queueCallback(err);
      //process.exit(1);
    }

    if (hasNoRouteFound(results.durations)) {
      console.error('Error: distance matrix is not complete');
      // process.exit(1);
      return queueCallback(new Error('Error: distance matrix is not complete'));
    }
  });
}

var q = d3.queue(1);

for (var numLocations = 30; numLocations < 251; numLocations+=5) {
   const CITY = process.env.City || 'atlanta';
   let bbox = bboxes[CITY];

   var locations = [];
   for (let i = 0; i < numLocations; i++) {
    locations.push([Math.random() * Math.abs(bbox.max.lng - bbox.min.lng) + bbox.min.lng, Math.random() * Math.abs(bbox.max.lat - bbox.min.lat) + bbox.min.lat]);
   }

   for (var computeTime = 300; computeTime < 300000; computeTime+=500) {
      q.defer(vrpSolver, locations, computeTime);
   }
}

q.awaitAll(function(error) {
  if (error) {
    console.log('there seems to be a problem');
  }
   console.log("All done!");
});
