# node-or-tools

[![Build Status](https://travis-ci.com/mapbox/node-or-tools.svg?token=hLpUd9oZwpjSs5JzfqFa&branch=master)](https://travis-ci.com/mapbox/node-or-tools)

NodeJS or-tools TSP / VRP solver bindings.

### Depends

 - c++11 capable compile
 - node-gyp deps

### Installing

    npm install

### Run tests

    npm test

### References

- [NAN2](https://github.com/nodejs/nan#api)
- [GYP](https://gyp.gsrc.io)
- Manual: [TSP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/TSP.html) / [VRP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/VRP.html)
- [Examples](https://github.com/google/or-tools/tree/v5.1/examples/cpp)
- [Reference](https://developers.google.com/optimization/reference/) (Main entry: `constraint_solver/routing.h`)
- Interface [routing.h](https://github.com/google/or-tools/blob/v5.1/src/constraint_solver/routing.h#L14)

### License

Copyright © 2017 Mapbox

Distributed under the MIT License (MIT).
