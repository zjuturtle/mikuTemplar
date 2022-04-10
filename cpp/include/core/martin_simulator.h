#ifndef MIKU_TEMPLAR_MARTIN_SIMULATOR_H_
#define MIKU_TEMPLAR_MARTIN_SIMULATOR_H_
#include "io.h"
#include "ext_dataframe.h"
#include "martin_dataframe.h"
#include "origin_dataframe.h"
#include "thread_pool.hpp"

namespace MikuTemplar {

class MartinSimulator {

public:
    MartinSimulator(const std::string &extCsvFile);
    MartinSimulator(const ExtDataFrame<DATA_TYPE> &extDataFrame);

    /**
     * @brief Simulate to get the martin result of a specific open tick
     * 
     * @param extDataFrame 
     * @param openArrayIndex 
     * @param op 
     * @param martinPositionIntervals 
     * @param martinProfitTargets 
     * @return MartinResult 
     */
    MartinResult martinSim(
        const size_t openArrayIndex,
        const Operation &op,
        const std::vector<DATA_TYPE> &martinPositionIntervals,
        const std::vector<DATA_TYPE> &martinStopProfitTargets,
        const DATA_TYPE &martinStopLossTarget) const;

    MartinDataFrame<DATA_TYPE> martinSim(
        const std::vector<std::size_t> &openArrayIndexList,
        const Operation &op,
        const std::vector<DATA_TYPE> &martinPositionIntervals,
        const std::vector<DATA_TYPE> &martinStopProfitTargets,
        const DATA_TYPE &martinStopLossTarget);

private:
    ExtDataFrame<DATA_TYPE> extDataFrame_;

    bool couldSkipWindow(const Operation op,
                     bool stopLossFlag,
                     const DATA_TYPE stopLossPrice,
                     const DATA_TYPE stopProfitPrice,
                     const DATA_TYPE addPositionPrice,
                     const DATA_TYPE futureBidMax, 
                     const DATA_TYPE futureBidMin,
                     const DATA_TYPE futureAskMax,
                     const DATA_TYPE futureAskMin) const;
    thread_pool threadPool;
};

}

#endif