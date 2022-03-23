#ifndef MIKU_TEMPLAR_MARTIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_MARTIN_DATA_FRAME_H_

#include <vector>
#include <string>
#include "origin_dataframe.h"

namespace MikuTemplar{

enum Operation{
    BUY, SELL
};

template <class T>
struct MartinDataFrame : public OriginDataFrame<T> {
    size_t smallWindow_;
    size_t largeWindow_;

    std::vector<T> futureMaxSmallWindow_;
    std::vector<T> futureMinSmallWindow_;

    std::vector<T> futureMaxLargeWindow_;
    std::vector<T> futureMinLargeWindow_;
};
}


#endif