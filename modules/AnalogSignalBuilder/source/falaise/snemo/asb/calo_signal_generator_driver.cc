// calo_signal_generator_driver.cc
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
#include <snemo/asb/calo_signal_generator_driver.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/mctools/signal/signal_shape_builder.h>

namespace snemo {

  namespace asb {

    struct calo_signal_generator_driver::pimpl_type
    {
      mctools::signal::signal_shape_builder ssb;
    };

    calo_signal_generator_driver::calo_signal_generator_driver(const std::string & id_)
      : base_signal_generator_driver(id_)
      , _mode_(MODE_INVALID)
    {
      _pimpl_.reset(new pimpl_type);
      return;
    }

    calo_signal_generator_driver::calo_signal_generator_driver(const mode_type mode_,
                                                               const std::string & id_)
      : base_signal_generator_driver(id_)
      , _mode_(mode_)
    {
      return;
    }

    calo_signal_generator_driver::~calo_signal_generator_driver()
    {
      if (is_initialized()) {
        this->calo_signal_generator_driver::reset();
      }
      _pimpl_.reset();
      return;
    }

    void calo_signal_generator_driver::set_mode(const mode_type m_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Driver is already initialized!");
      _mode_ = mode_;
      return;
    }

    mode_type calo_signal_generator_driver::get_mode() const
    {
      return _mode_;
    }

    void calo_signal_generator_driver::_initialize(const datatools::properties & config_)
    {
      if (_mode_ == MODE_INVALID) {
        if (config_.has_key("mode")) {
          std::string mode_label = config_.fetch_string("mode");
          if (mode_label == "triangle") {
            set_mode(MODE_TRIANGLE);
          } else {
            DT_THROW(std::logic_error,
                     "Unsupported driver mode '" << mode_label << "'!");
          }
        }
      }

      if (_mode_ == MODE_INVALID) {
        DT_THROW(std::logic_error, "Missing driver mode!");
      }

      return;
    }


    void calo_signal_generator_driver::_reset()
    {
      // clear resources...

      _mode_ = MODE_INVALID;
      return;
    }


    void calo_signal_generator_driver::_process(const mctools::simulated_data & sim_data_,
                                                mctools::signal::signal_data & sim_signal_data_)
    {
      if (_mode_ == MODE_TRIANGLE) {
        _process_triangle_mode_(sim_data_, sim_signal_data_);
      }
      return;
    }

    void  calo_signal_generator_driver::_process_triangle_mode_(const mctools::simulated_data & sim_data_,
                                                                mctools::signal::signal_data & sim_signal_data_)
    {
      // Do the job...
      return;
    }

  } // end of namespace asb

} // end of namespace snemo
