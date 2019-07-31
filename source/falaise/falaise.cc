// falaise/falaise.cc - Implementation of falaise init function
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2013-2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013-2017 by Université de Caen Normandie
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "falaise/falaise.h"

// Standard library
#include <iostream>

// Third party
// - Bayeux
#include <bayeux/bayeux.h>
// Tests
// - datatools
#include <bayeux/datatools/exception.h>

// Mute some specific "shadow" warnings in Boost <=1.55:
//#if defined(__clang__)
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wshadow"
//#endif
//#if defined(__GNUC__)
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wshadow"
//#endif

#include <bayeux/datatools/kernel.h>

//#if defined(__GNUC__)
//#pragma GCC diagnostic pop
//#endif
//#if defined(__clang__)
//#pragma clang diagnostic pop
//#endif

// This project;
#include "falaise/detail/falaise_sys.h"
#include "falaise/resource.h"

namespace {

static bool _flinit = false;

void falaise_initialize_impl() {
  falaise::init_resources();

  DT_THROW_IF(!datatools::kernel::is_instantiated(), std::runtime_error,
              "The Bayeux/datatools' kernel is not instantiated !");

  // Populate the library info register, basically dumb if we don't
  // have it so assume it exists and hope for an exception if
  // it doesn't
  if (!falaise::detail::falaise_sys::is_instantiated()) {
    falaise::detail::falaise_sys& flSys = ::falaise::detail::falaise_sys::instantiate();
    flSys.initialize();
  }
}

void falaise_terminate_impl() {
  if (falaise::detail::falaise_sys::is_instantiated()) {
    falaise::detail::falaise_sys& flSys = falaise::detail::falaise_sys::instance();
    flSys.shutdown();
  }
}

}  // namespace

namespace falaise {

void initialize(int argc_, char* argv_[], uint32_t flags_) {
  if (!::_flinit) {
    // Wrap Bayeux initialization because we need resources from various Bayeux's modules:
    bayeux::initialize(argc_, argv_, flags_);
    // Initialization code from Falaise itself:
    falaise_initialize_impl();
    ::_flinit = true;
  } else {
    DT_THROW(std::logic_error, "Attempt to initialize the already initialized Falaise library!");
  }
}

void terminate() {
  if (::_flinit) {
    // Termination code for Falaise:
    falaise_terminate_impl();

    // Wrap Bayeux termination:
    bayeux::terminate();
    ::_flinit = false;
  } else {
    DT_THROW(std::logic_error, "Attempt to terminate the already terminated Falaise library!");
  }
}

}  // namespace falaise
