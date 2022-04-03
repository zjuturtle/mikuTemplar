#include "cxxopt.hpp"
#include "io.h"
#include "ext_dataframe.h"
#include "origin_dataframe.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>


#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

using namespace MikuTemplar;
using namespace std;

int main(int argc, char *argv[]){
    cxxopts::Options options("martin sampling", "Sample forex preprocessed tick raw data to generate tick data");
    options.add_options()
        ("input", "input preprocessed tick csv file", cxxopts::value<string>())
        ("output", "output sample tick csv file", cxxopts::value<string>())
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help")){
        std::cout << options.help() << std::endl;
        exit(0);
    }

    auto inputDataFrame = loadOriginCsv<DATA_TYPE>(result["input"].as<string>());
    OriginDataFrame<DATA_TYPE> outputDataFrame;
    int lastHour = -1;
    int lastMin = -1;
    int lastSec = -1;
    for (size_t i=0;i<inputDataFrame.size();i++) {
        int year, month, day, hour, min, sec;
        char drop;
        istringstream ss(inputDataFrame.datetime_[i]);
        ss >> year >> drop >> month >> drop >> day >> drop 
           >> hour >> drop >> min >> drop >> sec >> drop;
        
        bool sameSecond = lastHour == hour && lastMin == min && lastSec == sec;
        if (!sameSecond) {
            outputDataFrame.append(inputDataFrame[i]);
        }
        lastHour = hour;
        lastMin = min;
        lastSec = sec;
    }

    saveOriginCsv(result["output"].as<string>(), outputDataFrame);
}