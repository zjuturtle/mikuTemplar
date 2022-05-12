#include <string>
#include <vector>
#include <limits>

#include "core/ext_dataframe.h"
#include "core/io.h"
#include "core/martin_dataframe.h"
#include "core/martin_simulator.h"
#include "core/origin_dataframe.h"
#include "core/martin_optimizer.h"
#include "utils/cxxopt.hpp"

using namespace MikuTemplar;
using namespace std;

int main(int argc, char *argv[]) {
    cxxopts::Options options("martin search", "Simulate martin process for each input open row.");
    options.add_options()
      ("input_ext", "input ext csv file", cxxopts::value<string>())
      ("input_open", "input open csv file. must included in input_ext", cxxopts::value<string>())
      ("input_search", "input search json file", cxxopts::value<string>())
      ("output_best", "output best martin dataframe csv file", cxxopts::value<string>())
      ("h,help", "Print usage");

    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    cout << "[INFO]Loading data..." << endl;
    auto martinParametersList = loadMartinParametersJson(result["input_search"].as<string>());

    auto extDataFrame = loadExtCsv<DATA_TYPE>(result["input_ext"].as<string>());
    auto openOriginDataFrame = loadOriginCsv<DATA_TYPE>(result["input_open"].as<string>());

    MartinSimulator mSim(extDataFrame);
    MartinOptimizer mo;
    MartinDataFrame<DATA_TYPE> bestMartinDataFrame;
    double bestProfit = numeric_limits<double>::min();
    for (auto &martinParameters : martinParametersList) {
        auto martinDataFrame = mSim.run(openOriginDataFrame, martinParameters);
        auto martinInfo = martinDataFrame.analyze(mo);

        if (martinInfo.s_.bestProfit_ > bestProfit) {
            bestProfit = martinInfo.s_.bestProfit_;
            bestMartinDataFrame = martinDataFrame;
        }
    }
    saveMartinCsv(result["output_best"].as<string>(), bestMartinDataFrame);  
    cout << "[INFO]All done" << endl;
}