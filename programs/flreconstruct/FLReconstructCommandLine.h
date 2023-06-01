// FLReconstructCommandLine.h - Interface for FLReconstruct command line arguments
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

#ifndef FLRECONSTRUCTCOMMANDLINE_H
#define FLRECONSTRUCTCOMMANDLINE_H

// Standard Library:
#include <stdexcept>
#include <string>
#include <vector>

// Third Party
// - Boost
#include "boost/program_options.hpp"
// - Bayeux
#include "bayeux/datatools/library_loader.h"
#include "bayeux/datatools/logger.h"

// This project
#include "FLReconstructErrors.h"

namespace FLReconstruct {

  //! \brief Command line arguments
  //!
  //! The parameters we can receive from the command line
  //! Help and so on are not marked because these are handled by the UI.
  struct FLReconstructCommandLine
  {
    datatools::logger::priority logLevel;  //!< Verbosity level
    uint32_t maxNumberOfEvents;            //!< Maximum number of processed events
    uint32_t moduloEvents;                 //!< Event modulo
    std::string userProfile;               //!< User profile
    std::vector<std::string> mountPoints;  //!< Directory mount directives
    std::string configScript;              //!< Path of the main reconstruction configuration script
    std::string inputMetadataFile;         //!< Path for loading metadata
    std::string inputFile;                 //!< Path for the input module
    std::string outputMetadataFile;        //!< Path for saving metadata
    std::string outputFile;                //!< Path for the output module

    //! Build a default arguments set:
    static FLReconstructCommandLine makeDefault();

    // Print:
    void print(std::ostream &) const;
  };

  //! Handle printing of version information to given ostream
  void do_version(std::ostream & os_, bool isVerbose_);

  //! Handle printing of help message to given ostream
  void do_help(std::ostream & os_, const boost::program_options::options_description & od_);

  //! Print list of known module names, one per line, to given stream
  void do_module_list(std::ostream & os_);

  //! Print OCD help for supplied module name to given ostream
  void do_help_module(std::ostream & os_, const std::string & module_);

  //! Print list of standard pipeline configurations to supplied ostream
  void do_help_pipeline_list(std::ostream & os_);

  //! load all default plugins
  void do_load_plugins(datatools::library_loader & libLoader_);

  //! Command line dialog
  FLDialogState do_cldialog(int argc_, char* argv_[], FLReconstructCommandLine & args_);

} // namespace FLReconstruct

#endif // FLRECONSTRUCTCOMMANDLINE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
