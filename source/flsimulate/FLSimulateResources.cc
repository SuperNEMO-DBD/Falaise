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
#include <boost/filesystem.hpp>
#include <boost/assert.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

// - Bayeux
#include "bayeux/datatools/exception.h"
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/library_info.h"

// This Project
#include "falaise/version.h"
#include "falaise/resource.h"
#include "falaise/falaise.h"

namespace FLSimulate {
  //! Define defaults and type for lookup table of experiment control/configuration files
  struct ExperimentLookup {
    typedef std::map<std::string, std::string> Table;
  };

  //! Construct lookup table
  ExperimentLookup::Table constructLookupTable() {
    ExperimentLookup::Table a;
    boost::assign::insert(a)
      ("",
       "config/snemo/demonstrator/simulation/geant4_control/%v/manager.conf")
      ("default",
       "config/snemo/demonstrator/simulation/geant4_control/%v/manager.conf")
      ("demonstrator",
       "config/snemo/demonstrator/simulation/geant4_control/%v/manager.conf")
      ("bipo3",
       "config/bipo3/simulation/geant4_control/1.0/manager.conf");

    return a;
  }

  //! Construct lookup table
  ExperimentLookup::Table constructLookupVariantsConfigTable() {
    ExperimentLookup::Table a;
    boost::assign::insert(a)
      ("",
       "config/snemo/demonstrator/simulation/geant4_control/%v/variants/repository.conf")
      ("default",
       "config/snemo/demonstrator/simulation/geant4_control/%v/variants/repository.conf")
      ("demonstrator",
       "config/snemo/demonstrator/simulation/geant4_control/%v/variants/repository.conf")
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
       "config/snemo/demonstrator/simulation/geant4_control/%v/variants/profiles/default.profile")
      ("default",
       "config/snemo/demonstrator/simulation/geant4_control/%v/variants/profiles/default.profile")
      ("demonstrator",
       "config/snemo/demonstrator/simulation/geant4_control/%v/variants/profiles/default.profile")
      ("bipo3",
       "");
    ;
    return a;
  }

  //! Construct lookup table
  ExperimentLookup::Table constructLookupServicesConfigTable() {
    ExperimentLookup::Table a;
    boost::assign::insert(a)
      ("",
       "config/snemo/demonstrator/simulation/geant4_control/%v/services.conf")
      ("default",
       "config/snemo/demonstrator/simulation/geant4_control/%v/services.conf")
      ("demonstrator",
       "config/snemo/demonstrator/simulation/geant4_control/%v/services.conf")
      ("bipo3",
       "");
    ;
    return a;
  }

  std::string version_token()
  {
    return std::string("/%v/");
  }

  std::string slashed_token(const std::string & token_)
  {
    std::ostringstream out;
    out << '/' << token_ << '/';
    return out.str();
  }

  std::string getControlFile(const std::string& experiment,
                             const std::string& versionID) {
    static ExperimentLookup::Table a;
    if (a.empty()) a = constructLookupTable();

    std::string canonicalName(boost::to_lower_copy(experiment));
    ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
    if (p == a.end()) {
      throw UnknownResourceException("no control file for '"+experiment+"'");
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*p).second;
    std::string control_file = basePath.string();
    boost::replace_all(control_file, version_token(), slashed_token(versionID));
    return control_file;
  }

  std::string getVariantsConfigFile(const std::string& experiment,
                                     const std::string& versionID) {
    static ExperimentLookup::Table a;
    if (a.empty()) a = constructLookupVariantsConfigTable();

    std::string canonicalName(boost::to_lower_copy(experiment));
    ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
    if (p == a.end()) {
      throw UnknownResourceException("no variants file for '"+experiment+"'");
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*p).second;
    std::string variants_config_file = basePath.string();
    boost::replace_all(variants_config_file, version_token(), slashed_token(versionID));
    return variants_config_file;
  }

  std::string getVariantsDefaultProfile(const std::string& experiment,
                                        const std::string& versionID) {
    static ExperimentLookup::Table a;
    if (a.empty()) a = constructLookupVariantsDefaultProfileTable();

    std::string canonicalName(boost::to_lower_copy(experiment));
    ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
    if (p == a.end()) {
      throw UnknownResourceException("no default variants profile for '"+experiment+"'");
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*p).second;
    std::string  variants_default_profile = basePath.string();
    boost::replace_all(variants_default_profile, version_token(), slashed_token(versionID));
    return variants_default_profile;
  }

  std::string getServicesConfigFile(const std::string& experiment,
                                    const std::string& versionID)
  {
    static ExperimentLookup::Table a;
    if (a.empty()) a = constructLookupServicesConfigTable();

    std::string canonicalName(boost::to_lower_copy(experiment));
    ExperimentLookup::Table::const_iterator p = a.find(canonicalName);
    if (p == a.end()) {
      throw UnknownResourceException("no default services manager configuration for '"+experiment+"'");
    }

    boost::filesystem::path basePath(falaise::get_resource_dir());
    basePath /= (*p).second;
    std::string  services_config_file = basePath.string();
    boost::replace_all(services_config_file, version_token(), slashed_token(versionID));
    return services_config_file;
  }

} // namespace FLSimulate
