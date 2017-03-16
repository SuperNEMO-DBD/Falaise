// FLSimulateResources.h - Interface for accessing FLSimulate resources
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLSIMULATERESOURCES_H
#define FLSIMULATERESOURCES_H

// Standard Library:
#include <string>
#include <stdexcept>

namespace FLSimulate {
  //! Exception for unknown resources
  class UnknownResourceException : public std::runtime_error {
  public:
    UnknownResourceException(const std::string& msg) : std::runtime_error(msg) {}
  };

  //! Return the control file for the given experiment and version id
  std::string getControlFile(const std::string& experiment,
                             const std::string& /*versionID*/ = "");

  //! Return the variants file for the given experiment and version id
  std::string getVariantsConfigFile(const std::string& experiment,
                                    const std::string& /*versionID*/ = "");

  // //! Return the default variants profile file for the given experiment and version id
  // std::string getVariantsDefaultProfile(const std::string& experiment,
  //                                       const std::string& /*versionID*/ = "");

  // //! Return the service configuration file for the given experiment and version id
  // std::string getServicesConfigFile(const std::string& experiment,
  //                                   const std::string& /*versionID*/ = "");

} // namespace FLSimulate

#endif // FLSIMULATERESOURCES_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
