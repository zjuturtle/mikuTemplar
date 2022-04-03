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
                     const DATA_TYPE nextStopProfitPrice,
                     const DATA_TYPE nextAddPositionPrice,
                     const DATA_TYPE futureBidMax, 
                     const DATA_TYPE futureBidMin,
                     const DATA_TYPE futureAskMax,
                     const DATA_TYPE futureAskMin){
    // BUY as ask price and SELL as bid price
    if (op == Operation::BUY) {
        // stop profit operation is SELL(in bid price), add position is BUY(in ask price)
        return nextAddPositionPrice < futureAskMin  && futureBidMax < nextStopProfitPrice;
    }

    if (op == Operation::SELL) {
        // stop profit operation is BUY(in ask price), add position is SELL(in bid price)
        return nextStopProfitPrice < futureAskMax && futureBidMin > nextAddPositionPrice;
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
    martinResult.op_ = op;
    auto currentMartinIndex = 0;
    auto openBidPrice = extDataFrame.bid_[openArrayIndex];
    auto openAskPrice = extDataFrame.ask_[openArrayIndex];
    DATA_TYPE nextStopProfitPrice, nextAddPositionPrice, stopLossPrice;
    vector<int> addPositionArrayIndexs;
    addPositionArrayIndexs.push_back(openArrayIndex);

    stopLossPrice = [&]() {
        if (op == Operation::BUY) return openBidPrice + martinStopLossTarget;
        
    }();

    auto updateMartin = [&]() {
        if (op == Operation::BUY) {
            nextStopProfitPrice = openBidPrice + martinPositionIntervals[currentMartinIndex];
            nextAddPositionPrice = openAskPrice + martinStopProfitTargets[currentMartinIndex];
            nextStopLossPrice = openBidPrice + martinStopLossTargets[currentMartinIndex];
        }
        if (op == Operation::SELL) {
            nextStopProfitPrice = openAskPrice + martinPositionIntervals[currentMartinIndex];
            nextAddPositionPrice =  openBidPrice + martinStopProfitTargets[currentMartinIndex];
            nextStopLossPrice = openAskPrice + martinStopLossTargets[currentMartinIndex];
        }
        currentMartinIndex++;
    };

    // Assign next close price and add position price depends on BUY or SELL
    updateMartin();

    // Loop 
    auto currentArrayIndex = openArrayIndex;
    while (true) {
        // skip large window if possible
        if (couldSkipWindow(op, nextStopProfitPrice, nextStopLossPrice,
                    extDataFrame.futureBidMaxLargeWindow_[currentArrayIndex], 
                    extDataFrame.futureBidMinLargeWindow_[currentArrayIndex],
                    extDataFrame.futureAskMaxLargeWindow_[currentArrayIndex],
                    extDataFrame.futureAskMinLargeWindow_[currentArrayIndex])) {
            currentArrayIndex += extDataFrame.largeWindow_;
            continue;
        }

        // skip small window if possible
        if (couldSkipWindow(op, nextStopProfitPrice, nextStopLossPrice,
                    extDataFrame.futureBidMaxSmallWindow_[currentArrayIndex], 
                    extDataFrame.futureBidMinSmallWindow_[currentArrayIndex],
                    extDataFrame.futureAskMaxSmallWindow_[currentArrayIndex],
                    extDataFrame.futureAskMinSmallWindow_[currentArrayIndex])) {
            currentArrayIndex += extDataFrame.smallWindow_;
            continue;
        }

        martinResult.closeType_ = CloseType::NOT_CLOSE;
        for (;currentArrayIndex < extDataFrame.size(); currentArrayIndex++) {
            // add position
            auto const shouldAddPosition = [&](){
                if (op == Operation::BUY) {
                    return extDataFrame.ask_[currentArrayIndex] <= nextAddPositionPrice;
                }
                if (op == Operation::SELL) {
                    return extDataFrame.bid_[currentArrayIndex] >= nextAddPositionPrice;
                }
                return false;
            }();
            if (shouldAddPosition) {
                addPositionArrayIndexs.push_back(currentArrayIndex);
                updateMartin();
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
                break;
            }

            // 
            if (extDataFrame.ask_[currentArrayIndex] )
        }

        

    }
}

MartinDataFrame<DATA_TYPE> martinSim(
    const ExtDataFrame<DATA_TYPE> &extDataFrame,
    const vector<size_t> &openExtArrayIndex,
    const Operation &op,
    const vector<DATA_TYPE> &martinIntervals,
    const vector<DATA_TYPE> &martinTargets) {
    
    // BUY as ask price and SELL as bid price
    for (auto it=openExtArrayIndex.cbegin();it!=openExtArrayIndex.cend();it++) {
        auto openIndex = *it;
        auto currentMartinIndex = 0;
        auto currentIndex = openIndex;
        auto openBidPrice = extDataFrame.bid_[openIndex];
        auto openAskPrice = extDataFrame.ask_[openIndex];
        DATA_TYPE nextClosePrice, nextAddPositionPrice;
        vector<int> addPositionIndexs;
        int closeIndex = -1;
        addPositionIndexs.push_back(openIndex);
        if (op == Operation::BUY) {
            nextClosePrice = openBidPrice + martinIntervals[currentMartinIndex];
            nextAddPositionPrice = openAskPrice + martinTargets[currentMartinIndex];
        }

        if (op == Operation::SELL) {
            nextClosePrice = openAskPrice + martinIntervals[currentMartinIndex];
            nextAddPositionPrice =  openBidPrice + martinTargets[currentMartinIndex];
        }
        
        while (true) {
            // skip large window if possible
            if (couldSkipWindow(op, nextClosePrice, nextAddPositionPrice,
                     extDataFrame.futureBidMaxLargeWindow_[currentIndex], 
                     extDataFrame.futureBidMinLargeWindow_[currentIndex],
                     extDataFrame.futureAskMaxLargeWindow_[currentIndex],
                     extDataFrame.futureAskMinLargeWindow_[currentIndex])) {
                currentIndex += extDataFrame.largeWindow_;
                continue;
            }

            // skip small window if possible
            if (couldSkipWindow(op, nextClosePrice, nextAddPositionPrice,
                     extDataFrame.futureBidMaxSmallWindow_[currentIndex], 
                     extDataFrame.futureBidMinSmallWindow_[currentIndex],
                     extDataFrame.futureAskMaxSmallWindow_[currentIndex],
                     extDataFrame.futureAskMinSmallWindow_[currentIndex])) {
                currentIndex += extDataFrame.smallWindow_;
                continue;
            }

            for (;currentIndex < extDataFrame.size(); currentIndex++) {
                // Need add position
                if (extDataFrame.ask_[currentIndex] <= nextAddPositionPrice) {
                    currentMartinIndex++;
                    addPositionIndexs.push_back(currentIndex);
                    nextClosePrice = openBidPrice + martinTargets[currentMartinIndex];
                    nextAddPositionPrice = openAskPrice + martinIntervals[currentMartinIndex];
                    break;
                }
                // Need close
                if (extDataFrame.bid_[currentIndex] >= nextClosePrice) {
                    closeIndex = currentIndex;
                    break;
                }
            }

            if (closeIndex >=0 ) {
                break;
            }
        }
    }
    
}

int main(int argc, char *argv[]){
    cxxopts::Options options("martin sim", "Simulate martin process for each second");
    options.add_options()
        ("input_ext", "input ext csv file", cxxopts::value<string>())
        ("input_open","input open csv file", cxxopts::value<string>())
        ("action", "open direction, bid or ask", cxxopts::value<string>())
        ("output", "output martin result csv file path", cxxopts::value<string>())
        ("martin_interval", "martin intervals, in list form", cxxopts::value<vector<int>>())
        ("martin_target","martin close target, in list form", cxxopts::value<vector<int>>())
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help")){
      std::cout << options.help() << std::endl;
      exit(0);
    }

    auto extDataFrame = loadExtCsv<DATA_TYPE>(result["input_ext"].as<string>());
    auto openOriginDataFrame = loadOriginCsv<DATA_TYPE>(result["input_open_index"].as<string>());

    auto openExtArrayIndex = locateOpenArrayIndex(extDataFrame, openOriginDataFrame);

}