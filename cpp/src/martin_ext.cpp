#include <string>
#include <vector>

#include "utils/cxxopt.hpp"
#include "utils/helper.h"
#include "utils/queue_with_max_min.h"
#include "core/io.h"
#include "core/ext_dataframe.h"
#include "core/preprocess_dataframe.h"

#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

using namespace std;
using namespace MikuTemplar;

PreprocessDataFrame<DATA_TYPE> drop(const PreprocessDataFrame<DATA_TYPE> &input) {
    PreprocessDataFrame<DATA_TYPE> result;

    result.append(input[0]);
    for (size_t index = 1; index < input.index_.size();index++) {
        if (input.bid_[index-1] == input.bid_[index] && input.ask_[index-1] == input.ask_[index]) {
            continue;
        }
        result.append(input[index]);
    }
    return result;
}

ExtDataFrame<DATA_TYPE> extendMaxMin(const PreprocessDataFrame<DATA_TYPE> &input, const int smallWindow, const int largeWindow) {
    ExtDataFrame<DATA_TYPE> result;
    result.assign(input);
    result.smallWindow_ = smallWindow;
    result.largeWindow_ = largeWindow;

    QueueWithMaxMin<DATA_TYPE> bidSmallQueue, bidLargeQueue, askSmallQueue, askLargeQueue;
    result.futureBidMaxLargeWindow_.resize(input.size(), 0);
    result.futureBidMinLargeWindow_.resize(input.size(), 0);
    result.futureBidMaxSmallWindow_.resize(input.size(), 0);
    result.futureBidMinSmallWindow_.resize(input.size(), 0);

    result.futureAskMaxLargeWindow_.resize(input.size(), 0);
    result.futureAskMinLargeWindow_.resize(input.size(), 0);
    result.futureAskMaxSmallWindow_.resize(input.size(), 0);
    result.futureAskMinSmallWindow_.resize(input.size(), 0);

    for (int index=input.size()-1;index >= 0;index--) {
        bidSmallQueue.push(input.bid_[index]);
        bidLargeQueue.push(input.bid_[index]);
        askSmallQueue.push(input.ask_[index]);
        askLargeQueue.push(input.ask_[index]);

        if (bidSmallQueue.size() > smallWindow) bidSmallQueue.pop();
        if (bidLargeQueue.size() > largeWindow) bidLargeQueue.pop();
        if (askSmallQueue.size() > smallWindow) askSmallQueue.pop();
        if (askLargeQueue.size() > largeWindow) askLargeQueue.pop();

        result.futureBidMaxLargeWindow_[index]=bidLargeQueue.max();
        result.futureBidMinLargeWindow_[index]=bidLargeQueue.min();
        result.futureBidMaxSmallWindow_[index]=bidSmallQueue.max();
        result.futureBidMinSmallWindow_[index]=bidSmallQueue.min();
        result.futureAskMaxLargeWindow_[index]=askLargeQueue.max();
        result.futureAskMinLargeWindow_[index]=askLargeQueue.min();
        result.futureAskMaxSmallWindow_[index]=askSmallQueue.max();
        result.futureAskMinSmallWindow_[index]=askSmallQueue.min();
    }
    return result;
}

int main(int argc, char *argv[]){
    cxxopts::Options options("martin ext", "Generate ext data for fast martin simulating");
    options.add_options()
        ("input", "input preprocess csv file path", cxxopts::value<std::string>())
        ("output", "output ext csv file path", cxxopts::value<std::string>())
        ("small_window", "small data window size", cxxopts::value<int>()->default_value("10"))
        ("large_window", "large data window size", cxxopts::value<int>()->default_value("100"))
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help")){
      std::cout << options.help() << std::endl;
      exit(0);
    }
    cout << "[INFO]start process" << endl;
    auto preprocessData = loadPreprocessCsv<DATA_TYPE>(result["input"].as<string>());
    cout << "[INFO]finish load" << endl;
    // Step 1: drop no change data
    auto uniqueData = drop(preprocessData);
    cout << "[INFO]finish drop" << endl;
    // Step 2: generate ExtDataFrame
    auto extDataFrame = extendMaxMin(uniqueData, 
                                     result["small_window"].as<int>(),
                                     result["large_window"].as<int>());
    cout << "[INFO]finish ext col generate" << endl;
    // Step 3: write to csv
    saveExtCsv(result["output"].as<string>(), extDataFrame);
    cout << "[INFO]All done";
}