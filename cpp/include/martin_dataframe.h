#ifndef MIKU_TEMPLAR_MARTIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_MARTIN_DATA_FRAME_H_

#include <vector>
#include <string>
#include "origin_dataframe.h"

namespace MikuTemplar{

enum Operation{
    BUY, SELL
};

enum CloseType{
    NOT_CLOSE,    // martin group is not closed yet
    STOP_PROFIT,  // close all position with profit
    STOP_LOSS,    // close all position with loss
    STOP_EARLY    // no more data avaiable so it is an early stop
};

typedef std::vector<std::size_t> AddPositions;
struct MartinResult {
    Operation op_;
    CloseType closeType_;
    std::size_t closeArrayIndex_;
    AddPositions addPositions_;
};
template <class T>
struct MartinDataFrame : public OriginDataFrame<T> {
    std::vector<T> martinPositionIntervals_;
    std::vector<T> martinProfitTargets_;
    T martinStopLossTarget_;

    // Following variables have exact the same size of size()
    std::vector<Operation> operation_;
    std::vector<CloseType> closeType_;
    std::vector<std::size_t> closeArrayIndex_;
    std::vector<AddPositions> addPositions_;

    void appendMartinResult(const MartinResult &martinResult) {
        operation_.push_back(martinResult.op_);
        closeType_.push_back(martinResult.closeType_);
        closeArrayIndex_.push_back(martinResult.closeArrayIndex_);
        addPositions_.push_back(martinResult.addPositions_);
    }
};
}

#endif