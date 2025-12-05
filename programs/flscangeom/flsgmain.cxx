// Standard Library
#include <memory>
#include <cstdlib>

// - Boost
#include "boost/program_options.hpp"

// This Project
#include "falaise/falaise.h"
#include "FLScanGeom.hpp"

int main(int argc_, char ** argv_)
{
  falaise::initialize(argc_, argv_);
  int exit_code = EXIT_SUCCESS;

  FLScanGeom::Config config;
  config.verbosity = datatools::logger::PRIO_DEBUG;
  config.variantProfilePath = "_tmp_simu.profile";
  config.print(std::cerr);
  
  FLScanGeom::Application app;
  app.init(config);
  
  app.run();

  app.reset(); 
  falaise::terminate();
  return exit_code;
}
