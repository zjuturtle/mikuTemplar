#ifndef MIKU_TEMPLAR_CORE_IO_H_
#define MIKU_TEMPLAR_CORE_IO_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "core/const.h"
#include "core/martin_info.h"
#include "core/ext_dataframe.h"
#include "core/martin_dataframe.h"
#include "core/martin_parameters.h"
#include "core/origin_dataframe.h"
#include "core/tick.h"
#include "utils/helper.h"
#include "utils/rapidjson/document.h"
#include "utils/rapidjson/istreamwrapper.h"
#include "utils/rapidjson/ostreamwrapper.h"
#include "utils/rapidjson/writer.h"

namespace MikuTemplar {
inline std::vector<MartinParameters> loadMartinParametersJson(const std::string &jsonFile) {
    std::vector<MartinParameters> result;
    std::ifstream ifs(jsonFile);
    if (ifs.is_open()) {
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document d;
        d.ParseStream(isw);
        for (auto &v : d["martin"].GetArray()) {
            MartinParameters tmp;
            tmp.op_ = generateOperation(v["op"].GetString());
            for (auto &data : v["positionIntervals"].GetArray()) tmp.positionIntervals_.push_back(data.GetInt());
            for (auto &data : v["stopProfits"].GetArray()) tmp.stopProfits_.push_back(data.GetInt());
            tmp.stopLoss_ = v["stopLoss"].GetInt();
            tmp.minFactor_ = v["minFactor"].GetDouble();
            tmp.minLotUnit_ = v["minLotUnit"].GetDouble(); // 0.01 手
            tmp.minProfit_ = v["minProfit"].GetDouble();   // 基础货币
            tmp.totalLot_ = v["totalLot"].GetDouble();
            tmp.validCheck();
            result.push_back(tmp);
        }
    } else {
        std::cout << "[ERROR]Cannot open file " << jsonFile << std::endl;
    }
    return result;
}

template <class T>
rapidjson::Value createJsonArray(rapidjson::Document::AllocatorType& allocator, const std::vector<T> &array) {
    rapidjson::Value myArray(rapidjson::kArrayType);
    for (auto it : array) {
        myArray.PushBack(rapidjson::Value().SetDouble((double)it), allocator);
    }
    return myArray;
}

inline rapidjson::Value createJsonString(rapidjson::Document::AllocatorType& allocator, const std::string &str){
    rapidjson::Value res;
    res.SetString(str.c_str(), str.size(), allocator);
    return res;
}

inline void saveMartinInfos(const std::string &outputFile, const std::vector<MartinInfo> &martinInfos) {
    std::fstream file(outputFile, std::ios::out);
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Value myArray(rapidjson::kArrayType);
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
    

    for (auto martinInfo : martinInfos) {
        rapidjson::Value martinInfoObj;
        martinInfoObj.SetObject();

        // MartinParameters
        rapidjson::Value martinParameters;
        martinParameters.SetObject();
        martinParameters.AddMember("op", createJsonString(allocator, toString(martinInfo.p_.op_)), allocator);
        martinParameters.AddMember("positionIntervals", createJsonArray(allocator, martinInfo.p_.positionIntervals_), allocator);
        martinParameters.AddMember("stopProfits", createJsonArray(allocator, martinInfo.p_.stopProfits_), allocator);
        martinParameters.AddMember("minFactor", martinInfo.p_.minFactor_, allocator);
        martinParameters.AddMember("minLotUnit", martinInfo.p_.minLotUnit_, allocator);
        martinParameters.AddMember("totalLot", martinInfo.p_.totalLot_, allocator);
        martinParameters.AddMember("stopLoss", martinInfo.p_.stopLoss_, allocator);
        martinParameters.AddMember("minProfit", martinInfo.p_.minProfit_, allocator);

        //MartinCounts
        rapidjson::Value martinCounts;
        martinCounts.SetObject();
        martinCounts.AddMember("allCount", (int)martinInfo.c_.allCount_, allocator);
        martinCounts.AddMember("stopLossCount", (int)martinInfo.c_.stopLossCount_, allocator);
        martinCounts.AddMember("earlyStopCount", (int)martinInfo.c_.earlyStopCount_, allocator);
        martinCounts.AddMember("stopProfitsCount", createJsonArray(allocator, martinInfo.c_.stopProfitsCount_), allocator);

        //MartinStatic
        rapidjson::Value martinStatistics;
        martinStatistics.SetObject();
        martinStatistics.AddMember("stopLossPossibility", martinInfo.s_.stopLossPossibility_, allocator);
        martinStatistics.AddMember("earlyStopPossibility", martinInfo.s_.earlyStopPossibility_, allocator);
        martinStatistics.AddMember("stopProfitPossibility_", createJsonArray(allocator, martinInfo.s_.stopProfitPossibility_), allocator);
        martinStatistics.AddMember("bestLots", createJsonArray(allocator, martinInfo.s_.bestLots_), allocator);
        martinStatistics.AddMember("bestLotsWeight", createJsonArray(allocator, martinInfo.s_.bestLotsWeight_), allocator);
        martinStatistics.AddMember("bestProfit", martinInfo.s_.bestProfit_, allocator);

        martinInfoObj.AddMember("parametes", martinParameters, allocator);
        martinInfoObj.AddMember("counts", martinCounts, allocator);
        martinInfoObj.AddMember("statistics", martinStatistics, allocator);

        myArray.PushBack(martinInfoObj, allocator);
    }
    d.AddMember("martinInfos", myArray, allocator);

    std::ofstream ofs(outputFile);
    rapidjson::OStreamWrapper osw(ofs);
    
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    d.Accept(writer);
}

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

