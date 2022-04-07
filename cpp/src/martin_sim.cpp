#include "cxxopt.hpp"
#include "io.h"
#include "ext_dataframe.h"
#include "origin_dataframe.h"
#include "martin_dataframe.h"
#include <string>
#include <vector>


#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

using namespace MikuTemplar;
using namespace std;

/**
 * @brief Search every hit in extDataFrame by given OriginDataFrame
 * 
 * @param extDataFrame 
 * @param openOriginDataFrame 
 * @return vector<size_t> 
 */
vector<size_t> locateOpenArrayIndex(const ExtDataFrame<DATA_TYPE> &extDataFrame,
                                    const OriginDataFrame<DATA_TYPE> &openOriginDataFrame) {
    vector<size_t> result;
    size_t extArrayIndex=0;
    for (auto it=openOriginDataFrame.index_.cbegin(); it != openOriginDataFrame.index_.cend(); it++) {
        while (true) {
            if (extDataFrame.index_[extArrayIndex] == *it) {
                result.push_back(extArrayIndex);
                break;
            }
            extArrayIndex++;
        }
    }
    return result;
}

bool couldSkipWindow(const Operation op,
                     bool stopLossFlag,
                     const DATA_TYPE stopLossPrice,
                     const DATA_TYPE stopProfitPrice,
                     const DATA_TYPE addPositionPrice,
                     const DATA_TYPE futureBidMax, 
                     const DATA_TYPE futureBidMin,
                     const DATA_TYPE futureAskMax,
                     const DATA_TYPE futureAskMin){
    // BUY as ask price and SELL as bid price
    if (op == Operation::BUY) {
        // stop profit is SELL(in bid price)
        auto shouldStopProfitInWindow = futureBidMax > stopProfitPrice;
        if (stopLossFlag) {
            // stop loss is SELL(in bid price) 
            auto shouldStopLossInWindow = futureBidMin < stopLossPrice;
            return !(shouldStopProfitInWindow || shouldStopLossInWindow);
        } else {
            // add position is BUY(in ask price)
            auto shouldAddPosition = futureAskMin < addPositionPrice;
            return !(shouldStopProfitInWindow || shouldAddPosition);
        }
    }

    if (op == Operation::SELL) {
        // stop profit is BUY(in ask price)
        auto shouldStopProfitInWindow = futureAskMin < stopProfitPrice;
        if (stopLossFlag) {
            // stop loss is BUY(in ask price)
            auto shouldStopLossInWindow = futureAskMax > stopLossPrice;
            return !(shouldStopProfitInWindow || shouldStopLossInWindow); 
        } else {
            // add position is SELL(in bid price)
            auto shouldAddPositionInWindow = futureBidMax > addPositionPrice;
            return !(shouldStopProfitInWindow || shouldAddPositionInWindow);
        }
    }
    return false;
}

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
    const ExtDataFrame<DATA_TYPE> &extDataFrame,
    const size_t openArrayIndex,
    const Operation &op,
    const vector<DATA_TYPE> &martinPositionIntervals,
    const vector<DATA_TYPE> &martinStopProfitTargets,
    const DATA_TYPE &martinStopLossTarget) {
    
    // ArrayIndex: array index of ExtDataFrame. NOT extDataFrame.index
    MartinResult martinResult;
    martinResult.closeType_ = CloseType::NOT_CLOSE;
    martinResult.op_ = op;
    auto currentMartinIndex = 0;
    auto openBidPrice = extDataFrame.bid_[openArrayIndex];
    auto openAskPrice = extDataFrame.ask_[openArrayIndex];
    DATA_TYPE nextStopProfitPrice, nextAddPositionPrice, stopLossPrice;
    martinResult.addPositionsArrayIndex_.push_back(openArrayIndex);

    stopLossPrice = [&]() {
        if (op == Operation::BUY) return openBidPrice - martinStopLossTarget;
        if (op == Operation::SELL) return openAskPrice + martinStopLossTarget;
        return 0;
    }();

    auto updateMartin = [&]() {
        if (op == Operation::BUY) {
            nextStopProfitPrice = openAskPrice + martinStopProfitTargets[currentMartinIndex];
            nextAddPositionPrice = openAskPrice - martinPositionIntervals[currentMartinIndex];
        }
        if (op == Operation::SELL) {
            nextStopProfitPrice = openBidPrice - martinStopProfitTargets[currentMartinIndex];
            nextAddPositionPrice =  openBidPrice + martinPositionIntervals[currentMartinIndex];
        }
        currentMartinIndex++;
    };

    // Assign next close price and add position price depends on BUY or SELL
    updateMartin();

    // Loop till close
    auto currentArrayIndex = openArrayIndex + 1;
    while (true) {
        bool stopLossFlag = currentMartinIndex == martinPositionIntervals.size();

        // skip large window if possible
        if (couldSkipWindow(op, stopLossFlag, stopLossPrice, nextStopProfitPrice, nextAddPositionPrice,
                    extDataFrame.futureBidMaxLargeWindow_[currentArrayIndex], 
                    extDataFrame.futureBidMinLargeWindow_[currentArrayIndex],
                    extDataFrame.futureAskMaxLargeWindow_[currentArrayIndex],
                    extDataFrame.futureAskMinLargeWindow_[currentArrayIndex])) {
            currentArrayIndex += extDataFrame.largeWindow_;
            continue;
        }

        // skip small window if possible
        if (couldSkipWindow(op, stopLossFlag, stopLossPrice, nextStopProfitPrice, nextAddPositionPrice,
                    extDataFrame.futureBidMaxSmallWindow_[currentArrayIndex], 
                    extDataFrame.futureBidMinSmallWindow_[currentArrayIndex],
                    extDataFrame.futureAskMaxSmallWindow_[currentArrayIndex],
                    extDataFrame.futureAskMinSmallWindow_[currentArrayIndex])) {
            currentArrayIndex += extDataFrame.smallWindow_;
            continue;
        }
        
        for (;currentArrayIndex < extDataFrame.size(); currentArrayIndex++) {
            auto currentAsk = extDataFrame.ask_[currentArrayIndex];
            auto currentBid = extDataFrame.bid_[currentArrayIndex];

            // stop loss
            auto const shouldStopLoss = [&]() {
                if (op == Operation::BUY) {
                    // stop loss is SELL(in bid price) 
                    return stopLossFlag && stopLossPrice < currentBid;
                }
                if (op == Operation::SELL) {
                    // stop loss is BUY(in ask price)
                    return stopLossFlag && stopLossPrice > currentAsk;
                }
                return false;
            }();
            if (shouldStopLoss) {
                martinResult.closeArrayIndex_ = currentArrayIndex;
                martinResult.closeType_ = CloseType::STOP_LOSS;
                return martinResult;
            }

            // add position
            auto const shouldAddPosition = [&](){
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
                    return extDataFrame.bid_[currentArrayIndex] >= nextStopProfitPrice;
                }
                if (op == Operation::SELL) {
                    return extDataFrame.ask_[currentArrayIndex] <= nextStopProfitPrice;
                }
                return false;
            }();
            if (shouldStopProfit) {
                martinResult.closeArrayIndex_ = currentArrayIndex;
                martinResult.closeType_ = CloseType::STOP_PROFIT;
                return martinResult;
            }
        }
        
        // early stop (reach the end of extDataFrame)
        if (currentArrayIndex >= extDataFrame.size()) {
            martinResult.closeArrayIndex_ = extDataFrame.size()-1;
            martinResult.closeType_ = CloseType::STOP_EARLY;
            return martinResult;
        }
    }
}

