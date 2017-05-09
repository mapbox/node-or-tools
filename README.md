# node-or-tools

[![Build Status](https://travis-ci.org/mapbox/node-or-tools.svg?branch=master)](https://travis-ci.org/mapbox/node-or-tools)

NodeJS bindings for or-tools Travelling Salesman Problem (TSP) and Vehicle Routing Problem (VRP) solvers.

See [API.md](API.md) for documentation.

Solving TSP and VRP problems always starts out with having a `m x m` cost matrix for all pairwise routes between all locations.
We recommend using the [Mapbox Directions Matrix](https://www.mapbox.com/api-documentation/#directions-matrix) service when optimizing travel times.

[![Example](https://raw.githubusercontent.com/mapbox/node-or-tools/master/example/solution.png?token=AAgLiX1m1BDa8ll0Lsk0xc6fz0RgQA1Lks5Y-VmAwA)](https://github.com/mapbox/node-or-tools/blob/master/example/solution.geojson)

### Quick Start

    npm install node_or_tools

```c++
var ortools = require('node_or_tools')

var VRP = new ortools.VRP(solverOpts);

VRP.Solve(searchOpts, function (err, solution) {
  // ..
});
```

See [API.md](API.md) for interface documentation and [the example](./example/README.md) for a small self-contained example.

We ship pre-built native binaries (for Node.js LTS 4 and 6 on Linux and macOS).
You will need a compatible C++ stdlib, see below if you encounter issues.
Building from source is supported via the `--build-from-source` flag.

#### Ubuntu 16.04

You're fine. The system's stdlib is recent enough.

#### Ubuntu 14.04

```
apt install software-properties-common
add-apt-repository ppa:ubuntu-toolchain-r/test
apt update
apt install libstdc++-5-dev
```

### Tests

    npm test


### Building - Undefined Symbols

If your C++ compiler and stdlib are quite recent they will default to a new ABI.
Mason packages are still built against an old ABI.
If you see `undefined symbols` errors force the stdlib to use the old ABI by setting:

    export CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0"

and re-build the project.

### Releases

- Push commit with `[publish binary]` on master
- Wait for Travis to build and publish binaries, check the AWS bucket if in doubt
- Tag the release `git tag vx.y.z -a` on master, `git push origin vx.y.z`
- Then `npm login`, `npm publish` to npm
- Make Github Release for tag

### References

Routing Interfaces
- [RoutingModel](https://github.com/google/or-tools/blob/v5.1/src/constraint_solver/routing.h#L14)
- [RoutingSearchParameters](https://github.com/google/or-tools/blob/v5.1/src/constraint_solver/routing_parameters.proto#L28-L29)
- [RoutingModelParameters](https://github.com/google/or-tools/blob/v5.1/src/constraint_solver/routing_parameters.proto#L263-L264)

More or-tools
- Manual: [TSP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/TSP.html) and [VRP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/VRP.html)
- [Examples](https://github.com/google/or-tools/tree/v5.1/examples/cpp)
- [Reference](https://developers.google.com/optimization/reference/)

Tests
- [npmjs.com/package/tap](https://www.npmjs.com/package/tap)
- [node-tap.org](http://www.node-tap.org)

Node bindings
- [NAN2](https://github.com/nodejs/nan#api)
- [GYP](https://gyp.gsrc.io)

### License

Copyright © 2017 Mapbox

Distributed under the MIT License (MIT).
