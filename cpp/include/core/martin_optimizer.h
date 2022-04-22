#ifndef MIKU_TEMPLAR_CORE_MARTIN_OPTIMIZER_H_
#define MIKU_TEMPLAR_CORE_MARTIN_OPTIMIZER_H_

#include "core/martin_parameters.h"
#include "core/martin_info.h"
#include "ortools/linear_solver/linear_solver.h"

namespace MikuTemplar{
class MartinOptimizer{
public:
    MartinOptimizer() {}
    MartinStatistics optimize(const MartinParameters &mp, const MartinCounts &mc) const {
        MartinStatistics res;
        res.stopLossPossibility_ = ((double)mc.stopLossCount_) / ((double)(mc.allCount_ - mc.earlyStopCount_));  
        return res;
    }
};
}
#endif