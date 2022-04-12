#ifndef MIKU_TEMPLAR_CORE_MARTIN_SIMULATOR_H_
#define MIKU_TEMPLAR_CORE_MARTIN_SIMULATOR_H_
#include "core/ext_dataframe.h"
#include "core/io.h"
#include "core/martin_dataframe.h"
#include "core/origin_dataframe.h"
#include "utils/thread_pool.hpp"

namespace MikuTemplar {

template <class T>
class MartinSimulator {
   public:
    MartinSimulator(const std::string &extCsvFile, int workerNum = 8) {
        extDataFrame_ = loadExtCsv<DATA_TYPE>(extCsvFile);
        workerNum_ = workerNum;
    }
    MartinSimulator(const ExtDataFrame<T> &extDataFrame, int workerNum = 8) {
        extDataFrame_ = extDataFrame;
        workerNum_ = workerNum;
    }

    /**
     * @brief Search every hit in extDataFrame by given OriginDataFrame
     *
     * @param openOriginDataFrame
     * @return vector<size_t>
     */
    std::vector<std::size_t> locateOpenArrayIndex(const OriginDataFrame<T> &openOriginDataFrame) const {
        std::vector<size_t> result;
        std::size_t extArrayIndex = 0;
        for (auto it = openOriginDataFrame.index_.cbegin(); it != openOriginDataFrame.index_.cend(); it++) {
            while (true) {
                if (extDataFrame_.index_[extArrayIndex] == *it) {
                    result.push_back(extArrayIndex);
                    break;
                }
                extArrayIndex++;
            }
        }
        return result;
    }

    MartinDataFrame<T> run(
        const OriginDataFrame<T> &openOriginDataFrame,
        const Operation &op,
        const std::vector<T> &martinPositionIntervals,
        const std::vector<T> &martinStopProfitTargets,
        const T &martinStopLossTarget) const {
        return run(locateOpenArrayIndex(openOriginDataFrame),
                   op,
                   martinPositionIntervals,
                   martinStopProfitTargets,
                   martinStopLossTarget);
    }

    MartinDataFrame<T> run(
        const std::vector<std::size_t> &openArrayIndexList,
        const Operation &op,
        const std::vector<T> &martinPositionIntervals,
        const std::vector<T> &martinStopProfitTargets,
        const T &martinStopLossTarget) const {
        MartinDataFrame<DATA_TYPE> martinDataFrame(
            martinPositionIntervals,
            martinStopProfitTargets,
            martinStopLossTarget);

        thread_pool threadPool(workerNum_);
        std::vector<MartinResult> martinResultList;
        std::vector<int> a;
        martinResultList.resize(openArrayIndexList.size());
        for (size_t i = 0; i <= openArrayIndexList.size(); i++) {
            auto openArrayIndex = openArrayIndexList[i];
            threadPool.push_task([this, &martinResultList, &martinDataFrame, &op, openArrayIndex, i] { martinResultList[i] = this->run(openArrayIndex, op,
                                                                                                                                       martinDataFrame.addPositionIntervals_,
                                                                                                                                       martinDataFrame.stopProfitTargets_,
                                                                                                                                       martinDataFrame.stopLossTarget_); });
        }

        for (size_t i = 0; i <= openArrayIndexList.size(); i++) {
            auto openArrayIndex = openArrayIndexList[i];
            martinDataFrame.appendMartinResult(martinResultList[i], extDataFrame_[openArrayIndex]);
        }
        return martinDataFrame;
    }

