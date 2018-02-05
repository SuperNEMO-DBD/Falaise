// FLTagsErrors.h - FLTags errors
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLTAGSERRORS_H
#define FLTAGSERRORS_H

// Standard library:
#include <stdexcept>
#include <string>

namespace FLTags {

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

}  // namespace FLTags

#endif  // FLTAGSERRORS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
