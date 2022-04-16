#ifndef MIKU_TEMPLAR_CORE_MARTIN_STATISTICS_H_
#define MIKU_TEMPLAR_CORE_MARTIN_STATISTICS_H_
#include<vector>
namespace MikuTemplar{

struct MartinStatistics {
    std::size_t allCount_=0;
    std::size_t stopLossCount_ = 0;
    std::size_t earlyStopCount_ = 0;
    std::vector<int> stopProfitsCount_; 

    double stopLossPossibility_;
    std::vector<double> expectedBestAddPositionWeight_;
    std::vector<double> expectedBestProfit_;
};
}

#endif