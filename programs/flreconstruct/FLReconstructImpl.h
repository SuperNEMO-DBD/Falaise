// FLReconstructImpl.h - Interface for FLReconstruct functions
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

#ifndef FLRECONSTRUCTIMPL_H
#define FLRECONSTRUCTIMPL_H

// Standard Library:
#include <string>

// Third Party
// - Bayeux
#include "bayeux/datatools/multi_properties.h"

// This project
#include "FLReconstructApplication.h"
#include "FLReconstructParams.h"
#include "falaise/exitcodes.h"

namespace FLReconstruct {

//! Populate the metadata container with various informations classified in several categories
falaise::exit_code do_metadata(const FLReconstructParams &, datatools::multi_properties &);

///! Configure variant parameters
void do_configure_variant(FLReconstructApplication &recApplication);

///! Parse command line arguments and provided script to configure the reconstruction parameters
void do_configure(int argc, char *argv[], FLReconstructApplication &recApplication);

///! Post process input metadata
void do_postprocess_input_metadata(FLReconstructParams &flRecParameters);

///! Post process reconstruction parameters
void do_postprocess(FLReconstructParams &flRecParameters);

///! Terminate the application variant service
void do_terminate_variant(FLReconstructApplication &recApplication);

///! Terminate the application properly
void do_terminate(FLReconstructApplication &recApplication);

}  // namespace FLReconstruct

#endif  // FLRECONSTRUCTIMPL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
