#ifndef MIKU_TEMPLAR_CORE_ORIGIN_DATA_FRAME_H_
#define MIKU_TEMPLAR_CORE_ORIGIN_DATA_FRAME_H_

#include <vector>
#include <string>
#include "core/tick.h"

namespace MikuTemplar{

template <class T>
struct OriginDataFrame {
    std::vector<size_t> index_;
    std::vector<T> bid_;
    std::vector<T> ask_;
    std::vector<std::string> datetime_;

    void assign(const OriginDataFrame<T>& df) {
        this->bid_ = df.bid_;
        this->ask_ = df.ask_;
        this->datetime_ = df.datetime_;
        this->bid_ = df.bid_;
        this->ask_ = df.ask_;
        this->index_ = df.index_;
    }

    const size_t size() const {
        return index_.size();
    }

    inline void append(const Tick<T>& tick) {
        index_.push_back(tick.index_);
        bid_.push_back(tick.bid_);
        ask_.push_back(tick.ask_);
        datetime_.push_back(tick.datatime_);
    }

    Tick<T> operator[](std::size_t idx) const {
        Tick<T> tick;
        tick.index_ = index_[idx];
        tick.bid_ = bid_[idx];
        tick.ask_ = ask_[idx];
        tick.datatime_ = datetime_[idx];
        return tick;
    }
};
}
#endif