    /**
     * @brief  Simulate to get the martin result of a specific open tick
     *
     * @param openArrayIndex
     * @param op
     * @param martinPositionIntervals
     * @param martinStopProfitTargets
     * @param martinStopLossTarget
     * @return MartinResult
     */
    MartinResult run(
        const std::size_t openArrayIndex,
        const Operation &op,
        const std::vector<T> &martinPositionIntervals,
        const std::vector<T> &martinStopProfitTargets,
        const T &martinStopLossTarget) const {
        // ArrayIndex: array index of ExtDataFrame. NOT extDataFrame.index
        MartinResult martinResult;
        martinResult.closeType_ = CloseType::NOT_CLOSE;
        martinResult.op_ = op;
        auto currentMartinIndex = 0;
        auto openBidPrice = extDataFrame_.bid_[openArrayIndex];
        auto openAskPrice = extDataFrame_.ask_[openArrayIndex];
        DATA_TYPE nextStopProfitPrice, nextAddPositionPrice, stopLossPrice;
        martinResult.addPositionsArrayIndex_.push_back(openArrayIndex);

        stopLossPrice = [&]() {
            if (op == Operation::BUY)
                return openAskPrice - martinStopLossTarget;
            if (op == Operation::SELL)
                return openBidPrice + martinStopLossTarget;
            return 0;
        }();

        auto updateMartin = [&]() {
            if (op == Operation::BUY) {
                if (currentMartinIndex < martinPositionIntervals.size())
                    nextAddPositionPrice = openAskPrice - martinPositionIntervals[currentMartinIndex + 1];
                auto lastAddPositionPrice = openAskPrice - martinPositionIntervals[currentMartinIndex];
                nextStopProfitPrice = lastAddPositionPrice + martinStopProfitTargets[currentMartinIndex];
            }
            if (op == Operation::SELL) {
                if (currentMartinIndex < martinPositionIntervals.size())
                    nextAddPositionPrice = openBidPrice + martinPositionIntervals[currentMartinIndex + 1];
                auto lastAddPositionPrice = openBidPrice + martinPositionIntervals[currentMartinIndex];
                nextStopProfitPrice = lastAddPositionPrice - martinStopProfitTargets[currentMartinIndex];
            }
            currentMartinIndex++;
        };

        // Assign next close price and add position price depends on BUY or SELL
        updateMartin();

        auto currentArrayIndex = openArrayIndex + 1;
        while (true) {
            bool stopLossFlag = currentMartinIndex == martinPositionIntervals.size();
            // early stop (reach the end of extDataFrame)
            if (currentArrayIndex >= extDataFrame_.size()) {
                martinResult.closeArrayIndex_ = extDataFrame_.size() - 1;
                martinResult.closeType_ = CloseType::STOP_EARLY;
                return martinResult;
            }

            // skip large window if possible
            if (couldSkipWindow(op, stopLossFlag, stopLossPrice, nextStopProfitPrice, nextAddPositionPrice,
                                extDataFrame_.futureBidMaxLargeWindow_[currentArrayIndex],
                                extDataFrame_.futureBidMinLargeWindow_[currentArrayIndex],
                                extDataFrame_.futureAskMaxLargeWindow_[currentArrayIndex],
                                extDataFrame_.futureAskMinLargeWindow_[currentArrayIndex])) {
                currentArrayIndex += extDataFrame_.largeWindow_;
                continue;
            }

            // skip small window if possible
            if (couldSkipWindow(op, stopLossFlag, stopLossPrice, nextStopProfitPrice, nextAddPositionPrice,
                                extDataFrame_.futureBidMaxSmallWindow_[currentArrayIndex],
                                extDataFrame_.futureBidMinSmallWindow_[currentArrayIndex],
                                extDataFrame_.futureAskMaxSmallWindow_[currentArrayIndex],
                                extDataFrame_.futureAskMinSmallWindow_[currentArrayIndex])) {
                currentArrayIndex += extDataFrame_.smallWindow_;
                continue;
            }

            for (; currentArrayIndex < extDataFrame_.size();) {
                auto currentAsk = extDataFrame_.ask_[currentArrayIndex];
                auto currentBid = extDataFrame_.bid_[currentArrayIndex];

                // stop loss
                auto const shouldStopLoss = [&]() {
                    if (op == Operation::BUY) {
                        // stop loss is SELL(in bid price)
                        return stopLossFlag && stopLossPrice >= currentBid;
                    }
                    if (op == Operation::SELL) {
                        // stop loss is BUY(in ask price)
                        return stopLossFlag && stopLossPrice <= currentAsk;
                    }
                    return false;
                }();
                if (shouldStopLoss) {
                    martinResult.closeArrayIndex_ = currentArrayIndex;
                    martinResult.closeType_ = CloseType::STOP_LOSS;
                    return martinResult;
                }

                // add position
                auto const shouldAddPosition = [&]() {
                    if (stopLossFlag)
                        return false;
                    if (op == Operation::BUY) {
                        return currentAsk <= nextAddPositionPrice;
                    }
                    if (op == Operation::SELL) {
                        return currentBid >= nextAddPositionPrice;
                    }
                    return false;
                }();
                if (shouldAddPosition) {
                    martinResult.addPositionsArrayIndex_.push_back(currentArrayIndex);
                    updateMartin();
                    currentArrayIndex++;
                    break;
                }

                // stop profit
                auto const shouldStopProfit = [&]() {
                    if (op == Operation::BUY) {
                        return currentBid >= nextStopProfitPrice;
                    }
                    if (op == Operation::SELL) {
                        return currentAsk <= nextStopProfitPrice;
                    }
                    return false;
                }();
                if (shouldStopProfit) {
                    martinResult.closeArrayIndex_ = currentArrayIndex;
                    martinResult.closeType_ = CloseType::STOP_PROFIT;
                    return martinResult;
                }
                currentArrayIndex++;
            }
        }
    }

   private:
    ExtDataFrame<T> extDataFrame_;
    int workerNum_;
    bool couldSkipWindow(const Operation op,
                         bool stopLossFlag,
                         const T stopLossPrice,
                         const T stopProfitPrice,
                         const T addPositionPrice,
                         const T futureBidMax,
                         const T futureBidMin,
                         const T futureAskMax,
                         const T futureAskMin) const {
        // BUY as ask price and SELL as bid price
        if (op == Operation::BUY) {
            // stop profit is SELL(in bid price)
            auto shouldStopProfitInWindow = futureBidMax >= stopProfitPrice;
            if (stopLossFlag) {
                // stop loss is SELL(in bid price)
                auto shouldStopLossInWindow = futureBidMin <= stopLossPrice;
                return !(shouldStopProfitInWindow || shouldStopLossInWindow);
            } else {
                // add position is BUY(in ask price)
                auto shouldAddPosition = futureAskMin <= addPositionPrice;
                return !(shouldStopProfitInWindow || shouldAddPosition);
            }
        }

        if (op == Operation::SELL) {
            // stop profit is BUY(in ask price)
            auto shouldStopProfitInWindow = futureAskMin <= stopProfitPrice;
            if (stopLossFlag) {
                // stop loss is BUY(in ask price)
                auto shouldStopLossInWindow = futureAskMax >= stopLossPrice;
                return !(shouldStopProfitInWindow || shouldStopLossInWindow);
            } else {
                // add position is SELL(in bid price)
                auto shouldAddPositionInWindow = futureBidMax >= addPositionPrice;
                return !(shouldStopProfitInWindow || shouldAddPositionInWindow);
            }
        }
        return false;
    }
};

}  // namespace MikuTemplar

#endif