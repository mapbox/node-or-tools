#include "tsp.h"
#include "vrp.h"

NAN_MODULE_INIT(Init) {
    TSP::Init(target);
    VRP::Init(target);
}

NODE_MODULE(node_or_tools, Init)
