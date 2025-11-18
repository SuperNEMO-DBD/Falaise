// Standard Library
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// - Boost
#include <boost/program_options.hpp>

// This Project
#include <falaise/falaise.h>
#include "FlUddStat.hh"

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int exit_code = EXIT_SUCCESS;

  try {
    if (argc_ < 2) {
      throw std::logic_error("missing arguments\nUsage: fluddstat uddfile.brio [udd.stat]");
    }
    FLUddStat::FLUddStatConfig appConfig;
    appConfig.inputUddFilePath   = argv_[1]; // "/opt/SW/SuperNEMO-DBD/data/UDD/snemo_run-2075_udd.brio";
    if (argc_ >= 3) {
      appConfig.outputStatFilePath = argv_[2]; // "snemo_run-2075_udd.stat";
    } else {
      appConfig.outputStatFilePath = "udd.stat";
    }

    FLUddStat::FLUddStatApp app(appConfig);
  
    exit_code = app.run();
  } catch (std::exception & error) {
    std::cerr << "[error] " << error.what() << std::endl;
    exit_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return exit_code;
}
