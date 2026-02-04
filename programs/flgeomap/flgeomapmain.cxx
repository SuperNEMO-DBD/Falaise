// Standard Library
#include <memory>
#include <cstdlib>
#include <exception>

// - Boost
#include "boost/program_options.hpp"

#include "bayeux/datatools/logger.h"

// This Project
#include "falaise/falaise.h"
#include "FLGeoMap.hpp"

void print_usage();

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int exit_code = EXIT_SUCCESS;

  FLGeoMap::Config config;
  config.verbosity = datatools::logger::PRIO_FATAL;
  config.variantProfilePath = "";
  config.actionName = "om_map";

  bool doApp = true;
  
  int iArg = 1;
  while (iArg < argc_) {
    std::string arg(argv_[iArg]);
    if (arg == "--action") {
      config.actionName = std::string(argv_[++iArg]);
      if (config.actionName == "om_find" or config.actionName == "gg_find") {
	config.findWhat = std::string(argv_[++iArg]);
      }
    } else if (arg == "--profile") {
      config.variantProfilePath = std::string(argv_[++iArg]);
    } else if (arg == "--debug") {
      config.verbosity = datatools::logger::PRIO_DEBUG;
    } else if (arg == "--help") {
      print_usage();
      doApp = false;
    }
    iArg++;
  }

  if (datatools::logger::is_debug(config.verbosity)) {
    config.print(std::cerr);
  }

  if (config.variantProfilePath.empty()) {
    config.variantProfilePath = "@falaise:snemo/demonstrator/geometry/variants/4.0/profiles/default_geom.profile";
  }

  if (doApp) {
    try {
      FLGeoMap::Application app;
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
  std::cout << "flgeomap : Print geometrical informations about SuperNEMO\n";
  std::cout << "\n";
  std::cout << "Options:\n\n";
  std::cout << "  flgeomap [--debug] args ... : activate debug printing \n";
  std::cout << "  flgeomap [--profile 'mygeom.profile'] args ... : load a specific geom variant profile\n";
  std::cout << "                                                   (otherwise, a default version is used)\n";
  std::cout << "\n";
  std::cout << "Usage:\n\n";
  std::cout << "  flgeomap --help    : print this help then exit \n";
  std::cout << "  flgeomap --action 'om_map' : print the map of OMs' numbers and GIDs \n";
  std::cout << "  flgeomap --action 'gg_map' : print the map of tracker cells' numbers and GIDs \n";
  std::cout << "  flgeomap --action 'om_find' {omNumber|omGID} : find the number and GID of a given OM \n";
  std::cout << "     Examples: flgeomap --action 'om_find' '[1251:0.1.0.10]' : find the OM's number from its GID \n";
  std::cout << "               flgeomap --action 'om_find' '690' : find the OM's GID from its number GID \n";
  std::cout << "  flgeomap --action 'gg_find' {ggNumber|ggGID} : find the number and GID of a given tracker cell \n";
  std::cout << "     Examples: flgeomap --action 'gg_find' '[1203:0.1.7.109]' : find the  tracker cell's number from its GID \n";
  std::cout << "               flgeomap --action 'gg_find' '2005' : find the  tracker cell's GID from its number GID \n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";

}
