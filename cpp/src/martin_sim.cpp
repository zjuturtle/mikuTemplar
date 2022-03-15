#include "cxxopt.hpp"
#include <string>

using namespace std;

int main(int argc, char *argv[]){
    cxxopts::Options options("martin sim", "Simulate martin process for each second");
    options.add_options()
        ("i,input", "input ext csv file", cxxopts::value<std::string>())
        ("o,output", "output martin result csv file path", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;

    auto tmp = loadCsv(result["input"].as<string>());    
}