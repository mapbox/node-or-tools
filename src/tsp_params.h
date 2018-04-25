#ifndef NODE_OR_TOOLS_TSP_PARAMS_D4D3AF3A298F_H
#define NODE_OR_TOOLS_TSP_PARAMS_D4D3AF3A298F_H

#include <nan.h>

#include <stdexcept>

#include "params.h"

struct TSPSolverParams {
    TSPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

    std::int32_t numNodes;
    CostMatrix costs;
};

struct TSPSearchParams {
    TSPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info);

    std::int32_t computeTimeLimit;
    std::int32_t depotNode;

    v8::Local<v8::Function> callback;
};

// Impl.

TSPSolverParams::TSPSolverParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() != 1 || !info[0]->IsObject())
        throw std::runtime_error{"Single object argument expected: SolverOptions"};

    auto opts = info[0].As<v8::Object>();

    auto maybeNumNodes = Nan::Get(opts, Nan::New("numNodes").ToLocalChecked());
    auto maybeCostMatrix = Nan::Get(opts, Nan::New("costs").ToLocalChecked());

    auto numNodesOk = !maybeNumNodes.IsEmpty() && maybeNumNodes.ToLocalChecked()->IsNumber();
    auto costMatrixOk = !maybeCostMatrix.IsEmpty() && maybeCostMatrix.ToLocalChecked()->IsArray();

    if (!numNodesOk || !costMatrixOk)
        throw std::runtime_error{"SolverOptions expects 'numNodes' (Number), 'costs' (Array)"};

    numNodes = Nan::To<std::int32_t>(maybeNumNodes.ToLocalChecked()).FromJust();

    auto costMatrix = maybeCostMatrix.ToLocalChecked().As<v8::Array>();
    costs = makeMatrixFrom2dArray<CostMatrix>(numNodes, costMatrix);
}

TSPSearchParams::TSPSearchParams(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() != 2 || !info[0]->IsObject() || !info[1]->IsFunction())
        throw std::runtime_error{"Two arguments expected: SearchOptions (Object) and callback (Function)"};

    auto opts = info[0].As<v8::Object>();

    auto maybeComputeTimeLimit = Nan::Get(opts, Nan::New("computeTimeLimit").ToLocalChecked());
    auto maybeDepotNode = Nan::Get(opts, Nan::New("depotNode").ToLocalChecked());

    auto computeTimeLimitOk = !maybeComputeTimeLimit.IsEmpty() && maybeComputeTimeLimit.ToLocalChecked()->IsNumber();
    auto depotNodeOk = !maybeDepotNode.IsEmpty() && maybeDepotNode.ToLocalChecked()->IsNumber();

    if (!computeTimeLimitOk || !depotNodeOk)
        throw std::runtime_error{"SearchOptions expects 'computeTimeLimit' (Number), 'depotNode' (Number)"};

    computeTimeLimit = Nan::To<std::int32_t>(maybeComputeTimeLimit.ToLocalChecked()).FromJust();
    depotNode = Nan::To<std::int32_t>(maybeDepotNode.ToLocalChecked()).FromJust();
    callback = info[1].As<v8::Function>();
}

#endif
