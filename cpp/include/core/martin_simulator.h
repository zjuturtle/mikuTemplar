#ifndef MIKU_TEMPLAR_CORE_MARTIN_SIMULATOR_H_
#define MIKU_TEMPLAR_CORE_MARTIN_SIMULATOR_H_
#include "core/io.h"
#include "core/ext_dataframe.h"
#include "core/martin_dataframe.h"
#include "core/origin_dataframe.h"


namespace MikuTemplar {

class MartinSimulator {

public:
    MartinSimulator(const std::string &extCsvFile, int workerNum=8);
    MartinSimulator(const ExtDataFrame<DATA_TYPE> &extDataFrame, int workerNum=8);

    /**
     * @brief Search every hit in extDataFrame by given OriginDataFrame
     * 
     * @param openOriginDataFrame 
     * @return vector<size_t> 
     */
    std::vector<std::size_t> locateOpenArrayIndex(const OriginDataFrame<DATA_TYPE> &openOriginDataFrame) const;

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
    MartinResult run(
        const size_t openArrayIndex,
        const Operation &op,
        const std::vector<DATA_TYPE> &martinPositionIntervals,
        const std::vector<DATA_TYPE> &martinStopProfitTargets,
        const DATA_TYPE &martinStopLossTarget) const;

    MartinDataFrame<DATA_TYPE> run(
        const OriginDataFrame<DATA_TYPE> &openOriginDataFrame,
        const Operation &op,
        const std::vector<DATA_TYPE> &martinPositionIntervals,
        const std::vector<DATA_TYPE> &martinStopProfitTargets,
        const DATA_TYPE &martinStopLossTarget) const;

    MartinDataFrame<DATA_TYPE> run(
        const std::vector<std::size_t> &openArrayIndexList,
        const Operation &op,
        const std::vector<DATA_TYPE> &martinPositionIntervals,
        const std::vector<DATA_TYPE> &martinStopProfitTargets,
        const DATA_TYPE &martinStopLossTarget) const;

private:
    ExtDataFrame<DATA_TYPE> extDataFrame_;
    int workerNum_;
    bool couldSkipWindow(const Operation op,
                     bool stopLossFlag,
                     const DATA_TYPE stopLossPrice,
                     const DATA_TYPE stopProfitPrice,
                     const DATA_TYPE addPositionPrice,
                     const DATA_TYPE futureBidMax, 
                     const DATA_TYPE futureBidMin,
                     const DATA_TYPE futureAskMax,
                     const DATA_TYPE futureAskMin) const;
};

}

#endif