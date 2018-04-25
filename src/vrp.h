#ifndef NODE_OR_TOOLS_VRP_D6C3BEA8CF7E_H
#define NODE_OR_TOOLS_VRP_D6C3BEA8CF7E_H

#include <nan.h>

#include "adaptors.h"
#include "types.h"

#include <memory>

class VRP : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

  private:
    static NAN_METHOD(New);

    static NAN_METHOD(Solve);

    static Nan::Persistent<v8::Function>& constructor();

    // Wrapped Object

    VRP(CostMatrix costs, DurationMatrix durations, TimeWindows timeWindows, DemandMatrix demands);

    // Non-Copyable
    VRP(const VRP&) = delete;
    VRP& operator=(const VRP&) = delete;

    // Non-Moveable
    VRP(VRP&&) = delete;
    VRP& operator=(VRP&&) = delete;

    // (s, t) arc costs we optimize, e.g. duration or distance.
    std::shared_ptr<const CostMatrix> costs;
    // (s, t) arc travel durations: service time for s plus travel time from s to t.
    std::shared_ptr<const DurationMatrix> durations;
    // Time windows keyed by node for when deliveries are possible at node.
    std::shared_ptr<const TimeWindows> timeWindows;
    // Demands at node s continuing to node t.
    std::shared_ptr<const DemandMatrix> demands;
};

#endif
