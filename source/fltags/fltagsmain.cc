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

      // Do something
      {
        datatools::kernel & dtk = datatools::kernel::instance();
        // const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();
        datatools::urn_query_service & dtkUrnQuery = dtk.grab_urn_query();
        dtkUrnQuery.set_logging_priority(datatools::logger::PRIO_DEBUG);
        dtkUrnQuery.tree_dump(std::clog, dtkUrnQuery.get_name(), "[info] ");
        std::clog << "\nBuild the dependency graph:" << std::endl;
        const datatools::dependency_graph & dg = dtkUrnQuery.get_dependency_graph();
        std::string dot_output_file = flTagsParameters.dot_outputFile;
        std::ostream * out = nullptr;
        std::unique_ptr<std::ofstream> hfout;
        if (dot_output_file == "-") {
          out = &std::cout;
        } else {
          if (dot_output_file.empty()) {
            dot_output_file = FLTags::default_dot_filename();
          }
          datatools::fetch_path_with_env(dot_output_file);
          hfout.reset(new std::ofstream(dot_output_file.c_str()));
          out = hfout.get();
        }
        uint32_t xgv_options = 0;
        if (flTagsParameters.dot_with_vertex_index) {
          xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_INDEX;
        }
        if (flTagsParameters.dot_with_vertex_category) {
          xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_CATEGORY;
        }
        if (flTagsParameters.dot_with_edge_topic) {
          xgv_options |= datatools::dependency_graph::XGV_WITH_EDGE_TOPIC;
        }
        dg.export_graphviz(*out, xgv_options);
        out = nullptr;
        hfout.reset();

        std::clog << std::endl;
        std::clog << "***********************************************" << std::endl;
        std::clog << "The DOT file '" << dot_output_file << "' has been generated. " << std::endl;
        std::clog << "Please use a graphviz-like tool to render the graph." << std::endl;
        std::clog << "Examples: " << std::endl;
        std::clog << std::endl;
        std::clog << " 1) Using xdot :" << std::endl;
        std::clog << std::endl;
        std::clog << "    $ xdot " << dot_output_file << " &" << std::endl;
        std::clog << std::endl;
        std::clog << " 2) Using Python graphviz module :" << std::endl;
        std::clog << std::endl;
        std::clog << "    $ python " << std::endl;
        std::clog << "    >>> import pygraphviz as pgv " << std::endl;
        std::clog << "    >>> B=pgv.AGraph('" << dot_output_file << "')" << std::endl;
        std::clog << "    >>> B.layout('dot')" << std::endl;
        std::clog << "    >>> B.draw('fltags.png')" << std::endl;
        std::clog << "    >>> ^D" << std::endl;
        std::clog << "    $ xdg-open fltags.png & " << std::endl;
        std::clog << std::endl;
        std::clog << "***********************************************" << std::endl;
        std::clog << std::endl;

      }

    } catch (std::exception & e) {
      std::cerr << "fltags : Setup/run threw exception" << std::endl;
      std::cerr << e.what() << std::endl;
      code = falaise::EXIT_UNAVAILABLE;
    }

    return code;
  }

}  // end of namespace FLTags
