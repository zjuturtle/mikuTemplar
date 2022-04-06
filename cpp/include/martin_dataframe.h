#ifndef MIKU_TEMPLAR_MARTIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_MARTIN_DATA_FRAME_H_

#include <vector>
#include <string>
#include "origin_dataframe.h"

namespace MikuTemplar{

enum Operation{
    BUY, SELL
};

Operation GenerateOperation(const std::string &input) {
    if (input == "buy" || input == "BUY") {
        return Operation::BUY;
    }
    if (input == "sell" || input == "SELL") {
        return Operation::SELL;
    }
    std::cout << "ERROR! invalid input for operation!";
    return Operation::BUY;
}

enum CloseType{
    NOT_CLOSE,    // martin group is not closed yet
    STOP_PROFIT,  // close all position with profit
    STOP_LOSS,    // close all position with loss
    STOP_EARLY    // no more data avaiable so it is an early stop
};

typedef std::vector<std::size_t> ArrayIndexList;

struct MartinResult {
    Operation op_;
    CloseType closeType_;
    std::size_t closeArrayIndex_;
    ArrayIndexList addPositionsArrayIndex_;
};
template <class T>
struct MartinDataFrame : public OriginDataFrame<T> {
    std::vector<T> addPositionIntervals_;
    std::vector<T> stopProfitTargets_;
    T stopLossTarget_;

    // Following variables have exact the same size of size()
    std::vector<Operation> operation_;
    std::vector<CloseType> closeType_;
    std::vector<std::size_t> closeArrayIndex_;
    std::vector<ArrayIndexList> addPositionsArrayIndex_;

    MartinDataFrame(const std::vector<T> &addPositionIntervals,
                    const std::vector<T> &stopProfitTargets,
                    const T stopLossTarget) : 
                    addPositionIntervals_(addPositionIntervals), 
                    stopProfitTargets_(stopProfitTargets),
                    stopLossTarget_(stopLossTarget){}

    void appendMartinResult(const MartinResult &martinResult) {
        operation_.push_back(martinResult.op_);
        closeType_.push_back(martinResult.closeType_);
        closeArrayIndex_.push_back(martinResult.closeArrayIndex_);
        addPositions_.push_back(martinResult.addPositions_);
    }
};
}

#endif