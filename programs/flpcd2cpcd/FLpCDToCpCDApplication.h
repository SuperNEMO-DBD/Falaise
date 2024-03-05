// FLpCDToCpCDApplication.h - FLpCDToCpCD application
//
// Copyright (c) 2024 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2024 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLPCDTOCPCDAPPLICATION_H
#define FLPCDTOCPCDAPPLICATION_H

#include <string>

// This Project
#include "FLpCDToCpCDParams.h"

// Third Party
// - Bayeux
#include "bayeux/datatools/configuration/variant_service.h"

namespace FLpCDToCpCD {

  /// \brief Struct handling both the static parameters of the pcd2cpcd application and the variant service
  struct FLpCDToCpCDApplication
  {
		static std::string default_pcd2cpcd_tag();
		
    FLpCDToCpCDParams parameters; ///< Parameters
    datatools::configuration::variant_service variantService; ///< Variant service
  };

} // namespace FLpCDToCpCD

#endif // FLPCDTOCPCDAPPLICATION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
