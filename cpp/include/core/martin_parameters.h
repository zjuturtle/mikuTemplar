#ifndef MIKU_TEMPLAR_CORE_MARTIN_PARAMETERS_H_
#define MIKU_TEMPLAR_CORE_MARTIN_PARAMETERS_H_
#include <vector>

#include "core/const.h"
#include "core/operation.h"

namespace MikuTemplar {
template <class T>
struct MartinParameters {
    std::vector<T> positionIntervals_;
    std::vector<T> stopProfits_;
    T stopLoss_;
    Operation op_;
};
}  // namespace MikuTemplar

#endif