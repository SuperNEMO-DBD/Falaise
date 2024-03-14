//! \file    flreconstructmain.cc
//! \brief   Main program for converting pCD data to CpCD data
//
// Copyright (c) 2024 by François Mauger, Emmanuel Chauveau
// Copyright (c) 2024 SuperNEMO Collaboration
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

// Standard Library
#include <memory>

// Third Party
// - Bayeux
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/logger.h"

// - Boost
#include "boost/program_options.hpp"

// This Project
#include "FLpCDToCpCDErrors.h"
#include "FLpCDToCpCDImpl.h"
#include "FLpCDToCpCDParams.h"
#include "FLpCDToCpCDPipeline.h"
#include "FLpCDToCpCDApplication.h"
#include "falaise/exitcodes.h"
#include "falaise/falaise.h"

namespace FLpCDToCpCD {

  //! Perform reconstruction using command line args as given
  falaise::exit_code do_flreconstruct(int argc_, char* argv_[]);

}  // end of namespace FLpCDToCpCD

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc_, char* argv_[])
{
  falaise::initialize(argc_, argv_);

  // - Do the reconstruction
  // Ideally, exceptions SHOULD NOT propagate out of this - the error
  // code should be enough, but might want a catch all...
  falaise::exit_code ret = FLpCDToCpCD::do_flreconstruct(argc_, argv_);

  falaise::terminate();
  return ret;
}

namespace FLpCDToCpCD {

  //! Perform reconstruction using command line args as given
  falaise::exit_code do_flreconstruct(int argc_, char * argv_[])
  {
    FLpCDToCpCDApplication flRecApp;

    // - Configure
    FLpCDToCpCDParams & flRecParameters = flRecApp.parameters;
    try {
       do_configure(argc_, argv_, flRecApp);
      DT_LOG_DEBUG(flRecParameters.logLevel, "flpcd2cpcd pipeline is configured.");
    } catch (FLConfigDefaultError& e) {
      std::cerr << "Unable to configure core of flpcd2cpcd" << std::endl;
      return falaise::EXIT_UNAVAILABLE;
    } catch (FLConfigHelpHandled& e) {
      return falaise::EXIT_OK;
    } catch (FLConfigUserError& e) {
      std::cerr << "User configuration error: " << e.what() << std::endl;
      return falaise::EXIT_USAGE;
    }

    // - Run
    falaise::exit_code code = falaise::EXIT_OK;
    DT_LOG_DEBUG(flRecParameters.logLevel, "Running the flpcd2cpcd pipeline...");
    code = do_pipeline(flRecParameters);
    DT_LOG_DEBUG(flRecParameters.logLevel, "Pipeline is done with code=" << code);

    // Terminate:
    do_terminate(flRecApp);

    return code;
  }

} // end of namespace FLpCDToCpCD
