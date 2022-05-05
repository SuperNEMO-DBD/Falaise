// FLReconstructApplication.h - FLReconstruct application
//
// Copyright (c) 2021 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2021 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLRECONSTRUCTAPPLICATION_H
#define FLRECONSTRUCTAPPLICATION_H

// This Project
#include "FLReconstructParams.h"

// Third Party
// - Bayeux
#include "bayeux/datatools/configuration/variant_service.h"

namespace FLReconstruct {

struct FLReconstructApplication
{
  FLReconstructParams parameters;
  datatools::configuration::variant_service variantService;
};

} // namespace FLReconstruct

#endif // FLRECONSTRUCTAPPLICATION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
