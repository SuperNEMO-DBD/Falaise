// FLSimulateResource.cc - Implementation of the FLSimulateResource
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

// - Ourselves
#include "FLSimulateResources.h"

// Standard Library
#include <map>

// Third Party
// - Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/assert.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

// - Bayeux
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/library_info.h"

// This Project
#include "falaise/version.h"
#include "falaise/resource.h"
#include "falaise/falaise.h"

namespace FLSimulate {
//! Define defaults and type for lookup table of experiment control files
struct ExperimentLookup {
  typedef std::map<std::string, std::string> Table;
};

//! Construct lookup table
ExperimentLookup::Table constructLookupTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)
      ("",
       "config/snemo/demonstrator/simulation/geant4_control/1.0/manager.conf")
      ("default",
       "config/snemo/demonstrator/simulation/geant4_control/1.0/manager.conf")
      ("demonstrator",
       "config/snemo/demonstrator/simulation/geant4_control/1.0/manager.conf")
      ("tracker_commissioning",
       "config/snemo/tracker_commissioning/simulation/geant4_control/1.0/manager.conf")
      ("bipo3",
       "config/bipo3/simulation/geant4_control/1.0/manager.conf");

  return a;
}

std::string getControlFile(const std::string& experiment,
                           const std::string& /*versionID*/) {
  static ExperimentLookup::Table a;
  if (a.empty()) a = constructLookupTable();

  std::string canonicalName(boost::to_lower_copy(experiment));
  ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
  if (p == a.end()) {
    throw UnknownResourceException("no control file for '"+experiment+"'");
  }

  boost::filesystem::path basePath(falaise::get_resource_dir());
  basePath /= (*p).second;
  return basePath.string();
}

} // namespace FLSimulate
