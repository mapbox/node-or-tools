# node-or-tools

NodeJS or-tools TSP / VRP solver bindings.

Requires `libortools.so` to be installed.
Tested with or-tools v5.1.

### Dependencies

- Grab v5.1.tar.gz source release from https://github.com/google/or-tools/releases
- Follow build instructions https://developers.google.com/optimization/introduction/installing

Note: binary distributions will most likely work, too. Adapt library and include dirs below appropriately.

### Quick Start

    npm install \
        --OR_TOOLS_LIBRARY_DIR=/home/daniel/node-or-tools/third_party/or-tools-5.1/lib \
        --OR_TOOLS_INCLUDE_DIR=/home/daniel/node-or-tools/third_party/or-tools-5.1/src

    npm test


### License

Copyright © 2017 Mapbox

Distributed under the MIT License (MIT).
