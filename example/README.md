# VRP Example using Mapbox Directions

    npm install

    export MAPBOX_ACCESS_TOKEN='YourAccessToken'
    node index

- Fetches a distance table using the [Mapbox Directions Matrix](https://www.mapbox.com/api-documentation/navigation/#matrix) API
- Runs the VRP solver with some dummy locations and constraints (see `index.js` for details)
- Fetches route geometries using the [Mapbox Directions Route](https://www.mapbox.com/api-documentation/navigation/#directions) API

[![Example](https://raw.githubusercontent.com/mapbox/node-or-tools/master/example/solution.png?token=AAgLiX1m1BDa8ll0Lsk0xc6fz0RgQA1Lks5Y-VmAwA)](https://github.com/mapbox/node-or-tools/blob/master/example/solution.geojson)
