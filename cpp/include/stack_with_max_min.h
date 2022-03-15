#ifndef MIKU_TEMPLAR_STACK_WITH_MAX_MIN_H_
#define MIKU_TEMPLAR_STACK_WITH_MAX_MIN_H_

#include <cstdint>
#include <stack>
namespace MikuTemplar{

template <class T>
class StackWithMaxMin {
    private:
        std::stack<T> stack_;
        std::stack<T> min_stack_;
        std::stack<T> max_stack_;
    public:
        void push(T val) {
            if (this->isEmpty()){
                min_stack_.push(val);
                max_stack_.push(val);
            } else {
                min_stack_.push(std::min(min_stack_.top(), val));
                max_stack_.push(std::max(max_stack_.top(), val));
            }
            stack_.push(val);
        }
        T pop() {
            min_stack_.pop();
            max_stack_.pop();
            auto res = stack_.top();
            stack_.pop();
            return res;
        }
        T min() {
            return min_stack_.top();
        }
        T max() {
            return max_stack_.top();
        }
        std::size_t size() {
            return stack_.size();
        }
        bool isEmpty() {
            return stack_.empty();
        }
};

}
#endif