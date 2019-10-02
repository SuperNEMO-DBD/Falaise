// FLSimulateUtils.h - FLSimulate utilities
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLSIMULATEUTILS_H
#define FLSIMULATEUTILS_H

// Standard Library
#include <map>
#include <string>

namespace FLSimulate {

// Return the default simulation setup for the current Falaise release
std::string default_simulation_setup();

// Return the list of registered simulation setups for the current Falaise release
std::map<std::string, std::string> list_of_simulation_setups();

}  // namespace FLSimulate

#endif  // FLSIMULATEUTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
