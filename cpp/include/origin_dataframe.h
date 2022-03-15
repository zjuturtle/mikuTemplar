#ifndef MIKU_TEMPLAR_ORIGIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_ORIGIN_DATA_FRAME_H_

#include <vector>
#include <string>
#include "tick.h"

namespace MikuTemplar{

template <class T>
struct OriginDataFrame {
    std::string priceName_;

    std::vector<size_t> index_;
    std::vector<T> price_;
    std::vector<std::string> datetime_;

    const size_t size() const {
        return index_.size();
    }

    inline void append(const Tick<T>& tick) {
        index_.push_back(tick.index_);
        price_.push_back(tick.price_);
        datetime_.push_back(tick.datatime_);
    }

    Tick<T>& operator[](std::size_t idx) {
        Tick<T> tick;
        tick.index_ = index_[idx];
        tick.price_ = price_[idx];
        tick.datatime_ = datetime_[idx];
        return tick;
    }

    Tick<T> operator[](std::size_t idx) const {
        Tick<T> tick;
        tick.index_ = index_[idx];
        tick.price_ = price_[idx];
        tick.datatime_ = datetime_[idx];
        return tick;
    }
};
}
#endif