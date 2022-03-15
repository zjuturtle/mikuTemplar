#ifndef MIKU_TEMPLAR_EXT_DATA_FRAME_H_
#define MIKU_TEMPLAR_EXT_DATA_FRAME_H_

#include <vector>
#include <string>
#include "origin_dataframe.h"

namespace MikuTemplar{

template <class T>
struct ExtDataFrame : public OriginDataFrame<T> {
    size_t smallWindow_;
    size_t largeWindow_;

    std::vector<T> futureMaxSmallWindow_;
    std::vector<T> futureMinSmallWindow_;

    std::vector<T> futureMaxLargeWindow_;
    std::vector<T> futureMinLargeWindow_;

    void assign(const OriginDataFrame<T>& df) {
        this->priceName_ = df.priceName_;
        this->datetime_ = df.datetime_;
        this->price_ = df.price_;
        this->index_ = df.index_;
    }
};
}


#endif