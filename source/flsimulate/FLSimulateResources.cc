// FLSimulateResources.cc - Implementation of the FLSimulateResources
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
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/assert.hpp>
#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

// - Bayeux
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/library_info.h"

// This Project
#include "falaise/falaise.h"
#include "falaise/resource.h"
#include "falaise/version.h"

namespace FLSimulate {
//! Define defaults and type for lookup table of experiment control/configuration files
struct ExperimentLookup {
  typedef std::map<std::string, std::string> Table;
};

//! Construct lookup table
ExperimentLookup::Table constructLookupTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)(
      "", "snemo/demonstrator/geant4_control/Geant4Manager.conf")(
      "default", "snemo/demonstrator/geant4_control/Geant4Manager.conf")(
      "demonstrator", "snemo/demonstrator/geant4_control/Geant4Manager.conf")
      ;

  return a;
}

//! Construct lookup table
ExperimentLookup::Table constructLookupVariantsConfigTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)(
      "", "snemo/demonstrator/geant4_control/Geant4VariantRepository.conf")(
      "default", "snemo/demonstrator/geant4_control/Geant4VariantRepository.conf")(
      "demonstrator", "snemo/demonstrator/geant4_control/Geant4VariantRepository.conf")
      ;
  ;
  return a;
}


std::string version_token() { return std::string("/%v/"); }

std::string slashed_token(const std::string& token_) {
  std::ostringstream out;
  out << '/' << token_ << '/';
  return out.str();
}

std::string getControlFile(const std::string& experiment, const std::string& versionID) {
  static ExperimentLookup::Table a;
  if (a.empty()) a = constructLookupTable();

  std::string canonicalName(boost::to_lower_copy(experiment));
  ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
  if (p == a.end()) {
    throw UnknownResourceException("no control file for '" + experiment + "'");
  }

  boost::filesystem::path basePath(falaise::get_resource_dir());
  basePath /= (*p).second;
  std::string control_file = basePath.string();
  boost::replace_all(control_file, version_token(), slashed_token(versionID));
  return control_file;
}

std::string getVariantsConfigFile(const std::string& experiment, const std::string& versionID) {
  static ExperimentLookup::Table a;
  if (a.empty()) a = constructLookupVariantsConfigTable();

  std::string canonicalName(boost::to_lower_copy(experiment));
  ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
  if (p == a.end()) {
    throw UnknownResourceException("no variants file for '" + experiment + "'");
  }

  boost::filesystem::path basePath(falaise::get_resource_dir());
  basePath /= (*p).second;
  std::string variants_config_file = basePath.string();
  boost::replace_all(variants_config_file, version_token(), slashed_token(versionID));
  return variants_config_file;
}


}  // namespace FLSimulate
