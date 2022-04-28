#ifndef MIKU_TEMPLAR_CORE_MARTIN_INFO_H_
#define MIKU_TEMPLAR_CORE_MARTIN_INFO_H_
#include<vector>
#include "core/martin_parameters.h"

namespace MikuTemplar{

struct MartinCounts {
    std::size_t allCount_=0;
    std::size_t stopLossCount_ = 0;
    std::size_t earlyStopCount_ = 0;
    std::vector<int> stopProfitsCount_;
};

struct MartinStatistics {
    double stopLossPossibility_;
    std::vector<double> stopProfitPossibility_;
    std::vector<double> expectedBestAddPositionWeight_;
    double expectedBestProfit_;
};

struct MartinInfo {
    MartinParameters p_;
    MartinCounts c_;
    MartinStatistics s_;
};
}

#endif