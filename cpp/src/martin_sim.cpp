#include "utils/cxxopt.hpp"
#include "core/io.h"
#include "core/ext_dataframe.h"
#include "core/martin_simulator.h"
#include "core/origin_dataframe.h"
#include "core/martin_dataframe.h"
#include "core/martin_parameters.h"
#include <string>
#include <vector>

using namespace MikuTemplar;
using namespace std;

int main(int argc, char *argv[]){
    cxxopts::Options options("martin sim", "Simulate martin process for each input open row.");
    options.add_options()
        ("input_ext", "input ext csv file", cxxopts::value<string>())
        ("input_open","input open csv file. must included in input_ext", cxxopts::value<string>())
        ("operation", "open direction, buy or sell", cxxopts::value<string>())
        ("output", "output martin result csv file path", cxxopts::value<string>())
        ("martin_position_intervals", "martin add position intervals, in list form", cxxopts::value<vector<int>>())
        ("martin_stop_profits","martin stop profit target, in list form", cxxopts::value<vector<int>>())
        ("martin_stop_loss", "martin stop loss target", cxxopts::value<int>())
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
    auto openOriginDataFrame = loadOriginCsv<DATA_TYPE>(result["input_open"].as<string>());
    cout << "[INFO]Finish data loading" << endl;

    MartinSimulator mSim(extDataFrame);
    
    auto martinDataFrame = mSim.run(openOriginDataFrame, operation, 
        cast<DATA_TYPE>(result["martin_position_intervals"].as<vector<int>>()), 
        cast<DATA_TYPE>(result["martin_stop_profits"].as<vector<int>>()),
        static_cast<DATA_TYPE>(result["martin_stop_loss"].as<int>()));

    cout << "[INFO]Finish martin simulate. Writing to file..." << endl;
    saveMartinCsv(result["output"].as<string>(), martinDataFrame);
    cout  << "[INFO]All done"<< endl;
}