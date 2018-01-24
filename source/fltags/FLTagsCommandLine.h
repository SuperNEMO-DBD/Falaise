// FLTagsCommandLine.h - Interface for FLTags command line arguments and processing
//
// Copyright (c) 2018 François Mauger <mauger@lpccaen.in2p3.fr>

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLTAGSCOMMANDLINE_H
#define FLTAGSCOMMANDLINE_H

// Standard Library:
#include <string>
#include <vector>

// Third Party
// - Boost
#include "boost/program_options.hpp"
// - Bayeux
#include "bayeux/datatools/logger.h"

namespace FLTags {

  //! \brief The parameters we can receive from the command line
  // Help and so on are not marked because these are handled by the UI.
  struct FLTagsCommandLine {
    datatools::logger::priority logLevel = datatools::logger::PRIO_FATAL;  //!< Logging priority threshold
    std::string action               = "";
    bool list_with_tree              = false;
    bool dot_with_vertex_index       = false;
    bool dot_without_vertex_category = false;
    bool dot_without_edge_topic      = false;
    bool dot_without_checks          = false;
    std::string outputFile; //!< Path for the output file
    static FLTagsCommandLine makeDefault();
  };

  //! Handle printing of version information to given ostream
  void do_version(std::ostream& os, bool isVerbose);

  //! Handle printing of help message to screen
  void do_help(const boost::program_options::options_description& od);

  //! Handle command line argument dialog
  void do_cldialog(int argc, char* argv[], FLTagsCommandLine& clArgs);

}  // namespace FLTags

#endif  // FLTAGSCOMMANDLINE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
