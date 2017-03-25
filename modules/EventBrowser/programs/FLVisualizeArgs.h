// FLVisualizeArgs.h - Interface for FLVisualize command line
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLVISUALIZEARGS_H
#define FLVISUALIZEARGS_H

// Standard library:
#include <string>
#include <vector>
#include <iostream>

// Third party:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/library_loader.h>
#include <bayeux/datatools/configuration/variant_service.h>
#include <bayeux/datatools/multi_properties.h>

namespace FLVisualize {

  //! Collect all needed configuration parameters in one data structure
  struct FLVisualizeArgs
  {
    // Application specific parameters:
    datatools::logger::priority logLevel;             //!< Logging priority threshold
    std::vector<std::string>    libraries;            //!< Libraries to be loaded
    // Experimental setup:
    std::string                 experimentalSetupUrn; //!< Current setup tag
    // Variants support:
    std::string                 variantConfigUrn;     //!< Variants configuration URN
    std::string                 variantProfileUrn;    //!< Variants profile URN
    datatools::configuration::variant_service::config variants; //!< Variants configuration
    // Services support:
    std::string                 servicesConfigUrn; //!< Services configuration URN
    std::string                 servicesConfig;    //!< The main configuration file for the service manager
    // I/O:
    std::string  inputMetadataFile;                //!< Input metadata file
    std::string  inputFile;                        //!< Input data file for the input module
    datatools::multi_properties inputMetadata;     //!< Metadata imported from the input

    // Print:
    void print(std::ostream &) const;

    //! Build a default arguments set:
    static FLVisualizeArgs makeDefault();

    ///! Post process reconstruction parameters
    static void do_postprocess(FLVisualizeArgs &);

  };

  //----------------------------------------------------------------------
  //! Exceptions for configuration
  class FLConfigDefaultError : public std::exception {};
  class FLConfigHelpHandled : public std::exception {};
  class FLConfigUserError : public std::exception {};

  //! Parse command line arguments to configure the simulation parameters
  void do_configure(int argc_, char *argv_[], FLVisualizeArgs& flVisParameters);

} // namespace FLVisualize

#endif // FLVISUALIZEARGS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
