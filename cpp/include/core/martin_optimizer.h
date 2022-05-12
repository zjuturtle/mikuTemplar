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
        
        // Optimize lots
        std::unique_ptr<operations_research::MPSolver> solver(operations_research::MPSolver::CreateSolver("SCIP"));
        const double infinity = solver->infinity();
        std::vector<operations_research::MPVariable*> lots;

        for (int i=0;i<mp.positionIntervals_.size();i++) {
            operations_research::MPVariable* lot_i = solver->MakeIntVar(0, infinity, "lot_"+std::to_string(i));
            lots.push_back(lot_i);
        }

        // Constraint: \sum{lot_i} * minUnitLot = totalLot
        operations_research::MPConstraint* sum = solver->MakeRowConstraint(mp.totalLot_, mp.totalLot_);
        for (int i=0;i<mp.positionIntervals_.size();i++) {
            sum->SetCoefficient(lots[i], mp.minLotUnit_);
        }

        // Constraint: stop profit should have gain
        for (int i=1;i<lots.size();i++) {
            operations_research::MPConstraint* c = solver->MakeRowConstraint(mp.minProfit_, infinity);
            for (int j=0;j<=i;j++) {
                auto coeff = mp.stopProfits_[i] - mp.positionIntervals_[i] + mp.positionIntervals_[j];
                c->SetCoefficient(lots[j], mp.minLotUnit_ * coeff);
            }
        }

        // Constraint: lots_(n-1) <= lots_(n)/minFactor
        for (int i=1;i<lots.size();i++) {
            operations_research::MPConstraint* c = solver->MakeRowConstraint(0, infinity);
            c->SetCoefficient(lots[i-1], -1);
            c->SetCoefficient(lots[i], 1./mp.minFactor_);
        }

        // Object function
        auto objective = solver->MutableObjective();
        for (int i=0;i<lots.size();i++) {
            double profitCoef = 0.0;
            double lossCoef = 0.0;
            for (int j=i;j<res.stopProfitPossibility_.size();j++) {
                profitCoef += (res.stopProfitPossibility_[j] * (mp.stopProfits_[j]-mp.positionIntervals_[j] + mp.positionIntervals_[i]));
            }
            lossCoef = res.stopLossPossibility_ * (mp.positionIntervals_[i] - mp.stopLoss_);
            objective->SetCoefficient(lots[i], mp.minLotUnit_ * (profitCoef + lossCoef));
        }

        // Solve problem
        objective->SetMaximization();
        auto result_status = solver->Solve();
        if (result_status != operations_research::MPSolver::OPTIMAL) {
            std::cout << "[WARN]Optimizer does not have an optimal solution!" << std::endl;
        }

        // Write to result
        for (auto lot : lots) {
            res.bestLots_.push_back(lot->solution_value() * mp.minLotUnit_);
        }
        res.bestProfit_ = objective->Value();
        return res;
    }
};
}
#endif