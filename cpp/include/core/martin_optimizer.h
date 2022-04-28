#ifndef MIKU_TEMPLAR_CORE_MARTIN_OPTIMIZER_H_
#define MIKU_TEMPLAR_CORE_MARTIN_OPTIMIZER_H_

#include "core/martin_parameters.h"
#include "core/martin_info.h"
#include "ortools/linear_solver/linear_solver.h"
#include <string>

namespace MikuTemplar{
class MartinOptimizer{
public:
    MartinOptimizer() {}
    inline MartinStatistics optimize(const MartinParameters &mp, const MartinCounts &mc) const {
        MartinStatistics res;
        res.stopLossPossibility_ = ((double)mc.stopLossCount_) / ((double)(mc.allCount_ - mc.earlyStopCount_));  
        
        // Optimize weight
        std::unique_ptr<operations_research::MPSolver> solver(operations_research::MPSolver::CreateSolver("SCIP"));
        const double infinity = solver->infinity();
        std::vector<operations_research::MPVariable* const> weights;
        for (int i=0;i<mp.positionIntervals_.size();i++) {
            operations_research::MPVariable* const w_i = solver->MakeNumVar(0.0, 1.0, "w_"+std::to_string(i));
            weights.push_back(w_i);
        }

        for (int i=1;i<mp.positionIntervals_.size();i++) {
            // w_(i-1) - w_(i) >=0
            operations_research::MPConstraint* const c = solver->MakeRowConstraint(-infinity, 0);
            c->SetCoefficient(weights[i-1], 1);
            c->SetCoefficient(weights[i], -1);
        }

        // \sum{w_i} = 1
        operations_research::MPConstraint* const sumToOne = solver->MakeRowConstraint(1.0, 1.0);
        for (int i=0;i<mp.positionIntervals_.size();i++) {
            sumToOne->SetCoefficient(weights[i], 1);
        }

        
        return res;
    }
};
}
#endif