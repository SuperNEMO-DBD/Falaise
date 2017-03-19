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
#include "falaise/exitcodes.h"
#include "FLReconstructParams.h"

namespace FLReconstruct {

  //! \brief Input metadata collector
  class metadata_collector
  {
  public:
    enum data_flags {
      DATA_MC = 0x1 //!< Simulated data type
    };

    //! Constructor
    metadata_collector(const uint32_t flags_ = 0);

    //! Extract metadata from input data file (embedded metadata)
    datatools::multi_properties do_get_metadata_from_data(const std::string & file) const;

    //! Extract metadata from input metadata file
    datatools::multi_properties do_get_metadata_from_file(const std::string & file) const;

  private:

    //! Initialization
    void _init_();

  private:

    uint32_t _dataflags_ = 0; //!< Data type

  };

  //! Populate the metadata container with various informations classified in several categories
  falaise::exit_code do_metadata(const FLReconstructParams &, datatools::multi_properties &);

  ///! Post process reconstruction parameters
  void do_postprocess(FLReconstructParams & flRecParameters);

  ///! Parse command line arguments and provided script to configure the reconstruction parameters
  void do_configure(int argc, char *argv[], FLReconstructParams& recParameters);

} // namespace FLReconstruct

#endif // FLRECONSTRUCTIMPL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
