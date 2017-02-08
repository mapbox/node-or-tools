# node-or-tools

NodeJS or-tools TSP / VRP solver bindings.

Requires `libortools.so` to be installed.
Tested with or-tools v5.1.

### Dependencies

- Grab v5.1.tar.gz source release from https://github.com/google/or-tools/releases
- Follow build instructions https://developers.google.com/optimization/introduction/installing

Note: binary distributions will most likely work, too. Adapt library and include dirs below appropriately.

### Quick Start

    env npm install \
        --jobs=$(nproc)
        --OR_TOOLS_LIBRARY_DIR=/home/daniel/node-or-tools/third_party/or-tools-5.1/lib \
        --OR_TOOLS_INCLUDE_DIR=/home/daniel/node-or-tools/third_party/or-tools-5.1/src

    export LD_LIBRARY_PATH="/home/daniel/node-or-tools/third_party/or-tools-5.1/lib:$LD_LIBRARY_PATH"

    npm test


### References

- [NAN2](https://github.com/nodejs/nan#api)
- [GYP](https://gyp.gsrc.io)
- Manual: [TSP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/TSP.html) / [VRP](https://acrogenesis.com/or-tools/documentation/user_manual/manual/VRP.html)
- [Examples](https://github.com/google/or-tools/tree/v5.1/examples/cpp)
- [Reference](https://developers.google.com/optimization/reference/) (Main entry: `constraint_solveri/routing.h`)
- Interface [routing.h](https://github.com/google/or-tools/blob/v5.1/src/constraint_solver/routing.h#L14)

### License

Copyright © 2017 Mapbox

Distributed under the MIT License (MIT).
