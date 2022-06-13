// FLReconstructPipeline.h - Interface for FLReconstruct pipeline
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

#ifndef FLRECONSTRUCTPIPELINE_H
#define FLRECONSTRUCTPIPELINE_H

// Third party
//  - Bayeux:
#include "bayeux/datatools/service_manager.h"

// This Project
#include "FLReconstructParams.h"
#include "falaise/exitcodes.h"

namespace FLReconstruct {

//! Run the pipeline after configuration step
falaise::exit_code do_pipeline(const FLReconstructParams& flRecParameters);

//! Ensure some critical services are setup
falaise::exit_code ensure_core_services(const FLReconstructParams& recParams,
                                        datatools::service_manager& recServices);

}  // namespace FLReconstruct

#endif  // FLRECONSTRUCTPIPELINE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
