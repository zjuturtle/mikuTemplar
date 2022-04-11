#include "utils/cxxopt.hpp"
#include "core/io.h"
#include "core/ext_dataframe.h"
#include "core/martin_simulator.h"
#include "core/origin_dataframe.h"
#include "core/martin_dataframe.h"
#include <string>
#include <vector>

using namespace MikuTemplar;
using namespace std;

int main(int argc, char *argv[]){
    cxxopts::Options options("martin search", "Simulate martin process for each input open row.");
    options.add_options()
        ("input_ext", "input ext csv file", cxxopts::value<string>())
        ("input_search", "input search json file", cxxopts::value<string>())
        ("output", "output martin result csv file path", cxxopts::value<string>())
        ("h,help", "Print usage")
    ;

    auto result = options.parse(argc, argv);
    if (result.count("help")){
      std::cout << options.help() << std::endl;
      exit(0);
    }
    cout << "[INFO]Loading data..." << endl;
    auto operation = generateOperation(result["operation"].as<string>());
    auto extDataFrame = loadExtCsv<DATA_TYPE>(result["input_ext"].as<string>());

    cout  << "[INFO]All done"<< endl;
}