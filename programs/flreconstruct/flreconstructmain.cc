//! \file    flreconstructmain.cc
//! \brief   Main program for flreconstruct command line application
//! \details Configure, setup and run the bayeux::dpp based pipeline
//!          reconstruction of SuperNEMO data.
//!
//!          We need to:
//!          - Construct an input and output module
//!          - Input should select itself based on input file type
//!          - Output is a trashcan (devnull) - Null output_module
//!            should handle this
//!          - Only module for now is a module that cats the output
//!            to screen - dump module should handle this.
//!          - Probably have to hard code some things in...
//!
//!          Need to write our own version of dpp_driver to make
//!          interface usable.
//
// Copyright (c) 2013-2014 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013-2014 by The University of Warwick
// Copyright (c) 2016-2022 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016-2022 by Université de Caen Normandie
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
#include "FLReconstructErrors.h"
#include "FLReconstructImpl.h"
#include "FLReconstructParams.h"
#include "FLReconstructPipeline.h"
#include "FLReconstructApplication.h"
#include "falaise/exitcodes.h"
#include "falaise/falaise.h"

namespace FLReconstruct {

  //! Perform reconstruction using command line args as given
  falaise::exit_code do_flreconstruct(int argc_, char* argv_[]);

}  // end of namespace FLReconstruct

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc_, char* argv_[])
{
  falaise::initialize(argc_, argv_);

  // - Do the reconstruction
  // Ideally, exceptions SHOULD NOT propagate out of this - the error
  // code should be enough, but might want a catch all...
  falaise::exit_code ret = FLReconstruct::do_flreconstruct(argc_, argv_);

  falaise::terminate();
  return ret;
}

namespace FLReconstruct {

  //! Perform reconstruction using command line args as given
  falaise::exit_code do_flreconstruct(int argc_, char * argv_[])
  {
    FLReconstructApplication flRecApp;

    // - Configure
    FLReconstructParams & flRecParameters = flRecApp.parameters;
    try {
      // DT_LOG_DEBUG(datatools::logger::PRIO_ALWAYS, "Configuring the flreconstruct pipeline...");
      do_configure(argc_, argv_, flRecApp);
      DT_LOG_DEBUG(flRecParameters.logLevel, "flreconstruct pipeline is configured.");
    } catch (FLConfigDefaultError& e) {
      std::cerr << "Unable to configure core of flreconstruct" << std::endl;
      return falaise::EXIT_UNAVAILABLE;
    } catch (FLConfigHelpHandled& e) {
      return falaise::EXIT_OK;
    } catch (FLConfigUserError& e) {
      std::cerr << "User configuration error: " << e.what() << std::endl;
      return falaise::EXIT_USAGE;
    }

    // - Run
    falaise::exit_code code = falaise::EXIT_OK;
    DT_LOG_DEBUG(flRecParameters.logLevel, "Running the pipeline...");
    code = do_pipeline(flRecParameters);
    DT_LOG_DEBUG(flRecParameters.logLevel, "Pipeline is done with code=" << code);

    // Terminate:
    do_terminate(flRecApp);

    return code;
  }

} // end of namespace FLReconstruct
