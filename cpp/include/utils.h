#ifndef MIKU_TEMPLAR_UTILS_H_
#define MIKU_TEMPLAR_UTILS_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <algorithm> 
#include <cctype>
#include <locale>

namespace MikuTemplar{

std::vector<std::string> split(const std::string &input, const char spliter=',') {
    std::vector<std::string> result;
    std::stringstream ss(input);
    while( ss.good() ){
        std::string substr;
        std::getline(ss, substr, ',');
        result.push_back( substr );
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