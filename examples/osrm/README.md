# node-osrm + node-or-tools

Prepare road network for routing

```
./node_modules/osrm/lib/binding/osrm-extract -p node_modules/osrm/profiles/car.lua monaco-latest.osm.pbf
./node_modules/osrm/lib/binding/osrm-contract monaco-latest.osrm
```

Run with fixed set of coordinates

```
node index.js
```
