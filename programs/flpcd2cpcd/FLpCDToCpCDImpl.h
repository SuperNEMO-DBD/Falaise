// FLpCDToCpCDImpl.h - Interface for FLpCDToCpCD functions
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

#ifndef FLPCDTOCPCDIMPL_H
#define FLPCDTOCPCDIMPL_H

// Standard Library:
#include <string>

// Third Party
// - Bayeux
#include "bayeux/datatools/multi_properties.h"

// This project
#include "FLpCDToCpCDApplication.h"
#include "FLpCDToCpCDParams.h"
#include "falaise/exitcodes.h"

namespace FLpCDToCpCD {

  //! Populate the metadata container with various informations classified in several categories
  falaise::exit_code do_metadata(const FLpCDToCpCDParams &, datatools::multi_properties &);

  ///! Configure variant parameters
  void do_configure_variant(FLpCDToCpCDApplication &recApplication_);

  ///! Parse command line arguments and provided script to configure the pcdtocpcdion parameters
  void do_configure(int argc_, char *argv_[], FLpCDToCpCDApplication &recApplication_);

  ///! Post process input metadata
  void do_postprocess_input_metadata(FLpCDToCpCDParams &flRecParameters_);

  ///! Post process pcdtocpcdion parameters
  void do_postprocess(FLpCDToCpCDParams &flRecParameters_);

  ///! Terminate the application variant service
  void do_terminate_variant(FLpCDToCpCDApplication &recApplication_);

  ///! Terminate the application properly
  void do_terminate(FLpCDToCpCDApplication &recApplication_);

} // namespace FLpCDToCpCD

#endif // FLPCDTOCPCDIMPL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
