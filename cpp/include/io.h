#ifndef MIKU_TEMPLAR_IO_H_
#define MIKU_TEMPLAR_IO_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "const.h"
#include "tick.h"
#include "utils.h"
#include "ext_dataframe.h"
#include "origin_dataframe.h"

namespace MikuTemplar{

template <class T>
void saveExtCsv(const std::string &outputFile, const ExtDataFrame<T> &extDataFrame) {
    std::fstream file(outputFile, std::ios::out);
    file << Key::INDEX << "," << Key::DATETIME << ","
         << Key::BID << "," << Key::ASK << ","
         << Key::FUTURE_BID_MAX_SMALL_WINDOW << extDataFrame.smallWindow_ << ","
         << Key::FUTURE_BID_MIN_SMALL_WINDOW << extDataFrame.smallWindow_ << ","
         << Key::FUTURE_BID_MAX_LARGE_WINDOW << extDataFrame.largeWindow_ << ","
         << Key::FUTURE_BID_MIN_LARGE_WINDOW << extDataFrame.largeWindow_ << ","
         << Key::FUTURE_ASK_MAX_SMALL_WINDOW << extDataFrame.smallWindow_ << ","
         << Key::FUTURE_ASK_MIN_SMALL_WINDOW << extDataFrame.smallWindow_ << ","
         << Key::FUTURE_ASK_MAX_LARGE_WINDOW << extDataFrame.largeWindow_ << ","
         << Key::FUTURE_ASK_MIN_LARGE_WINDOW << extDataFrame.largeWindow_ << std::endl;

    for (std::size_t index=0; index < extDataFrame.size();index++) {
        file << extDataFrame.index_[index]<<","
             << extDataFrame.datetime_[index]<<","
             << extDataFrame.bid_[index] << "," 
             << extDataFrame.ask_[index] << ","
             << extDataFrame.futureBidMaxSmallWindow_[index] << "," 
             << extDataFrame.futureBidMinSmallWindow_[index] << ","
             << extDataFrame.futureBidMaxLargeWindow_[index] << ","
             << extDataFrame.futureBidMinLargeWindow_[index] << ","

             << extDataFrame.futureAskMaxSmallWindow_[index] << "," 
             << extDataFrame.futureAskMinSmallWindow_[index] << ","
             << extDataFrame.futureAskMaxLargeWindow_[index] << ","
             << extDataFrame.futureAskMinLargeWindow_[index] << std::endl;
    }
    file.close();
}

template <class T> 
ExtDataFrame<T> loadExtCsv(const std::string& inputFile){
    ExtDataFrame<T> extDataFrame;
    std::fstream file(inputFile, std::ios::in);
	if(file.is_open()){
        std::string line;
        getline(file, line);
        extDataFrame.smallWindow_ = atoi(split(line)[4].substr(Key::FUTURE_BID_MAX_SMALL_WINDOW.size(), std::string::npos).c_str());
        extDataFrame.largeWindow_ = atoi(split(line)[6].substr(Key::FUTURE_BID_MAX_LARGE_WINDOW.size(), std::string::npos).c_str());
		while(getline(file, line)){
            auto tmp=split(line);
            extDataFrame.index_.push_back(atoi(tmp[0].c_str()));
            extDataFrame.datetime_.push_back(trimCopy(tmp[1]));
            extDataFrame.bid_.push_back(atoi(tmp[2].c_str()));
            extDataFrame.ask_.push_back(atoi(tmp[3].c_str()));

            extDataFrame.futureBidMaxSmallWindow_.push_back(atoi(tmp[4].c_str()));
            extDataFrame.futureBidMinSmallWindow_.push_back(atoi(tmp[5].c_str()));
            extDataFrame.futureBidMaxLargeWindow_.push_back(atoi(tmp[6].c_str()));
            extDataFrame.futureBidMinLargeWindow_.push_back(atoi(tmp[7].c_str()));

            extDataFrame.futureAskMaxSmallWindow_.push_back(atoi(tmp[8].c_str()));
            extDataFrame.futureAskMinSmallWindow_.push_back(atoi(tmp[9].c_str()));
            extDataFrame.futureAskMaxLargeWindow_.push_back(atoi(tmp[10].c_str()));
            extDataFrame.futureAskMinLargeWindow_.push_back(atoi(tmp[11].c_str()));
		}
	}
    return extDataFrame;
}

template <class T>
OriginDataFrame<T> loadOriginCsv(const std::string& inputFile) {
    OriginDataFrame<T> originDataFrame;
    std::fstream file(inputFile, std::ios::in);
	if(file.is_open()){
        std::string line;
        getline(file, line);
		while(getline(file, line)){
            auto tmp=split(line);
            originDataFrame.index_.push_back(atoi(tmp[0].c_str()));
            originDataFrame.datetime_.push_back(trimCopy(tmp[1]));
            originDataFrame.bid_.push_back(atoi(tmp[2].c_str()));
            originDataFrame.ask_.push_back(atoi(tmp[3].c_str()));
		}
	}
    return originDataFrame;
}

template <class T>
void saveOriginCsv(const std::string &outputFile, const OriginDataFrame<T> &originDataFrame) {
    std::fstream file(outputFile, std::ios::out);
    file << Key::INDEX << "," << Key::DATETIME << ","
         << Key::BID << "," << Key::ASK << std::endl;

    for (std::size_t index=0; index < originDataFrame.size();index++) {
        file << originDataFrame.index_[index]<<","
             << originDataFrame.datetime_[index]<<","
             << originDataFrame.bid_[index] << "," 
             << originDataFrame.ask_[index] << std::endl;
    }
    file.close();
}

template <class T>
void saveMartinCsv(const std::string &outputFile, const MartinDataFrame<T> &martinDataFrame) {
    std::fstream file(outputFile, std::ios::out);
    file << Key::INDEX << "," << Key::DATETIME << ","
         << Key::BID << "," << Key::ASK ;
    martinDataFrame.

    for (std::size_t index=0; index < martinDataFrame.size();index++) {
        file << martinDataFrame.index_[index]<<","
             << martinDataFrame.datetime_[index]<<","
             << martinDataFrame.bid_[index] << "," 
             << martinDataFrame.ask_[index];
         
        file << endl;
    }
    file.close();
}

}
#endif