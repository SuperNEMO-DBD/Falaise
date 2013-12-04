//! \file    falaise/resource.h
//! \brief   Utilities for accessing falaise resource files
//! \details The falaise library makes use of several resource files
//!          containing isotopic and radioactive decays data.
//!
//!          As these files are not compiled into the falaise library,
//!          falaise provides a simple API to get a path to known
//!          resource files.
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
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
#include <string>

// Third Party
// - A

// This Project

namespace falaise {

//! Return URL, i.e. a path, to the  base directory where resource files are installed
//! \param overriden_env If set this flag trigger the search for the
//!        environment variable FALAISE_RESOURCE_DIR as a directory path on the filesystem
//!        as an alternative base directory for resources in place of the
//!        standard installation path
std::string get_resource_dir(bool overriden_env = false);

//! Return URL, i.e. a path, to named resource
//! By default the encoded resource root, determined at compile time
//! is used to search for the resource. This can be overriden by setting
//! the FALAISE_RESOURCE_DIR environment variable to a path holding custom
//! resources.
//! \param rname name of resource, given as a path relative to
//!        resource root.
//! \param overriden_env flag to allow path overriding by the FALAISE_RESOURCE_DIR environment variable.
std::string get_resource(const std::string& rname, bool overriden_env = false);
} // namespace falaise

#endif // FALAISE_RESOURCE_H
