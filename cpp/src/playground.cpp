#include "utils/queue_with_max_min.h"
#include "core/io.h"
#include "ortools/linear_solver/linear_solver.h"
#include <iostream>
#include "utils/helper.h"


using namespace std;
using namespace MikuTemplar;
int main(int argc, char *argv[]){
    auto a = getUtcSecond("2018-02-15 10:38:41.130");
    auto b = getUtcSecond("2018-02-15 10:38:48.630");
    cout << b-a << endl;
}