// FLReconstructParams.h - Interface for FLReconstruct parameters
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

#ifndef FLRECONSTRUCTPARAMS_H
#define FLRECONSTRUCTPARAMS_H

// Standard Library:
#include <string>

// Third Party
// - Bayeux
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/configuration/variant_service.h"

namespace FLReconstruct {

  //! Collect all needed configuration parameters in one data structure
  struct FLReconstructParams {

    // Application specific parameters:
    datatools::logger::priority logLevel;       //!< Logging priority threshold
    std::string                 userProfile;    //!< User profile
    unsigned int                numberOfEvents; //!< Number of events to be processed in the pipeline
    unsigned int                moduloEvents;   //!< Number of events progress modulo

    // Required experimental setup and versioning:
    std::string  experimentalSetupUrn;        //!< The URN of the experimental setup

    // Variants support:
    std::string                  variantConfigUrn;  //!< Variants configuration URN
    std::string                  variantProfileUrn; //!< Variants profile URN
    //! Variants configuration parameters
    datatools::configuration::variant_service::config variantSubsystemParams;

    // Services support:
    std::string  servicesSubsystemConfigUrn; //!< Services configuration URN
    std::string  servicesSubsystemConfig;    //!< The main configuration file for the service manager

    // Reconstruction control:
    std::string  inputMetadataFile;       //!< Input metadata file
    std::string  inputFile;               //!< Input data file for the input module
    std::string  outputMetadataFile;      //!< Output metadata file
    bool         embeddedMetadata;        //!< Flag to embed metadata in the output data file
    std::string  outputFile;              //!< Output data file for the output module

    // // Description of the data to be processed by the FLReconstruct script:
    // std::string dataType;                 //!< The type of data ("Real", "MC")
    // std::string dataSubtype;              //!< Additional data subtype/flavour ("Blinded", "Calibration", "Commissioning")
    // // For strict checking of the input/output data models:
    // std::vector<std::string>  requiredInputBanks;  //!< Required input data banks ("SD", "UDD"...)
    // std::vector<std::string>  expectedOutputBanks; //!< Expected output data banks ("SSD", "SDD", "EH", "UDD", "CD", "TCD", "TTD", "PTD", "PID"...)

    // Working data:

    // Plugin dedicated service:
    datatools::multi_properties  userLibConfig; //!< Main configuration file for plugins loader

    // Required reconstruction pipeline and versioning:
    std::string reconstructionPipelineUrn;        //!< The URN of the reconstruction pipeline
    std::string reconstructionPipelineConfig;     //!< The reconstruction pipeline main definition file
    std::string reconstructionPipelineModule;     //!< The reconstruction module

    // Metadata container:
    datatools::multi_properties inputMetadata; //!< Metadata imported from the input

     // Processing pipeline modules configuration:
    datatools::multi_properties modulesConfig; //!< Main configuration file for plugins loader

    //! Build a default arguments set:
    static FLReconstructParams makeDefault();

    // Print:
    void print(std::ostream &) const;

  };

} // namespace FLReconstruct

#endif // FLRECONSTRUCTPARAMS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
