var tap = require('tap');
var ortools = require('..')


// Locations in a grid and Manhattan Distance for costs

/*var locations = [[0, 0], [0, 1], [0, 2], [0, 3],
                 [1, 0], [1, 1], [1, 2], [1, 3],
                 [2, 0], [2, 1], [2, 2], [2, 3],
                 [3, 0], [3, 1], [3, 2], [3, 3]];
*/


function manhattanDistance(lhs, rhs) {
  return Math.abs(lhs[0] - rhs[0]) + Math.abs(lhs[1] - rhs[1]);
}

/*var costMatrix = new Array(locations.length);

for (var from = 0; from < locations.length; ++from) {
  costMatrix[from] = new Array(locations.length);

  for (var to = 0; to < locations.length; ++to) {
    costMatrix[from][to] = manhattanDistance(locations[from], locations[to]);
  }
}
*/
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


/*var durationMatrix = new Array(locations.length);

for (var from = 0; from < locations.length; ++from) {
  durationMatrix[from] = new Array(locations.length);

  for (var to = 0; to < locations.length; ++to) {
    var serviceTime = Minutes(3);
    var travelTime = Minutes(costMatrix[from][to]);

    durationMatrix[from][to] = serviceTime + travelTime;
  }
}
*/

/*var timeWindows = new Array(locations.length);

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
*/

/*var demandMatrix = new Array(locations.length);

for (var from = 0; from < locations.length; ++from) {
  demandMatrix[from] = new Array(locations.length);

  for (var to = 0; to < locations.length; ++to) {
    if (from === depot)
      demandMatrix[from][to] = 0
    else
      demandMatrix[from][to] = 1
  }
}
*/

// setup timeWindows Array

/*function timeWindowsSetup(jsonContent) {

        
        var timeWindows = new Array(jsonContent.locations.length);
        for(var at = 0; at <jsonContent.locations.length; ++at) {
          if(at===depot) {
          timeWindows[at]=[dayStarts,dayEnds];
          continue;
           }
        var earliest = dayStarts;
        var latest = dayEnds - Hours(1);

        var start = rand() * (latest - earliest) + earliest;
        var stop = rand() * (latest - start) + start;

        timeWindows[at] = [start, stop];
      }
        return timeWindows;
      
}*/

/*var timeWindows = new Array(locations.length);

for (var at = 0; at < locations.length; ++at) {

  timeWindows[at] = new Array(locations.length);
  if (at === depot) {
    
    timeWindows[at][0] = dayStarts;
    timeWindows[at][1] = dayEnds;
    continue;
  }

  var earliest = dayStarts;
  var latest = dayEnds - Hours(1);

  var start = rand() * (latest - earliest) + earliest;
  var stop = rand() * (latest - start) + start;

  timeWindows[at][0] = start; 
  timeWindows[at][1] = stop;
}
*/
// set up distance matrix


 function DistSetup(jsonContent) {
        "use strict";
        //this.init();
        var distanceMatrix;
        var tempickUpDropId = [];
        
        var i = 0,
            j = 0,
            k = 0;
    
        tempickUpDropId.push("lat" + jsonContent.hubs[0].depot.lat + "lng" + jsonContent.hubs[0].depot.lng);
        
        for (i = 0; i < jsonContent.orders.length; i++) {
            tempickUpDropId.push("lat" + jsonContent.orders[i].drop.lat + "lng" + jsonContent.orders[i].drop.lng);
        }

 //       var pickUpDropId = tempickUpDropId.filter(function (elem, pos) {
 //           return tempickUpDropId.indexOf(elem) === pos;
 //       });

        var pickUpDropId = tempickUpDropId;
        
        function storeMatrix(locations) {
            var arr = [];
            var i;
            for (i = 0; i < locations; i++) {
                arr[i] = [];
            }
            return arr;
        }

        var distanceMatrix = storeMatrix(jsonContent.locations.length);

        
        
        for (j = 0; j < jsonContent.locations.length; j++) {
            for (k = 0; k < jsonContent.locations.length; k++) {
                distanceMatrix[j][k] = Number(jsonContent.matrix[pickUpDropId[j]][pickUpDropId[k]].d);
            }
        }
        return distanceMatrix;
    //console.log(distanceMatrix);
                
    
    /*distMat: function (num1, num2) {
        "use strict";
        return this.params.distanceMatrix[num1][num2];
    },
    
    costDisplay : function () {
        "use strict";
        return this.params.distanceMatrix;
    }*/
  };



