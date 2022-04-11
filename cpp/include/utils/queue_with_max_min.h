#ifndef MIKU_TEMPLAR_UTILS_QUEUE_WITH_MAX_MIN_H_
#define MIKU_TEMPLAR_UTILS_QUEUE_WITH_MAX_MIN_H_

#include "utils/stack_with_max_min.h"
namespace MikuTemplar{
template <class T>
class QueueWithMaxMin{
private:
    StackWithMaxMin<T> s0_;
    StackWithMaxMin<T> s1_;
public:
    void push(T data){
        s0_.push(data);
    }

    T pop() {
        if (s1_.isEmpty()) {
            while (!s0_.isEmpty()) {
                s1_.push(s0_.pop());
            }
        }
        return s1_.pop();
    }

    T max(){
        if (s0_.isEmpty()) return s1_.max();
        if (s1_.isEmpty()) return s0_.max();
        return std::max(s0_.max(), s1_.max());
    }

    T min(){
        if (s0_.isEmpty()) return s1_.min();
        if (s1_.isEmpty()) return s0_.min();
        return std::min(s0_.min(), s1_.min());
    }

    std::size_t size(){
        return s0_.size() + s1_.size();
    }
};
}
#endif