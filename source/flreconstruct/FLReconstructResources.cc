// FLReconstructResource.cc - Implementation of the FLReconstructResource
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
#include "falaise/plugin.h"
#include "falaise/falaise.h"
#include "FLReconstructBinReloc.h"
#include "FLReconstructConfig.h"

namespace FLReconstruct {
//! Start resources
void doInitResources();

//! Fix internal/external resource paths
void doFixupResourcePaths();

//! Define defaults and type for lookup table of experiment control files
struct ExperimentLookup {
  typedef std::map<std::string, std::string> Table;
};

//! Construct lookup table
ExperimentLookup::Table constructLookupTable() {
  ExperimentLookup::Table a;
  boost::assign::insert(a)
      ("",
       "resources/config/snemo/demonstrator/simulation/geant4_control/1.0/manager.conf")
      ("default",
       "resources/config/snemo/demonstrator/simulation/geant4_control/1.0/manager.conf")
      ("demonstrator",
       "resources/config/snemo/demonstrator/simulation/geant4_control/1.0/manager.conf")
      ("tracker_commissioning",
       "resources/config/snemo/tracker_commissioning/simulation/geant4_control/1.0/manager.conf")
      ("bipo3",
       "resources/config/bipo3/simulation/geant4_control/1.0/manager.conf");

  return a;
}

//! Convert BrInitError code to a string describing the error
//! \todo add errno to returned string
std::string BRErrorAsString(const BrInitError& err) {
  std::string errMsg;
  switch (err) {
    case BR_INIT_ERROR_NOMEM:
      errMsg = "Unable to open /proc/self/maps";
    case BR_INIT_ERROR_OPEN_MAPS:
      errMsg =  "Unable to read from /proc/self/maps";
    case BR_INIT_ERROR_READ_MAPS:
      errMsg = "The file format of /proc/self/maps";
    case BR_INIT_ERROR_INVALID_MAPS:
      errMsg = "The file format of /proc/self/maps is invalid";
    case BR_INIT_ERROR_DISABLED:
      errMsg = "Binary relocation disabled";
    default:
      BOOST_ASSERT_MSG(1,"Invalid BrInitError");
  }
  return errMsg;
}

void initResources() {
  doInitResources();
  doFixupResourcePaths();
}

void doInitResources() {
  BrInitError err;
  bool initSuccessful = br_init(&err);
  DT_THROW_IF(!initSuccessful,
              std::runtime_error,
              "resource initialization failed : "
              << err
              << " ("
              << BRErrorAsString(err)
              << ")");
}

void doFixupResourcePaths() {
  // Temporary trick: fix the registered resource path in the datatools'
  // kernel.
  //  The  official configuration  files  use the  "@falaise:foo.conf"
  //  syntax. "@falaise:"  (or "@falaise.resources:") is  thus assumed
  //  to  refer  to the  installation  path  of resource  files.   The
  //  plugin/module  DLL  files  may  use  the  "@falaise.plugins:..."
  //  syntax.  "@falaise.plugins:" is  thus  assumed to  refer to  the
  //  installation path of the plugin DLL files.  Here we detect if we
  //  should run with the build directory.  In case Bayeux is embedded
  //  in Falaise and  not yet installed, we also  fix the "geomtools",
  //  "materials" and "genbb_help" resource paths.
  boost::filesystem::path dyn_res_path        = FLReconstruct::getResourceDir() + "/resources";
  boost::filesystem::path install_res_path    = "whothefuckcares";
  boost::filesystem::path dyn_plugin_path     = FLReconstruct::getPluginLibDir();
  boost::filesystem::path install_plugin_path = "idontgiveafuck";
  // bool fl_installed = true;
  if(!boost::filesystem::exists(install_res_path)) {
    // fl_installed = false;
    // Access to the datatools' kernel :
    datatools::kernel & krnl = datatools::kernel::instance();
    if (krnl.has_library_info_register()) {
      // Access to the datatools' kernel library info register:
      datatools::library_info& lib_info_reg = krnl.grab_library_info_register();
      if (lib_info_reg.has("falaise")) {
        // Kernel's library info already has en entry related to "falaise":
        datatools::properties & falaise_lib_infos = lib_info_reg.grab("falaise");

        // Fix resource path:
        falaise_lib_infos.update_string(
            datatools::library_info::keys::install_resource_dir(),
            dyn_res_path.string());
        falaise_lib_infos.update_string(
            datatools::library_info::keys::env_resource_dir(),
            "FALAISE_RESOURCE_DIR");

        // Fix plugin path:
        falaise_lib_infos.update_string(
            datatools::library_info::keys::install_plugin_lib_dir(),
            dyn_plugin_path.string());
        falaise_lib_infos.update_string(
            datatools::library_info::keys::env_plugin_lib_dir(),
            "FALAISE_PLUGIN_LIB_DIR");

      } else {
        // Kernel's library info does not have entry related to "falaise":
        datatools::properties & falaise_lib_infos =
            lib_info_reg.registration(
                "falaise",
                "Falaise provides the main computational environment for the simulation,"
                "processing and analysis of data for the SuperNEMO double beta decay "
                "search experiment.",
                falaise::version::get_version());

        // Fix resource path:
        falaise_lib_infos.store_string(
            datatools::library_info::keys::install_resource_dir(),
            dyn_res_path.string());
        falaise_lib_infos.store_string(
            datatools::library_info::keys::env_resource_dir(),
            "FALAISE_RESOURCE_DIR");

        // Fix plugin path:
        falaise_lib_infos.update_string(
            datatools::library_info::keys::install_plugin_lib_dir(),
            dyn_plugin_path.string());
        falaise_lib_infos.update_string(
            datatools::library_info::keys::env_plugin_lib_dir(),
            "FALAISE_PLUGIN_LIB_DIR");
      }
    }
  }
}

std::string getApplicationDir() {
  char* exePath(0);
  exePath = br_find_exe_dir("");
  boost::filesystem::path sExePath(exePath);
  free(exePath);
  boost::filesystem::path cExePath = boost::filesystem::canonical(sExePath);
  return cExePath.string();
}

std::string getResourceDir() {
  boost::filesystem::path tmpPath(getApplicationDir());
  tmpPath /= relativePathToResourceDir();
  boost::filesystem::path absPath = boost::filesystem::canonical(tmpPath);
  return absPath.string();
}

std::string getPluginLibDir() {
  boost::filesystem::path tmpPath(getApplicationDir());
  tmpPath /= relativePathToPluginLibDir();
  boost::filesystem::path absPath = boost::filesystem::canonical(tmpPath);
  return absPath.string();
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

  boost::filesystem::path basePath(getResourceDir());
  basePath /= (*p).second;
  return basePath.string();
}

} // namespace FLReconstruct
