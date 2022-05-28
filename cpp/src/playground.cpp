#include "utils/queue_with_max_min.h"
#include "core/io.h"
#include "ortools/linear_solver/linear_solver.h"
#include <iostream>


using namespace std;
using namespace MikuTemplar;
int main(int argc, char *argv[]){
    MartinInfo m;
    auto tt = loadMartinParametersJson("/Users/turtle/github/mikuTemplar/data/EURUSD/search.json");
    m.p_ = tt[0];
    vector<MartinInfo> mL;
    mL.push_back(m);
    saveMartinInfos("/Users/turtle/github/mikuTemplar/data/tt.json", mL);

}