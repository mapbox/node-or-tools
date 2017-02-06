'use strict';

var node_or_tools = require('.');
var assert = require('assert');
var util = require('util')

if (require.main === module) {
  console.log('self-testing module');

  var solver = new node_or_tools.Solver();
}

module.exports = node_or_tools;
