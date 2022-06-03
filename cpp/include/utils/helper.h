#ifndef MIKU_TEMPLAR_UTILS_HELPER_H_
#define MIKU_TEMPLAR_UTILS_HELPER_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <ctime>

namespace MikuTemplar{

inline uint64_t getTimeFromEpochSecond(const std::string &str){
    char drop;
    std::tm time;
    std::istringstream ss(str);
    ss >> time.tm_year >> drop >> time.tm_mon >> drop >> time.tm_mday >> drop 
       >> time.tm_hour >> drop >> time.tm_min >> drop >> time.tm_sec >> drop;
    
    return mktime(&time);
}

inline std::vector<std::string> split(const std::string &input, const char spliter=',') {
    std::vector<std::string> result;
    std::stringstream ss(input);
    while( ss.good() ){
        std::string substr;
        std::getline(ss, substr, ',');
        result.push_back( substr );
    }
    return result;
}

template<class T, class F>
std::vector<T> cast(const std::vector<F> &input) {
    std::vector<T> result;
    for (auto it = input.cbegin(); it != input.cend(); it++) {
        result.push_back(*it);
    }
    return result;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrimCopy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrimCopy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trimCopy(std::string s) {
    trim(s);
    return s;
}
}

#endif