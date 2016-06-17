// FLVisualizeResources.cc - Implementation of the FLVisualizeResources
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
#include "FLVisualizeResources.h"

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

namespace FLVisualize {

  //! Define defaults and type for lookup table of experiment control/configuration files
  struct ExperimentLookup {
    typedef std::map<std::string, std::string> Table;
  };

  // //! Construct lookup table
  // ExperimentLookup::Table constructLookupDetectorConfigTable() {
  //   ExperimentLookup::Table a;
  //   boost::assign::insert(a)
  //     ("",
  //      "config/snemo/demonstrator/geometry/4.0/manager.conf")
  //     ("default",
  //      "config/snemo/demonstrator/geometry/4.0/manager.conf")
  //     ("demonstrator",
  //      "config/snemo/demonstrator/geometry/4.0/manager.conf")
  //     ("bipo3",
  //      "");
  //   return a;
  // }

  //! Construct lookup table
  ExperimentLookup::Table constructLookupVariantsConfigTable() {
    ExperimentLookup::Table a;
    boost::assign::insert(a)
      ("",
       "config/snemo/demonstrator/geometry/4.0/variants/repository.conf")
      ("default",
       "config/snemo/demonstrator/geometry/4.0/variants/repository.conf")
      ("demonstrator",
       "config/snemo/demonstrator/geometry/4.0/variants/repository.conf")
      ("bipo3",
       "");
    ;
    return a;
  }

  //! Construct lookup table
  ExperimentLookup::Table constructLookupVariantsDefaultProfileTable() {
    ExperimentLookup::Table a;
    boost::assign::insert(a)
      ("",
       "config/snemo/demonstrator/geometry/4.0/variants/profiles/default.profile")
      ("default",
       "config/snemo/demonstrator/geometry/4.0/variants/profiles/default.profile")
      ("demonstrator",
       "config/snemo/demonstrator/geometry/4.0/variants/profiles/default.profile")
      ("bipo3",
       "");
    ;
    return a;
  }

  // std::string getDetectorDefaultConfigFile(const std::string& experiment,
  //                                          const std::string& /*versionID*/) {
  //   static ExperimentLookup::Table a;
  //   if (a.empty()) a = constructLookupDetectorConfigTable();

  //   std::string canonicalName(boost::to_lower_copy(experiment));
  //   ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
  //   if (p == a.end()) {
  //     throw UnknownResourceException("no control file for '"+experiment+"'");
  //   }

  //   boost::filesystem::path basePath(falaise::get_resource_dir());
  //   basePath /= (*p).second;
  //   return basePath.string();
  // }

  std::string getVariantsConfigFile(const std::string& experiment,
                                     const std::string& /*versionID*/) {
    static ExperimentLookup::Table a;
    if (a.empty()) a = constructLookupVariantsConfigTable();

    std::string canonicalName(boost::to_lower_copy(experiment));
    ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
    if (p == a.end()) {
      throw UnknownResourceException("no variants file for '"+experiment+"'");
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*p).second;
    return basePath.string();
  }

  std::string getVariantsDefaultProfile(const std::string& experiment,
                                        const std::string& /*versionID*/) {
    static ExperimentLookup::Table a;
    if (a.empty()) a = constructLookupVariantsDefaultProfileTable();

    std::string canonicalName(boost::to_lower_copy(experiment));
    ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
    if (p == a.end()) {
      throw UnknownResourceException("no default variants profile for '"+experiment+"'");
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*p).second;
    return basePath.string();
  }

} // namespace FLVisualize
