//! \file    falaise/resource.h
//! \brief   Utilities for accessing falaise resource/plugin files
//! \details The falaise library makes use of several resource files
//!          containing isotopic and radioactive decays data.
//!          Falaise also implements a plugin architecture, with
//!          several supplied as core functionality.
//!
//!          As these files are not compiled into the falaise library,
//!          a simple API is provided to return the root paths to core
//!          resource and plugin directories.
//!
//!          These paths are calculated based on the location of the
//!          Falaise library, allowing relocation of the Falaise
//!          package after installation.
//!
//!          The resource path may be overridden by an environment variable for testing
//!          purposes, and the use of the environment override may be
//!          queried.
//!
//!          If your application uses resources from Falaise,
//!          you must call the falaise::init_resources function
//!          before trying to access resources.
//
// Copyright (c) 2013-2016 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013-2016 by The University of Warwick
//
// This file is part of falaise.
//
// falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with falaise.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FALAISE_RESOURCE_H
#define FALAISE_RESOURCE_H
// Standard Library
#include <stdexcept>
#include <string>

// Third Party
// - A

// This Project

namespace falaise {
//! Exception class for bad resource initialization
class ResourceInitializationException : public std::runtime_error {
 public:
  ResourceInitializationException(const std::string& msg) : std::runtime_error(msg) {}
};

//! Exception class for unknown resources
class UnknownResourceException : public std::runtime_error {
 public:
  UnknownResourceException(const std::string& msg) : std::runtime_error(msg) {}
};

//! Name of environment variable that can be set to override root resource path
constexpr char kResourceOverrideVarName[] = "FALAISE_RESOURCE_DIR";

//! Initialize the library resource paths
//! \throw ResourceInitializationException when library cannot self locate
void init_resources();

//! Return URL, i.e. a path, to the  base directory where resource files are installed
//! It may be overiden at runtime by the FALAISE_RESOURCE_DIR environment variable
std::string get_resource_dir();

//! Return URL, i.e. a path, to named resource
std::string get_resource(const std::string& rname);

//! Return true if the resource directory is overidden from the environment
bool resources_overidden();

//! Return URL, i.e. a path, to the  base directory where plugin DLL files are installed
std::string get_plugin_dir();

//! Return URL, i.e. a path, to named resource
std::string get_plugin(const std::string& rname);

}  // namespace falaise

#endif  // FALAISE_RESOURCE_H
