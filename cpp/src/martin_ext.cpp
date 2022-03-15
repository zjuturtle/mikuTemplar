#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>

#include "cxxopt.hpp"
#include "utils.h"
#include "ext_dataframe.h"
#include "origin_dataframe.h"
#include "queue_with_max_min.h"

#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

using namespace std;
using namespace MikuTemplar;

void saveExtCsv(const string &outputFile, const ExtDataFrame<DATA_TYPE> &extDataFrame) {
    fstream file(outputFile, ios::out);
    file << "index,datetime," << extDataFrame.priceName_ 
         << ",futureMaxSmallWindow" << extDataFrame.smallWindow_
         << ",futureMinSmallWindow" << extDataFrame.smallWindow_
         << ",futureMaxLargeWindow" << extDataFrame.largeWindow_
         << ",futureMinLargeWindow" << extDataFrame.largeWindow_ << endl;

    for (size_t index=0; index < extDataFrame.size();index++) {
        file << extDataFrame.index_[index]<<","
             << extDataFrame.datetime_[index]<<","
             << extDataFrame.price_[index] << "," 
             << extDataFrame.futureMaxSmallWindow_[index] << "," 
             << extDataFrame.futureMinSmallWindow_[index] << ","
             << extDataFrame.futureMaxLargeWindow_[index] << ","
             << extDataFrame.futureMinLargeWindow_[index] <<endl;
    }
    file.close();
}

OriginDataFrame<DATA_TYPE> loadOriginCsv(const string& inputFile) {
    OriginDataFrame<DATA_TYPE> originDataFrame;
    fstream file(inputFile, ios::in);
	if(file.is_open()){
        string line;
        getline(file, line);
        originDataFrame.priceName_ = trimCopy(split(line)[2]);
		while(getline(file, line)){
            auto tmp=split(line);
            originDataFrame.index_.push_back(atoi(tmp[0].c_str()));
            originDataFrame.datetime_.push_back(trimCopy(tmp[1]));
            originDataFrame.price_.push_back(atoi(tmp[2].c_str()));
		}
	}
    return originDataFrame;
}

OriginDataFrame<DATA_TYPE> drop(const OriginDataFrame<DATA_TYPE> &input) {
    OriginDataFrame<DATA_TYPE> result;
    result.priceName_ = input.priceName_;

    result.append(input[0]);
    for (size_t index = 1; index < input.index_.size();index++) {
        if (input.price_[index-1] == input.price_[index]) {
            continue;
        }
        result.append(input[index]);
    }
    return result;
}

OriginDataFrame<DATA_TYPE> monotonic(const OriginDataFrame<DATA_TYPE> &input) {
    OriginDataFrame<DATA_TYPE> result;
    if (input.size() <= 2) {
        result = input;
    } else {
        result.priceName_ = input.priceName_;
        result.append(input[0]);
        bool goUp = input.price_[1] > input.price_[0];
        for (size_t index = 1; index < input.price_.size(); index++) {
            if (goUp && input.price_[index] > input.price_[index-1]) continue;
            if (!goUp&& input.price_[index] < input.price_[index-1]) continue;
            goUp = !goUp;
            result.append(input[index-1]);
        }
        if ((goUp && input.price_[input.size()-1] > input.price_[input.size()-2]) || 
           (!goUp && input.price_[input.size()-1] < input.price_[input.size()-2])) {
            result.append(input[input.size()-1]);
        }
    }
    return result;
}

ExtDataFrame<DATA_TYPE> extendMaxMin(const OriginDataFrame<DATA_TYPE> &input, const int smallWindow, const int largeWindow) {
    ExtDataFrame<DATA_TYPE> result;
    result.assign(input);
    result.smallWindow_ = smallWindow;
    result.largeWindow_ = largeWindow;

    QueueWithMaxMin<DATA_TYPE> smallQueue, largeQueue;
    result.futureMaxLargeWindow_.resize(input.size(), 0);
    result.futureMaxLargeWindow_.resize(input.size(), 0);
    result.futureMaxSmallWindow_.resize(input.size(), 0);
    result.futureMinSmallWindow_.resize(input.size(), 0);
    for (size_t index=input.size()-1;index >= 0;index--) {
        smallQueue.push(input.price_[index]);
        largeQueue.push(input.price_[index]);
        if (smallQueue.size() > smallWindow) smallQueue.pop();
        if (largeQueue.size() > largeWindow) largeQueue.pop();

        result.futureMaxLargeWindow_[index]=largeQueue.max();
        result.futureMinLargeWindow_[index]=largeQueue.min();
        result.futureMaxSmallWindow_[index]=smallQueue.max();
        result.futureMinSmallWindow_[index]=smallQueue.min();
    }

    return result;
}


int main(int argc, char *argv[]){
    cxxopts::Options options("martin ext", "Generate ext data for fast martin simulating");
    options.add_options()
        ("i,input", "input csv file path", cxxopts::value<std::string>())
        ("o,output", "output csv file path", cxxopts::value<std::string>())
        ("s,small_window", "small data window size", cxxopts::value<int>()->default_value("10"))
        ("l,large_window", "large data window size", cxxopts::value<int>()->default_value("100"))
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help")){
      std::cout << options.help() << std::endl;
      exit(0);
    }

    auto rawData = loadOriginCsv(result["input"].as<string>());
    
    // Step 1: drop no change data
    auto uniqueData = drop(rawData);

    // Step 2: drop monotonic data
    auto monotonicData = monotonic(uniqueData);

    // Step 3: generate ExtDataFrame
    auto extDataFrame = extendMaxMin(monotonicData, 
                                     result["small_window"].as<int>(),
                                     result["large_window"].as<int>());

    // Step 4: write to csv
    saveExtCsv(result["output"].as<string>(), extDataFrame);
    cout << "All done";
}