    for (std::size_t index = 0; index < extDataFrame.size(); index++) {
        file << extDataFrame.index_[index] << ","
             << extDataFrame.datetime_[index] << ","
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
ExtDataFrame<T> loadExtCsv(const std::string &inputFile) {
    ExtDataFrame<T> extDataFrame;
    std::fstream file(inputFile, std::ios::in);
    if (file.is_open()) {
        std::string line;
        getline(file, line);
        extDataFrame.smallWindow_ = atoi(split(line)[4].substr(Key::FUTURE_BID_MAX_SMALL_WINDOW.size(), std::string::npos).c_str());
        extDataFrame.largeWindow_ = atoi(split(line)[6].substr(Key::FUTURE_BID_MAX_LARGE_WINDOW.size(), std::string::npos).c_str());
        while (getline(file, line)) {
            auto tmp = split(line);
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
    for (std::size_t i=0;i<extDataFrame.index_.size()-1;i++) {
        if (extDataFrame.index_[i+1] <= extDataFrame.index_[i]) {
            std::cout << "[WARN]extDataFrame.index_[" << i +1 << "]=" << extDataFrame.index_[i+1]
                      << "<= extDataFrame.index_["<< i << "]=" << extDataFrame.index_[i] << std::endl;
        }
    }
    return extDataFrame;
}

template <class T>
OriginDataFrame<T> loadOriginCsv(const std::string &inputFile) {
    OriginDataFrame<T> originDataFrame;
    std::fstream file(inputFile, std::ios::in);
    if (file.is_open()) {
        std::string line;
        getline(file, line);
        if (split(line).size() > 4) {
            std::cout << "[WARN]Input file "<<inputFile<<" have extra columns, will be ignored."<<std::endl;
        }
        while (getline(file, line)) {
            auto tmp = split(line);
            originDataFrame.index_.push_back(atoi(tmp[0].c_str()));
            originDataFrame.datetime_.push_back(trimCopy(tmp[1]));
            originDataFrame.bid_.push_back(atoi(tmp[2].c_str()));
            originDataFrame.ask_.push_back(atoi(tmp[3].c_str()));

            if (originDataFrame.index_.size() >= 2) {
                auto lastIndex = originDataFrame.index_.size() - 1;
                if (originDataFrame.index_[lastIndex-1] >= originDataFrame.index_[lastIndex]) {
                    std::cout << "[WARN]originDataFrame.index_[" << lastIndex-1 << "]=" << originDataFrame.index_[lastIndex-1]
                              << "<= originDataFrame.index_["<< lastIndex << "]=" << originDataFrame.index_[lastIndex]
                              << ". The original text is "<< line <<std::endl;
                }
            }
        }
    }
    return originDataFrame;
}

template <class T>
void saveOriginCsv(const std::string &outputFile, const OriginDataFrame<T> &originDataFrame) {
    std::fstream file(outputFile, std::ios::out);
    file << Key::INDEX << "," << Key::DATETIME << ","
         << Key::BID << "," << Key::ASK << std::endl;

    for (std::size_t index = 0; index < originDataFrame.size(); index++) {
        file << originDataFrame.index_[index] << ","
             << originDataFrame.datetime_[index] << ","
             << originDataFrame.bid_[index] << ","
             << originDataFrame.ask_[index] << std::endl;
    }
    file.close();
}

template <class T>
void saveMartinCsv(const std::string &outputFile, const MartinDataFrame<T> &martinDataFrame) {
    std::fstream file(outputFile, std::ios::out);
    file << Key::INDEX << "," << Key::DATETIME << ","
         << Key::BID << "," << Key::ASK << ","
         << Key::OPERATION << "," << Key::CLOSE_TYPE << "," << Key::CLOSE_ARRAY_INDEX << "," << Key::CLOSE_RELATIVE_TIME << ","
         << Key::ADD_POSITION_COUNT;

    for (int i = 0; i < martinDataFrame.martinParameters_.positionIntervals_.size(); i++) {
        file << "," << Key::ADD_POSITION_ARRAY_INDEX << i << ","<<Key::ADD_POSITION_RELATIVE_TIME << i;
    }
    file << std::endl;

    for (std::size_t i = 0; i < martinDataFrame.size(); i++) {
        file << martinDataFrame.index_[i] << ","
             << martinDataFrame.datetime_[i] << ","
             << martinDataFrame.bid_[i] << ","
             << martinDataFrame.ask_[i] << ","
             << toString(martinDataFrame.martinParameters_.op_) << ","
             << toString(martinDataFrame.closeType_[i]) << ","
             << martinDataFrame.closeArrayIndex_[i] << ","
             << martinDataFrame.closeRelativeTime_[i] << ","
             << martinDataFrame.addPositionsArrayIndex_[i].size();

        for (auto it = martinDataFrame.addPositionsArrayIndex_[i].cbegin();
             it != martinDataFrame.addPositionsArrayIndex_[i].cend();
             it++) {
            file << "," << (*it);
        }

        for (auto it = martinDataFrame.addPositionsRelativeTime_[i].cbegin();
             it != martinDataFrame.addPositionsRelativeTime_[i].cend();
             it++) {
            file << "," << (*it);
        }

        file << std::endl;
    }
    file.close();
}

}  // namespace MikuTemplar
#endif