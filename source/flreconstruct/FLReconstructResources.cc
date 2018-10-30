// FLReconstructResources.cc - Implementation of the FLReconstructResources
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
#include "FLReconstructResources.h"

// Standard Library
#include <map>

// Third Party
// - Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/algorithm/string.hpp>
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

namespace FLReconstruct {

//! Define defaults and type for lookup table of experiment control/configuration files
struct ExperimentLookup {
  typedef std::map<std::string, std::string> Table;

  /// Return resource path in table for given experiment
  /// Returns empty string if not found
  static std::string findPathInTable(const Table& in, const std::string& exe) {
    std::string canonicalName(boost::to_lower_copy(exe));
    auto iter = in.find(canonicalName);
    if (iter == in.end()) {
      return std::string{};
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*iter).second;
    return basePath.string();
  }
};

//! Construct lookup table
ExperimentLookup::Table constructLookupPipelineTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)("", "snemo/demonstrator/reconstruction/official-1.0.0.conf")(
      "default", "snemo/demonstrator/reconstruction/official-1.0.0.conf")(
      "demonstrator", "snemo/demonstrator/reconstruction/official-1.0.0.conf")("bipo3", "");

  return a;
}

//! Construct lookup table
ExperimentLookup::Table constructLookupVariantsConfigTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)(
      "", "snemo/demonstrator/geant4_control/Geant4VariantRepository.conf")(
      "default",
      "snemo/demonstrator/geant4_control/Geant4VariantRepository.conf")(
      "demonstrator",
      "snemo/demonstrator/geant4_control/Geant4VariantRepository.conf")("bipo3",
                                                                                          "");
  ;
  return a;
}

//! Construct lookup table
ExperimentLookup::Table constructLookupVariantsDefaultProfileTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)(
      "",
      "snemo/demonstrator/profiles/demonstrator-simulation.profile")(
      "default",
      "snemo/demonstrator/profiles/demonstrator-simulation.profile")(
      "demonstrator",
      "snemo/demonstrator/profiles/demonstrator-simulation.profile")(
      "bipo3", "");
  ;
  return a;
}

std::string getPipelineDefaultControlFile(const std::string& experiment,
                                          const std::string& /*versionID*/) {
  static ExperimentLookup::Table a;
  if (a.empty()) a = constructLookupPipelineTable();

  std::string path = ExperimentLookup::findPathInTable(a, experiment);

  if (path.empty()) {
    throw UnknownResourceException("no control file for '" + experiment + "'");
  }

  return path;
}

std::string getVariantsConfigFile(const std::string& experiment, const std::string& /*versionID*/) {
  static ExperimentLookup::Table a;
  if (a.empty()) a = constructLookupVariantsConfigTable();

  std::string path = ExperimentLookup::findPathInTable(a, experiment);

  if (path.empty()) {
    throw UnknownResourceException("no variants file for '" + experiment + "'");
  }

  return path;
}

std::string getVariantsDefaultProfile(const std::string& experiment,
                                      const std::string& /*versionID*/) {
  static ExperimentLookup::Table a;
  if (a.empty()) a = constructLookupVariantsDefaultProfileTable();

  std::string path = ExperimentLookup::findPathInTable(a, experiment);

  if (path.empty()) {
    throw UnknownResourceException("no default variants profile for '" + experiment + "'");
  }

  return path;
}

}  // namespace FLReconstruct
