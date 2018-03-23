//! \file    fltagsmain.cc
//! \brief   Generate a graph of the hierarchy of published tagged setup.
//!          Export to DOT format for visualization with external tools.
// TODO
//  - implement the generation of partial subgraphs of tags:
//    - all depender tags wich used a dependee tag:
//      $ ftags --subgraph-dependees-of "depender tag"
//    - all dependee tags used by a depender tag:
//      $ ftags --subgraph-dependers-of "depender tag"
//  - various filters to build list of tags in some specific categories
//
// Copyright (c) 2018 François Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard Library:
#include <string>
#include <memory>

// Third Party:
// - Boost:
#include "boost/filesystem.hpp"

// - Bayeux:
#include "bayeux/bayeux.h"
#include "bayeux/version.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/utils.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/datatools/dependency_graph.h"

// This Project:
#include "falaise/falaise.h"
#include "falaise/version.h"
#include "falaise/exitcodes.h"

// This App:
#include "FLTagsArgs.h"
#include "FLTagsErrors.h"
#include "FLTagsUtils.h"
#include "FLTags.h"

namespace FLTags {

  //! Perform simulation using command line args as given
  falaise::exit_code do_fltags(int argc, char *argv[]);

}  // end of namespace FLTags

//----------------------------------------------------------------------
// MAIN PROGRAM
//----------------------------------------------------------------------
int main(int argc, char *argv[])
{
  falaise::initialize();

  // - Do the simulation.
  // Ideally, exceptions SHOULD NOT propagate out of this  - the error
  // code should be enough.
  falaise::exit_code ret = FLTags::do_fltags(argc, argv);

  falaise::terminate();
  return ret;
}

namespace FLTags {

  //----------------------------------------------------------------------
  falaise::exit_code do_fltags(int argc, char *argv[])
  {

    // - Configure:
    FLTagsArgs flTagsParameters;
    try {
      do_configure(argc, argv, flTagsParameters);
    } catch (FLConfigDefaultError &e) {
      std::cerr << "Unable to configure core of fltags" << std::endl;
      return falaise::EXIT_UNAVAILABLE;
    } catch (FLConfigHelpHandled &e) {
      return falaise::EXIT_OK;
    } catch (FLConfigUserError &e) {
      std::cerr << "User configuration error: " << e.what() << std::endl;
      return falaise::EXIT_USAGE;
    }

    // - Run:
    falaise::exit_code code = falaise::EXIT_OK;

    try {

      // Initialize and run the application:
      FLTags::fltags tags(flTagsParameters);
      tags.run();

    } catch (std::exception & e) {
      std::cerr << "fltags : Setup/run threw exception" << std::endl;
      std::cerr << e.what() << std::endl;
      code = falaise::EXIT_UNAVAILABLE;
    }

    return code;
  }

}  // end of namespace FLTags
