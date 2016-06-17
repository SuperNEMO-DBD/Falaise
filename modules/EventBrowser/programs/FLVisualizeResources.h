// FLVisualizeResources.h - Interface for accessing FLVisualize resources
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLVISUALIZERESOURCES_H
#define FLVISUALIZERESOURCES_H

// Standard Library:
#include <string>
#include <stdexcept>

namespace FLVisualize {

  //! Exception for unknown resources
  class UnknownResourceException : public std::runtime_error {
  public:
    UnknownResourceException(const std::string& msg) : std::runtime_error(msg) {}
  };

  //! Return the detector default config file for the given experiment and version id
  std::string getDetectorDefaultConfigFile(const std::string& experiment,
                                           const std::string& /*versionID*/ = "");

  //! Return the variants file for the given experiment and version id
  std::string getVariantsConfigFile(const std::string& experiment,
                                    const std::string& /*versionID*/ = "");

  //! Return the default variants profile file for the given experiment and version id
  std::string getVariantsDefaultProfile(const std::string& experiment,
                                        const std::string& /*versionID*/ = "");

} // namespace FLVisualize

#endif // FLVISUALIZERESOURCES_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
