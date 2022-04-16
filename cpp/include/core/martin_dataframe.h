#ifndef MIKU_TEMPLAR_CORE_MARTIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_CORE_MARTIN_DATA_FRAME_H_

#include <string>
#include <vector>
#include <numeric>

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
    CloseType closeType_;
    std::size_t closeArrayIndex_;
    ArrayIndexList addPositionsArrayIndex_;
};
template <class T>
struct MartinDataFrame : public OriginDataFrame<T> {
    MartinParameters<T> martinParameters_;

    // Following variables have exact the same size of size()
    std::vector<CloseType> closeType_;
    std::vector<std::size_t> closeArrayIndex_;
    std::vector<ArrayIndexList> addPositionsArrayIndex_;

    MartinDataFrame() {}
    MartinDataFrame(const MartinParameters<T> &mP) : martinParameters_(mP) {}

    void appendMartinResult(const MartinResult &martinResult, const Tick<T> &tick) {
        closeType_.push_back(martinResult.closeType_);
        closeArrayIndex_.push_back(martinResult.closeArrayIndex_);
        addPositionsArrayIndex_.push_back(martinResult.addPositionsArrayIndex_);
        this->append(tick);
    }

    MartinStatistics count() {
        MartinStatistics res;
        res.stopProfitsCount_.resize(martinParameters_.stopProfits_.size(), 0);
        res.allCount_ = closeType_.size();
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

        res.stopLossPossibility_ = ((double)res.stopLossCount_) / ((double)res.allCount_);
        return res;
    }
};
}  // namespace MikuTemplar

#endif