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
#ifndef FLSIMULATERESOURCES_HH
#define FLSIMULATERESOURCES_HH
// Standard Library
#include <string>

// Third Party
// - A

// This Project

namespace FLSimulate {
//! Initialize the application resources, throws runtime_error on fail
void initResources();

//! Return the path to the application directory
std::string getApplicationDir();

//! Return the path to the root of the resource directory
std::string getResourceDir();
}
#endif // FLSIMULATERESOURCES_HH

