#ifndef MIKU_TEMPLAR_UTILS_TIMER_H_
#define MIKU_TEMPLAR_UTILS_TIMER_H_
#include <chrono>
#include <string>

namespace MikuTemplar{
class Timer{
private:
    std::string name_;
    std::chrono::time_point<std::chrono::system_clock> start_;
    std::chrono::time_point<std::chrono::system_clock> end_;
public:
    Timer(std::string name="defaultTimer"){
        this->name_ = name;
    }

    void start(){
        this->start_ = std::chrono::system_clock::now();
    }

    void end() {
        this->end_ = std::chrono::system_clock::now();
    }

    double elapsedSeconds() {
        std::chrono::duration<double> tmp = end_ - start_;
        return tmp.count();
    }
};

}

#endif