// FLReconstructErrors.h - Interface for FLReconstruct error handling
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

#ifndef FLRECONSTRUCTERRORS_H
#define FLRECONSTRUCTERRORS_H

// Standard Library:
#include <string>
#include <iostream>
#include <stdexcept>

namespace FLReconstruct {

  enum FLDialogState {
    DIALOG_OK,
    DIALOG_QUERY,
    DIALOG_ERROR
  };

  //! Exceptions:
  //! Exceptions for configuration
  class FLConfigDefaultError : public std::exception {};
  class FLConfigHelpHandled : public std::exception {};
  class FLConfigUserError : public std::runtime_error {using std::runtime_error::runtime_error;};
  //! Exceptions for dialog
  class FLDialogHelpRequested : public std::exception {};
  class FLDialogOptionsError : public std::exception {};

  void do_error(std::ostream& os, const char* err);

  void do_error(std::ostream& os, const std::string & serr);

} // namespace FLReconstruct

#endif // FLRECONSTRUCTERRORS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
