#include "vrp.h"
#include "vrp_params.h"
#include "vrp_worker.h"

VRP::VRP(CostMatrix costs_, DurationMatrix durations_, TimeWindows timeWindows_, DemandMatrix demands_)
    : costs{std::make_shared<const CostMatrix>(std::move(costs_))},
      durations{std::make_shared<const DurationMatrix>(std::move(durations_))},
      timeWindows{std::make_shared<const TimeWindows>(std::move(timeWindows_))},
      demands{std::make_shared<const DemandMatrix>(std::move(demands_))} {}

NAN_MODULE_INIT(VRP::Init) {
    const auto whoami = Nan::New("VRP").ToLocalChecked();

    auto fnTp = Nan::New<v8::FunctionTemplate>(New);
    fnTp->SetClassName(whoami);
    fnTp->InstanceTemplate()->SetInternalFieldCount(1);

    SetPrototypeMethod(fnTp, "Solve", Solve);

    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
    constructor().Reset(fn);

    Nan::Set(target, whoami, fn);
}

NAN_METHOD(VRP::New)
try {
    // Handle `new T()` as well as `T()`
    if (!info.IsConstructCall()) {
        auto init = Nan::New(constructor());
        info.GetReturnValue().Set(init->NewInstance());
        return;
    }

    VRPSolverParams userParams{info};

    const auto bytesChange = getBytes(userParams.costs)         //
                             + getBytes(userParams.durations)   //
                             + getBytes(userParams.timeWindows) //
                             + getBytes(userParams.demands);    //

    Nan::AdjustExternalMemory(bytesChange);

    auto* self = new VRP{std::move(userParams.costs),       //
                         std::move(userParams.durations),   //
                         std::move(userParams.timeWindows), //
                         std::move(userParams.demands)};    //

    self->Wrap(info.This());

    info.GetReturnValue().Set(info.This());

} catch (const std::exception& e) {
    return Nan::ThrowError(e.what());
}

NAN_METHOD(VRP::Solve)
try {
    auto* const self = Nan::ObjectWrap::Unwrap<VRP>(info.Holder());

    VRPSearchParams userParams(info);

    const auto bytesChange = getBytes(userParams.routeLocks);
    Nan::AdjustExternalMemory(bytesChange);

    // See routing_parameters.proto and routing_enums.proto
    auto modelParams = RoutingModel::DefaultModelParameters();
    auto searchParams = RoutingModel::DefaultSearchParameters();

    auto firstSolutionStrategy = FirstSolutionStrategy::AUTOMATIC;
    auto metaHeuristic = LocalSearchMetaheuristic::AUTOMATIC;

    searchParams.set_first_solution_strategy(firstSolutionStrategy);
    searchParams.set_local_search_metaheuristic(metaHeuristic);
    searchParams.set_time_limit_ms(userParams.computeTimeLimit);

    // As long as we have a homogeneous fleet wrt. costs we can simplify the underlying model
    modelParams.set_reduce_vehicle_cost_model(true);

    // Do not cache callbacks internally, too: we already provide efficient matrix adaptors
    modelParams.set_max_callback_cache_size(0);

    const std::int32_t numNodes = self->costs->dim();
    const std::int32_t numVehicles = userParams.numVehicles;

    // TODO: this is getting out of hand, clean up, e.g. split into data vs. config
    auto* worker = new VRPWorker{self->costs,                            //
                                 self->durations,                        //
                                 self->timeWindows,                      //
                                 self->demands,                          //
                                 new Nan::Callback{userParams.callback}, //
                                 modelParams,                            //
                                 searchParams,                           //
                                 numNodes,                               //
                                 numVehicles,                            //
                                 userParams.depotNode,                   //
                                 userParams.timeHorizon,                 //
                                 userParams.vehicleCapacity,             //
                                 std::move(userParams.routeLocks),       //
                                 std::move(userParams.pickups),          //
                                 std::move(userParams.deliveries)};      //

    Nan::AsyncQueueWorker(worker);

} catch (const std::exception& e) {
    return Nan::ThrowError(e.what());
}

Nan::Persistent<v8::Function>& VRP::constructor() {
    static Nan::Persistent<v8::Function> init;
    return init;
}
