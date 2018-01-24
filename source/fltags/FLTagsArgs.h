// FLTagsArgs.h - Interface for FLTags parameters
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLTAGSARGS_H
#define FLTAGSARGS_H

// Standard Library:
#include <string>

// Third Party
// - Bayeux
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/multi_properties.h"

namespace FLTags {

//! Collect all needed configuration parameters in one data structure
struct FLTagsArgs {
  // Application specific parameters:
  datatools::logger::priority logLevel;     //!< Logging priority threshold
  bool dot_with_vertex_index    = false;
  bool dot_without_vertex_category = false;
  bool dot_without_edge_topic      = false;
  bool dot_without_checks       = false;
  std::string dot_outputFile;               //!< Path for the output module

  //! Construct and return the default configuration object
  // Equally, could be supplied in a .application file, though note
  // how some parameters are derived (i.e. there's a postprocessing step)
  static FLTagsArgs makeDefault();

  // Print:
  void print(std::ostream &) const;

};

//! Parse command line arguments to configure the simulation parameters
void do_configure(int argc, char *argv[], FLTagsArgs &params);

}  // namespace FLTags

#endif  // FLTAGSARGS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
