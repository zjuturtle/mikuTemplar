#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "cxxopt.hpp"
#include "tick.h"
#include "queue_with_max_min.h"

#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

using namespace std;
using namespace MikuTemplar;

vector<Tick<DATA_TYPE>> loadCsv(const string& input_file) {
    vector<Tick<DATA_TYPE>> rawData;
    fstream file(input_file, ios::in);
	if(file.is_open()){
        string line;
        string cell;
        getline(file, line); // Drop the first header line

		while(getline(file, line)){
			stringstream ss(line);
            Tick<DATA_TYPE> tick;
            
            getline(ss, cell, ',');
            tick.index_ = atoi(cell.c_str());
            getline(ss, tick.datetime_, ',');
            getline(ss, cell, ',');
            tick.price_ = atoi(cell.c_str());
            rawData.push_back(tick);
		}
	}
    return rawData;
}

vector<Tick<DATA_TYPE>> drop(const vector<Tick<DATA_TYPE> > &input) {
    vector<Tick<DATA_TYPE> > result;
    result.push_back(input[0]);
    for (size_t index = 1; index < input.size();index++) {
        if (input[index-1].price_ == input[index].price_) {
            continue;
        }
        result.push_back(input[index]);
    }
    return result;
}

vector<Tick<DATA_TYPE>> monotonic(const vector<Tick<DATA_TYPE> > &input) {
    vector<Tick<DATA_TYPE>> result;
    if (input.size() <= 2) {
        result = input;
    } else {
        result.push_back(input[0]);

        bool goUp = result[1].price_ > result[0].price_;
        for (size_t index = 1; index < input.size(); index++) {
            if (goUp && input[index].price_ > input[index-1].price_) continue;
            if (!goUp&& input[index].price_ < input[index-1].price_) continue;
            goUp = !goUp;
            result.push_back(input[index]);
        }
    }
    return result;
}

int main(int argc, char *argv[]){
    cxxopts::Options options("ext", "Generate ext data for fast martin simulating");
    options.add_options()
        ("i,input", "input csv file path", cxxopts::value<std::string>())
        ("o,output", "output csv file path", cxxopts::value<std::string>())
        ("s,small_window", "small data window size", cxxopts::value<int>()->default_value("10"))
        ("l,large_window", "large data window size", cxxopts::value<int>()->default_value("100"))
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }

    auto rawData = loadCsv(result["input"].as<string>());
    
    // Step 1: drop no change data
    auto uniqueData = drop(rawData);

    // Step 2: drop monotonic data
    auto monotonicData = monotonic(uniqueData);

    // Step 3: use queue to get future min/max
    cout << "";
    }