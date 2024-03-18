// FLSimulateArgs.h - Interface for FLSimulate parameters
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie
// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLSIMULATEARGS_H
#define FLSIMULATEARGS_H

// Standard Library:
#include <string>
#include <vector>

// Third Party
// - Bayeux
#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/event_id.h"
#include "bayeux/datatools/multi_properties.h"
#include "bayeux/mctools/g4/manager_parameters.h"

namespace FLSimulate {

  //! Collect all needed configuration parameters in one data structure
  struct FLSimulateArgs
  {
    // Application specific parameters:
    datatools::logger::priority logLevel; //!< Logging priority threshold
    std::string userProfile;              //!< User profile
    std::vector<std::string> mountPoints; //!< Directory mount directives
    unsigned int numberOfEvents;          //!< Number of events to be processed in the pipeline
		int runNumber = datatools::event_id::ANY_RUN_NUMBER;
 		unsigned int firstEventNumber = 0u;
    // Plugin dedicated service:
    datatools::multi_properties userLibConfig; //!< Main configuration for plugins loader

    bool doSimulation;                //!< Simulation flag
    bool doDigitization;              //!< Digitization flag
    /// 2022-10-25, FM : Obsolete ?
    std::string experimentalSetupUrn; //!< The URN of the experimental setup of type "expsetup" (possibly extracted from the simulation setup)

    // Simulation module setup:
    std::string simulationSetupUrn; //!< The URN of the simulation engine setup
    /// Parameters for the Geant4 simulation manager
    mctools::g4::manager_parameters simulationManagerParams;  

    // Digitization module setup:
    std::string digitizationSetupUrn; //!< The URN of the digitization module setup

    // Variants support:
    std::string variantConfigUrn;  //!< Variants configuration URN
    std::string variantProfileUrn; //!< Variants profile URN
    bool saveVariantSettings;      //!< Flag to save effective variant settings in metadata
    /// Variants configuration parameters
    datatools::configuration::variant_service::config variantServiceConfig; 

    // Services support:
    std::string servicesSubsystemConfigUrn; //!< Services configuration URN
    std::string servicesSubsystemConfig;    //!< The main configuration file for the service manager

    // Simulation control:
    std::string outputMetadataFile; //!< Output metadata file
    bool embeddedMetadata;          //!< Flag to embed metadata in the output data file
    bool saveRngSeeding;            //!< Flag to save PRNG seeds in metadata
    std::string rngSeeding;         //!< PRNG seed initialization
    std::string outputFile;         //!< Output data file for the output module

    //! Construct and return the default configuration object
    // Equally, could be supplied in a .application file, though note
    // how some parameters are derived (i.e. there's a postprocessing step)
    static FLSimulateArgs makeDefault();

    // Print:
    void print(std::ostream &) const;

    // Return the default file for output metadata
    static const std::string & default_file_for_output_metadata();

    // Return the default file output metadata file
    static const std::string & default_file_for_seeds();
    
  };

  //! Parse command line arguments to configure the simulation parameters
  void do_configure(int argc_, char * argv_[], FLSimulateArgs & flSimParameters_);

}  // namespace FLSimulate

#endif  // FLSIMULATEARGS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
