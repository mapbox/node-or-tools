'use strict';

var node_or_tools = require('.');

if (require.main === module) {
  console.log('self-testing module');

  var TSP = new node_or_tools.TSP();
  var VRP = new node_or_tools.VRP();

  console.log(TSP.Solve());
  console.log(VRP.Solve());
}

module.exports = node_or_tools;
