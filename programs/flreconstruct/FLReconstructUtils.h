// FLReconstructUtils.h - Utilities for FLReconstruct
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

#ifndef FLRECONSTRUCTUTILS_H
#define FLRECONSTRUCTUTILS_H

// Standard Library
#include <iostream>
#include <string>
#include <vector>

// Third Party
// - Boost
#include "boost/any.hpp"
// - Bayeux
#include "bayeux/datatools/logger.h"

namespace FLReconstruct {

//! Data type
enum data_type {
  DATA_UNKNOWN = 0,  //!< Unknown data type
  DATA_REAL = 1,     //!< Real data
  DATA_MC = 2        //!< Monte Carlo data
};

std::string get_data_type_label(data_type);

data_type get_data_type(const std::string&);

}  // end of namespace FLReconstruct

// - Validation of verbosity command line arguments. must exist inside
// the datatools namespace.
// TODO : refactor operator>> into datatools, though can't do this
// for validator (bpo dependency not wanted)
namespace datatools {

std::istream& operator>>(std::istream& in, datatools::logger::priority& p);

//! validate logging argument
void validate(boost::any& v, std::vector<std::string> const& values,
              datatools::logger::priority* /*target_type*/, int);
}  // namespace datatools

#endif  // FLRECONSTRUCTUTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
