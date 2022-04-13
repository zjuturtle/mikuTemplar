#ifndef MIKU_TEMPLAR_CORE_MARTIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_CORE_MARTIN_DATA_FRAME_H_

#include <string>
#include <vector>

#include "core/martin_parameters.h"
#include "core/martin_statistics.h"
#include "core/operation.h"
#include "core/origin_dataframe.h"

namespace MikuTemplar {

enum CloseType {
    NOT_CLOSE,    // martin group is not closed yet
    STOP_PROFIT,  // close all position with profit
    STOP_LOSS,    // close all position with loss
    STOP_EARLY    // no more data avaiable so it is an early stop
};

inline std::string toString(const CloseType &closeType) {
    switch (closeType) {
        case NOT_CLOSE:
            return "notClose";
        case STOP_PROFIT:
            return "stopProfit";
        case STOP_LOSS:
            return "stopLoss";
        case STOP_EARLY:
            return "stopEarly";
    }
}

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
    Operation operation_;

    // Following variables have exact the same size of size()
    std::vector<Operation> operation_;
    std::vector<CloseType> closeType_;
    std::vector<std::size_t> closeArrayIndex_;
    std::vector<ArrayIndexList> addPositionsArrayIndex_;

    MartinDataFrame(const MartinParameters<T> &mP) : addPositionIntervals_(mP.positionIntervals_),
                                                     stopProfitTargets_(mP.stopProfits_),
                                                     stopLossTarget_(mP.stopLoss_) {}

    MartinDataFrame(const std::vector<T> &addPositionIntervals,
                    const std::vector<T> &stopProfitTargets,
                    const T stopLossTarget) : addPositionIntervals_(addPositionIntervals),
                                              stopProfitTargets_(stopProfitTargets),
                                              stopLossTarget_(stopLossTarget) {}

    void appendMartinResult(const MartinResult &martinResult, const Tick<T> &tick) {
        operation_.push_back(martinResult.op_);
        closeType_.push_back(martinResult.closeType_);
        closeArrayIndex_.push_back(martinResult.closeArrayIndex_);
        addPositionsArrayIndex_.push_back(martinResult.addPositionsArrayIndex_);
        this->append(tick);
    }

    MartinStatistics count() {
        MartinStatistics res;
        res.stopProfitsCount_.resize(stopProfitTargets_.size(), 0);
        for (std::size_t i = 0; i < closeType_.size(); i++) {
            if (closeType_[i] == CloseType::STOP_EARLY) {
                res.earlyStopCount_++;
                continue;
            }
            if (closeType_[i] == CloseType::STOP_LOSS) {
                res.stopLossCount_++;
                continue;
            }
            if (closeType_[i] == CloseType::STOP_PROFIT) {
                res.stopProfitsCount_[addPositionsArrayIndex_[i].size() - 1]++;
                continue;
            }
        }
        return res;
    }
};
}  // namespace MikuTemplar

#endif