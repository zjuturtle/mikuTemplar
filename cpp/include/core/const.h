#ifndef MIKU_TEMPLAR_CORE_CONST_H_
#define MIKU_TEMPLAR_CORE_CONST_H_

#include <string>

#ifndef DATA_TYPE
#define DATA_TYPE int16_t
#endif

namespace MikuTemplar {
namespace Key {
const std::string INDEX = "index";
const std::string DATETIME = "datetime";
const std::string BID = "bid";
const std::string ASK = "ask";

const std::string OPERATION = "operation";
const std::string CLOSE_ARRAY_INDEX = "closeArrayIndex";
const std::string CLOSE_TYPE = "closeType";
const std::string ADD_POSITION_COUNT = "addPositionCount";
const std::string ADD_POSITION_ARRAY_INDEX = "addPositionArrayIndex";

const std::string FUTURE_BID_MAX_SMALL_WINDOW = "futureBidMaxSmallWindow";
const std::string FUTURE_BID_MIN_SMALL_WINDOW = "futureBidMinSmallWindow";
const std::string FUTURE_BID_MAX_LARGE_WINDOW = "futureBidMaxLargeWindow";
const std::string FUTURE_BID_MIN_LARGE_WINDOW = "futureBidMinLargeWindow";

const std::string FUTURE_ASK_MAX_SMALL_WINDOW = "futureAskMaxSmallWindow";
const std::string FUTURE_ASK_MIN_SMALL_WINDOW = "futureAskMinSmallWindow";
const std::string FUTURE_ASK_MAX_LARGE_WINDOW = "futureAskMaxLargeWindow";
const std::string FUTURE_ASK_MIN_LARGE_WINDOW = "futureAskMinLargeWindow";
}  // namespace Key
}  // namespace MikuTemplar

#endif