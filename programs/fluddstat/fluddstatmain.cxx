// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <iomanip>
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

  FLUddStat::FLUddStatConfig appConfig;

  try {
    int iArg = 1;
    while (iArg < argc_) {
      std::string token = argv_[iArg++];
      std::clog << "[log] token=" << std::quoted(token) << '\n';
      if (token[0] == '-') {
	std::string option = token;
	if (option == "--debug" or option == "-d") {
	  appConfig.verbosity = datatools::logger::PRIO_DEBUG;
	} else if (option == "--max-nb-events" or option == "-M") {
	  appConfig.maxNbEvents = std::stoi(argv_[iArg++]);
	}
      } else {
	std::string argument = token;
	if (appConfig.inputUddFilePath.empty()) {
	  appConfig.inputUddFilePath = argument;
	} else if (appConfig.outputDirPath.empty()) {
	  appConfig.outputDirPath = argument;
	}
      }
    }
    // if (argc_ < 2) {
    //   throw std::logic_error("missing arguments\nUsage: fluddstat uddfile.brio [udd.stat]");
    // }
    // appConfig.inputUddFilePath   = argv_[1]; // "/opt/SW/SuperNEMO-DBD/data/UDD/snemo_run-2075_udd.brio";
    // if (argc_ >= 3) {
    //   appConfig.outputStatFilePath = argv_[2]; // "snemo_run-2075_udd.stat";
    // } else {

    if (appConfig.outputDirPath.empty()) {
      appConfig.outputDirPath = "_udd_channel_mon-0000/";
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
