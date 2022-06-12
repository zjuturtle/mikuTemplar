#ifndef MIKU_TEMPLAR_CORE_EXT_DATA_FRAME_H_
#define MIKU_TEMPLAR_CORE_EXT_DATA_FRAME_H_

#include <string>
#include <vector>

#include "core/origin_dataframe.h"

namespace MikuTemplar {

template <class T>
struct ExtDataFrame : public OriginDataFrame<T> {
    int smallWindow_;
    int largeWindow_;

    std::vector<T> futureBidMaxSmallWindow_;
    std::vector<T> futureBidMinSmallWindow_;
    std::vector<T> futureBidMaxLargeWindow_;
    std::vector<T> futureBidMinLargeWindow_;

    std::vector<T> futureAskMaxSmallWindow_;
    std::vector<T> futureAskMinSmallWindow_;
    std::vector<T> futureAskMaxLargeWindow_;
    std::vector<T> futureAskMinLargeWindow_;
};
}  // namespace MikuTemplar

#endif