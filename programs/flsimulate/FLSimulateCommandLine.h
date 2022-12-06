// FLSimulateCommandLine.h - Interface for FLSimulate command line arguments and processing
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2017-2022 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017-2022 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLSIMULATECOMMANDLINE_H
#define FLSIMULATECOMMANDLINE_H

// Standard Library:
#include <string>
#include <vector>

// Third Party
// - Boost
#include "boost/program_options.hpp"
// - Bayeux
#include "bayeux/datatools/logger.h"

namespace FLSimulate {

  //! \brief The parameters we can receive from the command line
  // Help and so on are not marked because these are handled by the UI.
  struct FLSimulateCommandLine
  {
    datatools::logger::priority logLevel;  //!< Logging priority threshold
    std::string userProfile;               //!< User profile
    std::vector<std::string> mountPoints;  //!< Directory mount directives
    std::string configScript;              //!< Path to configuration script
    std::string outputMetadataFile;        //!< Path for saving metadata
    bool embeddedMetadata;                 //!< Flag to embed metadata in the output data file
    std::string outputFile;                //!< Path for the output module
    unsigned int numberOfEvents;           //!< Number of events to be generated
    static FLSimulateCommandLine makeDefault();
  };

  //! Handle printing of version information to given ostream
  void do_version(std::ostream & os_, bool isVerbose_);

  //! Handle printing of help message to screen
  void do_help(const boost::program_options::options_description & od_);

  //! Command line help on script schema
  void do_help_scripting(std::ostream & os_);

  //! Command line help on simulation setup
  void do_help_simulation_setup(std::ostream & os_);

  //! Handle command line argument dialog
  void do_cldialog(int argc_, char * argv_[], FLSimulateCommandLine & clArgs_);

}  // namespace FLSimulate

#endif  // FLSIMULATECOMMANDLINE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
