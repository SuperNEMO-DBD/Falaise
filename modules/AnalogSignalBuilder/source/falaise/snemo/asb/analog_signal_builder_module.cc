// analog_signal_builder_module.cc - Implementation of Falaise ASB plugin version struct
//
// Copyright (c) 2016 F. Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 G. Oliviéro <goliviero@lpccaen.in2p3.fr>
//
// This file is part of Falaise/ASB plugin.
//
// Falaise/ASB plugin is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise/ASB plugin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise/ASB plugin.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <snemo/asb/analog_signal_builder_module.h>

namespace snemo {

  namespace asb {

    /// \brief Handler for an embedded signal generator driver
    struct analog_signal_builder_module::driver_entry
    {
      void initialize();
      void reset();
      base_signal_generator_driver & grab_driver();
      consy base_signal_generator_driver & get_driver() const;

      analog_signal_builder_module * parent = nullptr;
      std::string name;
      std::string type_id;
      datatools::properties config;
      std::shared_ptr<::snemo::asb::base_signal_generator_driver> handle;
    };

    void analog_signal_builder_module::driver_entry::initialize()
    {
    }

    void analog_signal_builder_module::driver_entry::reset()
    {
    }

    const base_signal_generator_driver &
    analog_signal_builder_module::driver_entry::get_driver() const
    {
      return handle.get();
    }

    base_signal_generator_driver &
    analog_signal_builder_module::driver_entry::grab_driver()
    {
      return handle.grab();
    }

  } // end of namespace asb

} // end of namespace snemo
