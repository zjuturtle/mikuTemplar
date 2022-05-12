#ifndef MIKU_TEMPLAR_CORE_MARTIN_PARAMETERS_H_
#define MIKU_TEMPLAR_CORE_MARTIN_PARAMETERS_H_
#include <vector>

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
};
}  // namespace MikuTemplar

#endif