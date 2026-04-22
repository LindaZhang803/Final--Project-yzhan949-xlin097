#ifndef BSP_H
#define BSP_H

#include "graph.h"

class BspAlgorithm {
public:
    virtual ~BspAlgorithm() = default;
    virtual bool HasWork() const = 0;
    virtual void Process(int tid, int v, const CSRGraph& g) = 0;
    virtual void PostRound() = 0;
};

void BspSerial(const CSRGraph& g, BspAlgorithm& algo);
void BspParallel(const CSRGraph& g, BspAlgorithm& algo, int num_threads);

#endif
