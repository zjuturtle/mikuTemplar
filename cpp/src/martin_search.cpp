#include <string>
#include <vector>
#include <limits>

#include "core/ext_dataframe.h"
#include "core/io.h"
#include "core/martin_dataframe.h"
#include "core/martin_simulator.h"
#include "core/preprocess_dataframe.h"
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
      ("output_martin_infos", "for each martin paramter the simulate result", cxxopts::value<string>())
      ("h,help", "Print usage");

    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    cout << "[INFO]Loading data..." << endl;
    auto martinParametersList = loadMartinParametersJson(args["input_search"].as<string>());

    auto extDataFrame = loadExtCsv<DATA_TYPE>(args["input_ext"].as<string>());
    auto openPreprocessDataFrame = loadPreprocessCsv<DATA_TYPE>(args["input_open"].as<string>());

    MartinSimulator mSim(extDataFrame);
    MartinOptimizer mo;
    MartinDataFrame<DATA_TYPE> bestMartinDataFrame;
    double bestProfit = numeric_limits<double>::min();
    vector<MartinInfo> martinInfos;
    for (auto &martinParameters : martinParametersList) {
        auto martinDataFrame = mSim.run(openPreprocessDataFrame, martinParameters);
        auto martinInfo = martinDataFrame.analyze(mo);
        martinInfos.push_back(martinInfo);
        if (!martinInfo.s_.bestLots_.empty() && martinInfo.s_.bestProfit_ > bestProfit) {
            bestProfit = martinInfo.s_.bestProfit_;
            bestMartinDataFrame = martinDataFrame;
        }
    }
    saveMartinInfos(args["output_martin_infos"].as<string>(), martinInfos);
    saveMartinCsv(args["output_best"].as<string>(), bestMartinDataFrame);  
    cout << "[INFO]All done" << endl;
}