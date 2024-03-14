// FLpCDToCpCDParams.h - Interface for FLpCDToCpCD parameters
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2017-2024 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017-2024 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLPCDTOCPCDPARAMS_H
#define FLPCDTOCPCDPARAMS_H

// Standard Library:
#include <string>
#include <vector>

// Third Party
// - Bayeux
#include "bayeux/datatools/configuration/variant_service.h"
#include "bayeux/datatools/logger.h"
#include "bayeux/datatools/multi_properties.h"

namespace FLpCDToCpCD {

  //! Collect all needed configuration parameters in one data structure
  struct FLpCDToCpCDParams
  {
    std::string processConfig; //!< The pcd2cpcd process main configuration file

    // Required pcdtocpcd process setup and versioning:
		std::string processSetupUrn;    //!< The URN of the pcd2cpcd setup

    // Application specific parameters:
    datatools::logger::priority logLevel;  //!< Logging verbosity threshold
    std::string userProfile;               //!< User profile
    std::vector<std::string> mountPoints;  //!< Directory mount directives
    unsigned int numberOfEvents;           //!< Number of events to be processed
    unsigned int moduloEvents;             //!< Number of events progress modulo
		bool preserveUnclusteredEvents = false;
		bool preserveUnclusteredHits = false; // unused
		
    // Required experimental setup and versioning:
    std::string experimentalSetupUrn;  //!< The URN of the experimental setup

    // Variants support:
    std::string variantConfigUrn;   //!< Variants configuration URN
    std::string variantProfileUrn;  //!< Variants profile URN
    //! Variants configuration parameters
    datatools::configuration::variant_service::config variantSubsystemParams;

    // Services support:
    std::string servicesSubsystemConfigUrn;  //!< Services configuration URN
    std::string servicesSubsystemConfig;     //!< The main configuration file for the service manager

    // pCDToCpCDion control:
    std::string inputMetadataFile;   //!< Input metadata file
    std::string inputFile;           //!< Input data file for the input module
    std::string outputMetadataFile;  //!< Output metadata file
    std::string outputFile;          //!< Output data file for the output module

    // Plugin dedicated service:
    datatools::multi_properties userLibConfig;  //!< Main configuration file for plugins loader

    // Metadata container:
    datatools::multi_properties inputMetadata;  //!< Metadata imported from the input

    //! Build a default arguments set:
    static FLpCDToCpCDParams makeDefault();

    // Print:
    void print(std::ostream &) const;
  };

} // namespace FLpCDToCpCD

#endif // FLPCDTOCPCDPARAMS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
