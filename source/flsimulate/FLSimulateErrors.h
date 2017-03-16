// FLSimulateErrors.h - FLSimulate errors
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

#ifndef FLSIMULATEERRORS_H
#define FLSIMULATEERRORS_H

// Standard library:
#include <string>
#include <stdexcept>

namespace FLSimulate {

  //----------------------------------------------------------------------
  // IMPLEMENTATION DETAILS
  //----------------------------------------------------------------------
  //! Exceptions:
  //! Exceptions for configuration
  class FLConfigDefaultError : public std::exception {};
  class FLConfigHelpHandled : public std::exception {};
  class FLConfigUserError : public std::invalid_argument {
  public:
    FLConfigUserError();
    explicit FLConfigUserError(const std::string& what_arg);
    explicit FLConfigUserError(const char*);

  };
  //! Exceptions for dialog
  class FLDialogHelpRequested : public std::exception {};
  class FLDialogOptionsError : public std::invalid_argument {
  public:
    FLDialogOptionsError();
    explicit FLDialogOptionsError(const std::string& what_arg);
    explicit FLDialogOptionsError(const char*);
  };

} // namespace FLSimulate

#endif // FLSIMULATEERRORS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
