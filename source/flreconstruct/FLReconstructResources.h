// FLReconstructResources.h - Interface for accessing FLReconstruct resources
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.
#ifndef FLRECONSTRUCTRESOURCES_HH
#define FLRECONSTRUCTRESOURCES_HH
// Standard Library
#include <string>
#include <stdexcept>

// Third Party
// - A

// This Project

namespace FLReconstruct {
//! Exception for unknown resources
class UnknownResourceException : public std::runtime_error {
 public:
  UnknownResourceException(const std::string& msg) : std::runtime_error(msg) {}
};

//! Initialize the application resources, throws runtime_error on fail
void initResources();

//! Return the path to the application directory
std::string getApplicationDir();

//! Return the path to the root of the resource directory
std::string getResourceDir();

//! Return the control file for the given experiment and version id
std::string getControlFile(const std::string& experiment,
                           const std::string& /*versionID*/ = "");
}
#endif // FLRECONSTRUCTRESOURCES_HH

