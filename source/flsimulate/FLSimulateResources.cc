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
#include <stdexcept>

// Third Party
// - Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/assert.hpp>

// - Bayeux
#include "bayeux/datatools/exception.h"

// This Project
#include "FLSimulateBinReloc.h"
#include "FLSimulateConfig.h"

namespace FLSimulate {
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
  BrInitError err;
  bool initSuccessful = br_init(&err);
  DT_THROW_IF(!initSuccessful,
              std::runtime_error,
              "resource initialization failed : "
              << err
              << " ("
              << BRErrorAsString(err)
              << ")")
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

} // namespace FLSimulate

