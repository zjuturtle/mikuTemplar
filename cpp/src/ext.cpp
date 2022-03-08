#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "tick.h"
#include "queue_with_max_min.h"

#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

using namespace std;
using namespace MikuTemplar;

vector<Tick<DATA_TYPE> > loadCsv(const string& input_file) {
    vector<Tick<DATA_TYPE> > rawData;
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

vector<Tick<DATA_TYPE> > drop(const vector<Tick<DATA_TYPE> > &input) {
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

vector<Tick<DATA_TYPE> > monotonic(const vector<Tick<DATA_TYPE> > &input) {
    vector<Tick<DATA_TYPE> > result;
    
    return result;
}

int main(int argc, char *argv[]){
    cout << "Generate ext data for fast martin simulating" << endl;
    string inputCsvPath = string(argv[1]);
    string outputCsvPath;

    if (argc == 3) {
        outputCsvPath = string(argv[2]);
    } else {
        outputCsvPath = inputCsvPath + ".martin.csv";
    }

    vector<Tick<DATA_TYPE> > rawData = loadCsv(inputCsvPath);
    
    // Step 1: drop no change data
    vector<Tick<DATA_TYPE> > uniqueData = drop(rawData);

    // Step 2: drop monotonic data
    vector<Tick<DATA_TYPE> > monotonicData = monotonic(uniqueData);
}