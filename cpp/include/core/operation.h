#ifndef MIKU_TEMPLAR_CORE_OPERATION_H_
#define MIKU_TEMPLAR_CORE_OPERATION_H_


namespace MikuTemplar{
enum Operation{
    BUY, SELL
};

inline std::string toString(const Operation &op) {
    switch (op){
        case Operation::BUY:  return "BUY";
        case Operation::SELL: return "SELL"; 
    }
}

inline Operation generateOperation(const std::string &input) {
    if (input == "buy" || input == "BUY") {
        return Operation::BUY;
    }
    if (input == "sell" || input == "SELL") {
        return Operation::SELL;
    }
    std::cout << "ERROR! invalid input for operation!";
    return Operation::BUY;
}
}

#endif