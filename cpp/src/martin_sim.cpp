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

MartinDataFrame<DATA_TYPE> martinSim(
    const ExtDataFrame<DATA_TYPE> &extDataFrame,
    const vector<size_t> &openExtArrayIndex,
    const Operation &op,
    const vector<DATA_TYPE> &martinIntervals,
    const vector<DATA_TYPE> &martinTargets) {
    
    // BUY as ask price and SELL as bid price
    if (op == Operation::BUY) {

        for (auto it=openExtArrayIndex.cbegin();it!=openExtArrayIndex.cend();it++) {

            DATA_TYPE nextClosePrice = extDataFrame.bid_[*it] + martinTargets[0];
            DATA_TYPE nextAddPositionPrice = extDataFrame.ask_[*it] - martinIntervals[0];

            extDataFrame.
        }
    }

    if (op == Operation::SELL) {

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