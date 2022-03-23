#ifndef MIKU_TEMPLAR_TICK_H_
#define MIKU_TEMPLAR_TICK_H_

#include <vector>
#include <string>
namespace MikuTemplar{
template <class T>
struct Tick {
    size_t index_;
    T bid_;
    T ask_;
    std::string datatime_;
};

template <class T>
struct TickExt : public Tick<T>{
    T futureBidMaxSmallWindow_;
    T futureBidMinSmallWindow_;
    T futureBidMaxLargeWindow_;
    T futureBidMinLargeWindow_;

    T futureAskMaxSmallWindow_;
    T futureAskMinSmallWindow_;
    T futureAskMaxLargeWindow_;
    T futureAskMinLargeWindow_;
};
}

#endif