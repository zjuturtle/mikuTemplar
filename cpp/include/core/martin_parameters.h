#ifndef MIKU_TEMPLAR_CORE_MARTIN_PARAMETERS_H_
#define MIKU_TEMPLAR_CORE_MARTIN_PARAMETERS_H_
#include <vector>
#include <iostream>

#include "core/const.h"
#include "core/operation.h"

namespace MikuTemplar {

struct MartinParameters {
    std::vector<int> positionIntervals_;
    std::vector<int> stopProfits_;
    double minLotUnit_;
    double totalLot_;
    double minFactor_;
    double minProfit_;
    int stopLoss_;
    Operation op_;

    void validCheck(){
        if (positionIntervals_.size() != stopProfits_.size()) {
            std::cout << "[ERROR]positionIntervals should be exactly the same size as stopProfits!" << std::endl;
        }
        if (positionIntervals_[0] != 0) {
            std::cout << "[ERROR]positionIntervals[0] should be 0!" << std::endl;
        }
        if (stopLoss_ < positionIntervals_[positionIntervals_.size()-1]) {
            std::cout << "[ERROR]stop loss should be larger than the last positionIntervals!" << std::endl;
        }
    }
};
}  // namespace MikuTemplar

#endif