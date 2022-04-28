#ifndef MIKU_TEMPLAR_CORE_MARTIN_OPTIMIZER_H_
#define MIKU_TEMPLAR_CORE_MARTIN_OPTIMIZER_H_

#include "core/martin_parameters.h"
#include "core/martin_info.h"
#include "ortools/linear_solver/linear_solver.h"
#include <string>
#include <cmath>

namespace MikuTemplar{
class MartinOptimizer{
public:
    MartinOptimizer() {}
    inline MartinStatistics optimize(const MartinParameters &mp, const MartinCounts &mc) const {
        MartinStatistics res;
        res.stopLossPossibility_ = ((double)mc.stopLossCount_) / ((double)(mc.allCount_ - mc.earlyStopCount_));
        for (int i=0;i<mc.stopProfitsCount_.size();i++) {
            res.stopProfitPossibility_.push_back(((double)mc.stopProfitsCount_[i]) / ((double)(mc.allCount_ - mc.earlyStopCount_)));
        }
        
        // Optimize weight
        std::unique_ptr<operations_research::MPSolver> solver(operations_research::MPSolver::CreateSolver("SCIP"));
        const double infinity = solver->infinity();
        std::vector<operations_research::MPVariable*> weights;
        for (int i=0;i<mp.positionIntervals_.size();i++) {
            operations_research::MPVariable* w_i = solver->MakeNumVar(0.0, 1.0, "w_"+std::to_string(i));
            weights.push_back(w_i);
        }

        // Constraint: w_(i-1) - w_(i) >=0
        for (int i=1;i<mp.positionIntervals_.size();i++) {
            operations_research::MPConstraint* c = solver->MakeRowConstraint(-infinity, 0);
            c->SetCoefficient(weights[i-1], 1);
            c->SetCoefficient(weights[i], -1);
        }

        // Constraint: \sum{w_i} = 1
        operations_research::MPConstraint* sumToOne = solver->MakeRowConstraint(1.0, 1.0);
        for (int i=0;i<mp.positionIntervals_.size();i++) {
            sumToOne->SetCoefficient(weights[i], 1);
        }

        // Constraint: w_0 >= 1/(2^N) N is mp.positionIntervals_.size()
        operations_research::MPConstraint* minimal = solver->MakeRowConstraint(1./std::pow(2, mp.positionIntervals_.size()), 1);
        minimal->SetCoefficient(weights[0], 1);

        // Object function
        auto objective = solver->MutableObjective();
        for (int i=0;i<weights.size();i++) {
            double profitCoef, lossCoef = 0.0;
            for (int j=0;j<res.stopProfitPossibility_.size();j++) {
                profitCoef += (res.stopProfitPossibility_[j] * (mp.stopProfits_[j]-mp.positionIntervals_[j] + mp.positionIntervals_[i]));
            }
            lossCoef = res.stopLossPossibility_ * (mp.positionIntervals_[i] - mp.stopLoss_);
            objective->SetCoefficient(weights[i], profitCoef + lossCoef);
        }

        // Solve problem
        objective->SetMaximization();
        auto result_status = solver->Solve();
        if (result_status != operations_research::MPSolver::OPTIMAL) {
            std::cout << "[WARN]Optimizer does not have an optimal solution!" << std::endl;
        }

        // Write to result
        for (auto w : weights) {
            res.expectedBestAddPositionWeight_.push_back(w->solution_value());
        }
        res.expectedBestProfit_ = objective->Value();
        return res;
    }
};
}
#endif