// setup duration matrix


 function DurationSetup(jsonContent) {
        "use strict";
        //this.init();
        var tempickUpDropId = [];
        
        var i = 0,
            j = 0,
            k = 0;
    
        tempickUpDropId.push("lat" + jsonContent.hubs[0].depot.lat + "lng" + jsonContent.hubs[0].depot.lng);
        
        for (i = 0; i < jsonContent.orders.length; i++) {
            tempickUpDropId.push("lat" + jsonContent.orders[i].drop.lat + "lng" + jsonContent.orders[i].drop.lng);
        }
    /*
        var pickUpDropId = tempickUpDropId.filter(function (elem, pos) {
            return tempickUpDropId.indexOf(elem) === pos;
        });*/

    var pickUpDropId = tempickUpDropId;

        function storeMatrix(locations) {
            var arr = [];
            var i;
            for (i = 0; i < locations; i++) {
                arr[i] = [];
            }
            return arr;
        }

        var durationMatrix = storeMatrix(jsonContent.locations.length);

        
        
        for (j = 0; j < jsonContent.locations.length; j++) {
            for (k = 0; k < jsonContent.locations.length; k++) {
                durationMatrix[j][k] = Number(jsonContent.matrix[pickUpDropId[j]][pickUpDropId[k]].t);
            }
        }
    

    return durationMatrix;
    /*durMat: function (num1, num2) {
        "use strict";
        return this.params.durationMatrix[num1][num2];
    },
    
    durDisplay : function () {
        "use strict";
        return this.params.durationMatrix;
    }*/
  
    };

var depot = 0;
//our input processed from here

var fs = require("fs");
var payload,final;

    //console.log(JSON.stringify(event, null, 2));
        payload = fs.readFileSync("inputfiles/10.json");
           
        var jsonContent = JSON.parse(payload);
        //console.log("jdata is "+jsonContent);
        //for(var i =0 ; i< 1000 ; i++){
        /*var orderList = require('../..lamdafunction/Algorithms/SinglePickupMultipleDrop/WMDepottoMultipleDrop');
            orderList.setup(jsonContent);

            //WMDrop ID reades lat long of different locations
            var ID = require('../..lamdafunction/Algorithms/SinglePickupMultipleDrop/WMDropID');
            ID.setup(jsonContent);
        */
            //WMCostMatrix is basically Distance Matrix
            var distanceMatrix = new Array(jsonContent.locations.length);
            distanceMatrix = DistSetup(jsonContent);
            

            var durationMatrix = new Array(jsonContent.locations.length);
            durationMatrix = DurationSetup(jsonContent);
            
            
            var timeWindows = new Array(jsonContent.locations.length);

            for (var at = 0; at < jsonContent.locations.length; ++at) {
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


            //WMCWTranslationsNew is basically the brain which intructs different parts of algo body to perform specific tasks
            //var start = new Date().getTime();

            var demandMatrix = new Array(jsonContent.locations.length);

            for (var from = 0; from < jsonContent.locations.length; ++from) {
              demandMatrix[from] = new Array(jsonContent.locations.length);

              for (var to = 0; to < jsonContent.locations.length; ++to) {
                if (from === depot)
                  demandMatrix[from][to] = 0
                else
                  demandMatrix[from][to] = 1
              }
            }

            



//input upto here


tap.test('Test VRP', function(assert) {





  var solverOpts = {
    numNodes: jsonContent.locations.length,  // ID.length    or   jsonContent.locations.length  ok
    costs: distanceMatrix,          //distanceMatrix    ok  
    durations: durationMatrix, //durationMatrix    ok   
    timeWindows: timeWindows,  // to be fixed by us ??????  
    demands: demandMatrix       // ok
  };

  var VRP = new ortools.VRP(solverOpts);

  var numVehicles = Number(jsonContent.drivers.length);
  var timeHorizon = dayEnds-dayStarts;


  //var vehicleCapacity = 10;

  var vehicleCapacity = new Array(numVehicles);
  for (var i = 0; i < numVehicles; i++) {

    if(jsonContent.drivers[i].vehicletype="ace")
    vehicleCapacity[i]=800;
    else if(jsonContent.drivers[i].vehicletype="bike")
    vehicleCapacity[i]=100;
    else if(jsonContent.drivers[i].vehicletype="t407")
    vehicleCapacity[i]=2400;

  }

  // Dummy lock to let vehicle 0 go to location 2 and 3 first - to test route locks
  var routeLocks = new Array(numVehicles);

  for (var vehicle = 0; vehicle < numVehicles; ++vehicle) {
    if (vehicle === 0)
      routeLocks[vehicle] = [];             //
    else
      routeLocks[vehicle] = [];                 // must not contain depots
  }


  //pickups 
  //var pickups = new Array(jsonContent.orders.length);
  // for(var i=0;i<jsonContent.orders.length;i++) {
  //      pickups[i]=1;
  // }
  //Deliveries
  //var deliveries = new Array(jsonContent.orders.length);
  // for(int i=0;i<jsonContent.orders.length;i++) {
  //     /var deliveries[i]=;
  // }


  var searchOpts = {
    computeTimeLimit: 1000,       // ok
    numVehicles: numVehicles,     // ok
    depotNode: depot,             // ok
    timeHorizon: timeHorizon,         //ok
    vehicleCapacity: vehicleCapacity,   //  ok
    routeLocks: routeLocks,             //  ok
    pickups: [1, 1],                   // pickups array 
    deliveries: [4, 6]                  // corresponding deliveries array
  };

  VRP.Solve(searchOpts, function (err, solution) {

    console.log("Solution ------------------------------");
    console.log(JSON.stringify(solution));
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
        assert.ok(u[0] <= u[1], 'Valid Time Window');
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