int main(int argc, char *argv[]){
    cxxopts::Options options("martin sim", "Simulate martin process for each input open row.");
    options.add_options()
        ("input_ext", "input ext csv file", cxxopts::value<string>())
        ("input_open","input open csv file. must included in input_ext", cxxopts::value<string>())
        ("operation", "open direction, buy or sell", cxxopts::value<string>())
        ("output", "output martin result csv file path", cxxopts::value<string>())
        ("martin_position_intervals", "martin add position intervals, in list form", cxxopts::value<vector<int>>())
        ("martin_stop_profits","martin stop profit target, in list form", cxxopts::value<vector<int>>())
        ("martin_stop_loss", "martin stop loss target", cxxopts::value<int>())
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help")){
      std::cout << options.help() << std::endl;
      exit(0);
    }
    cout << "[INFO]Start martin simulate" << endl;
    auto operation = generateOperation(result["operation"].as<string>());
    auto extDataFrame = loadExtCsv<DATA_TYPE>(result["input_ext"].as<string>());
    
    auto openOriginDataFrame = loadOriginCsv<DATA_TYPE>(result["input_open"].as<string>());
    cout << "[INFO]Finish data loading" << endl;

    auto locateExtArrayIndexs = locateOpenArrayIndex(extDataFrame, openOriginDataFrame);
    cout << "[INFO]Finish array index locating" << endl;

    MartinDataFrame<DATA_TYPE> martinDataFrame(
        cast<DATA_TYPE>(result["martin_position_intervals"].as<vector<int>>()),
        cast<DATA_TYPE>(result["martin_stop_profits"].as<vector<int>>()),
        static_cast<DATA_TYPE>(result["martin_stop_loss"].as<int>())
    );
    
    double done = 0;
    double all = locateExtArrayIndexs.size();
    for (auto it = locateExtArrayIndexs.cbegin(); it != locateExtArrayIndexs.cend(); it++) {
        auto openArrayIndex = *it;
        auto martinResult = martinSim(extDataFrame, openArrayIndex, operation, 
                                      martinDataFrame.addPositionIntervals_, 
                                      martinDataFrame.stopProfitTargets_, 
                                      martinDataFrame.stopLossTarget_);
        martinDataFrame.appendMartinResult(martinResult, extDataFrame[openArrayIndex]);
        done++;
        cout << "\r[INFO] "<< int(done) << "/" << int(all) << "=" << fixed << setprecision(2) << done/all * 100 << "%"; 
    }

    saveMartinCsv(result["output"].as<string>(), martinDataFrame);
    cout << endl << "[INFO]All done"<< endl;
}