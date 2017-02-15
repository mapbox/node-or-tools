# node-or-tools

[![Build Status](https://travis-ci.com/mapbox/node-or-tools.svg?token=hLpUd9oZwpjSs5JzfqFa&branch=master)](https://travis-ci.com/mapbox/node-or-tools)

NodeJS or-tools TSP and VRP solver bindings.

### Installing

    npm install

### Run tests

    npm test

### References

Routing Interfaces
- [RoutingModel](https://github.com/google/or-tools/blob/v5.1/src/constraint_solver/routing.h#L14)
- [RoutingSearchParameters](https://github.com/google/or-tools/blob/master/src/constraint_solver/routing_parameters.proto#L28)
- [RoutingModelParameters](https://github.com/google/or-tools/blob/master/src/constraint_solver/routing_parameters.proto#L263)

More or-tools
- Manual: [TSP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/TSP.html) and [VRP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/VRP.html)
- [Examples](https://github.com/google/or-tools/tree/v5.1/examples/cpp)
- [Reference](https://developers.google.com/optimization/reference/) - Main entry is `RoutingModel` in `constraint_solver/routing.h`

Node bindings
- [NAN2](https://github.com/nodejs/nan#api)
- [GYP](https://gyp.gsrc.io)

### License

Copyright © 2017 Mapbox

Distributed under the MIT License (MIT).
