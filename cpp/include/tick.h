#ifndef MIKU_TEMPLAR_TICK_H_
#define MIKU_TEMPLAR_TICK_H_

#include <vector>
#include <string>
namespace MikuTemplar{
template <class T>
struct Tick {
    size_t index_;
    T price_;
    std::string datatime_;
};
}

#endif