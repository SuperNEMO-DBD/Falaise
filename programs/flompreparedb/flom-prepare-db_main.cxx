// Standard Library
#include <memory>
#include <cstdlib>
#include <exception>

// - Boost
#include "boost/program_options.hpp"

#include "bayeux/datatools/logger.h"

// This Project
#include "falaise/falaise.h"
#include "FLOmStatus.hpp"

void print_usage();

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int exit_code = EXIT_SUCCESS;

  FLOmStatus::Config config;
  config.verbosity = datatools::logger::PRIO_FATAL;
  config.variantProfilePath = "";
  config.target = "ecalib";

  bool doApp = true;
  
  int iArg = 1;
  while (iArg < argc_) {
    std::string arg(argv_[iArg]);
    // std::clog << "arg = " << std::quoted(arg) << '\n';
    if (arg[0] == '-') {
      // if (arg == "--action") {
      // 	config.actionName = std::string(argv_[++iArg]);
      // 	if (config.actionName == "om_find" or config.actionName == "gg_find") {
      // 	  config.findWhat = std::string(argv_[++iArg]);
      // 	}
      // } else
      if (arg == "--profile") {
	config.variantProfilePath = std::string(argv_[++iArg]);
      } else if (arg == "--calib-version") {
	config.calibVersion = std::string(argv_[++iArg]);
      } else if (arg == "--calib-data-path") {
	config.calibDataPath = std::string(argv_[++iArg]);
      } else if (arg == "--calib-method") {
	config.calibMethod = std::string(argv_[++iArg]);
      } else if (arg == "--om-status-map") {
	config.omStatusPath = std::string(argv_[++iArg]);
      } else if (arg == "--list") {
	config.inputFileList = std::string(argv_[++iArg]);
      } else if (arg == "--output" or arg == "-o") {
	config.outputFile = std::string(argv_[++iArg]);
      } else if (arg == "--format" or arg == "-f") {
	config.outputFormat = std::string(argv_[++iArg]);
      } else if (arg == "--target" or arg == "-t") {
	config.target = std::string(argv_[++iArg]);
      } else if (arg == "--debug") {
	config.verbosity = datatools::logger::PRIO_DEBUG;
      } else if (arg == "--help" or arg == "-h") {
	print_usage();
	doApp = false;
      }
    } else {
      config.inputFiles.push_back(arg);
    }
    iArg++;
  }

  if (config.variantProfilePath.empty()) {
    config.variantProfilePath = "@falaise:snemo/demonstrator/geometry/variants/4.0/profiles/default_geom.profile";
  }


  if (datatools::logger::is_debug(config.verbosity)) {
    std::cerr << "=======================================================\n";
    std::cerr << "Config\n";
    std::cerr << "=======================================================\n";
    config.print(std::cerr);
    std::cerr << "=======================================================\n";
  }

  // doApp = false; 
  if (doApp) {
    try {
      FLOmStatus::Application app;
      app.init(config);
      exit_code = app.run();
      app.reset();
    } catch (std::exception & error) {
      std::cerr << "[error] " << error.what() << std::endl;
    }
  }
  
  falaise::terminate();
  return exit_code;
}

void print_usage()
{
  std::cout << "flomstatus : Build OM Status Change records and OM Calibration records\n";
  std::cout << "\n";
  std::cout << "Options:\n\n";
  // std::cout << "  flomstatus [--debug] args ... : activate debug printing \n";
  // std::cout << "  flomstatus [--profile 'mygeom.profile'] args ... : load a specific geom variant profile\n";
  // std::cout << "                                                   (otherwise, a default version is used)\n";
  // std::cout << "\n";
  // std::cout << "Usage:\n\n";
  // std::cout << "  flgeomap --help    : print this help then exit \n";
  // std::cout << "  flgeomap --action 'om_map' : print the map of OMs' numbers and GIDs \n";
  // std::cout << "  flgeomap --action 'gg_map' : print the map of tracker cells' numbers and GIDs \n";
  // std::cout << "  flgeomap --action 'om_find' {omNumber|omGID} : find the number and GID of a given OM \n";
  // std::cout << "     Examples: flgeomap --action 'om_find' '[1251:0.1.0.10]' : find the OM's number from its GID \n";
  // std::cout << "               flgeomap --action 'om_find' '690' : find the OM's GID from its number GID \n";
  std::cout << "  flomstatus --target ecalib \\\n"
	    << "             --list /data/SuperNEMO/OMCalib/OM_status_calib.lis \n";
  std::cout << "\n";
  std::cout << "\n";

}
