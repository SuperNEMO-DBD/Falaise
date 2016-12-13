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
      :	base_signal_generator_driver(id_)
    {
      _mode_ = MODE_INVALID;
      _pimpl_.reset(new pimpl_type);
      return;
    }

    calo_signal_generator_driver::calo_signal_generator_driver(const mode_type mode_,
                                                               const std::string & id_)
      : base_signal_generator_driver(id_),
	_mode_(mode_)
    {
      _pimpl_.reset(new pimpl_type);
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

    void calo_signal_generator_driver::set_mode(const mode_type mode_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Driver is already initialized!");
      _mode_ = mode_;
      return;
    }

    calo_signal_generator_driver::mode_type calo_signal_generator_driver::get_mode() const
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
      
      _pimpl_->ssb.set_logging(datatools::logger::PRIO_DEBUG);
      _pimpl_->ssb.set_category("calo");
      _pimpl_->ssb.add_registered_shape_type_id("mctools::signal::triangle_signal_shape");
      _pimpl_->ssb.add_registered_shape_type_id("mctools::signal::multi_signal_shape");
      _pimpl_->ssb.initialize_simple();
      return;
    }


    void calo_signal_generator_driver::_reset()
    {
      // clear resources...

      _mode_ = MODE_INVALID;
      return;
    }


    double calo_signal_generator_driver::_convert_energy_to_amplitude(const double energy_)
    {
      // 1 MeV is equivalent to 300 mV
      double amplitude_1MeV = 0.3 * CLHEP::volt; // value : 0.3 * 10^-7 
      double amplitude = (energy_ / 1 * CLHEP::MeV) * amplitude_1MeV;
      return amplitude; // maybe units problem for the moment
    }


    void calo_signal_generator_driver::_process(const mctools::simulated_data & sim_data_,
                                                mctools::signal::signal_data & sim_signal_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo signal generator driver is not initialized !");
      DT_THROW_IF(_mode_ == MODE_INVALID, std::logic_error, "Calo signal generator driver mode is invalid !");
      
      if (_mode_ == MODE_TRIANGLE) {
        _process_triangle_mode_(sim_data_, sim_signal_data_);
      }
      return;
    }

    void  calo_signal_generator_driver::_process_triangle_mode_(const mctools::simulated_data & sim_data_,
                                                                mctools::signal::signal_data & sim_signal_data_)
    {
      DT_THROW_IF(!sim_data_.has_step_hits("calo"), std::logic_error, "Simulated Datas have no step hits 'calo'");
      
      // For the moment, each calo hit is represented by a triangle calo signal. 
      // The next step is to take into account multi hit into one GID. Several
      // 'small' signals must construct a 'multi signal' for one GID (sum)

      if (sim_data_.has_step_hits("calo"))
	{
	  const size_t number_of_calo_hits = sim_data_.get_number_of_step_hits("calo");
	  for (size_t ihit = 0; ihit < number_of_calo_hits; ihit++)
	    {
	      const mctools::base_step_hit & main_calo_hit = sim_data_.get_step_hit("calo", ihit);
	      unsigned int calo_hit_id    = main_calo_hit.get_hit_id();
	      const double signal_time    = main_calo_hit.get_time_stop() * CLHEP::ns;
	      const double energy_deposit = main_calo_hit.get_energy_deposit() * CLHEP::MeV;
	      const geomtools::geom_id & calo_gid = main_calo_hit.get_geom_id();
	      
	      
	      mctools::signal::base_signal & signal = sim_signal_data_.add_signal("calo");
	      signal.set_hit_id(calo_hit_id);
	      signal.set_geom_id(calo_gid);
	      // signal.set_shape_builder(_pimpl_->ssb);
	      double t0 = signal_time;
	      double t1 = t0 + 14 * CLHEP::ns; // Rise time on calo signal (from Bordeaux wavecatcher signals)
	      double t2 = t1 + 72 * CLHEP::ns; // Fall time on calo signal (from Bordeaux wavecatcher signals)
	      const double amplitude = _convert_energy_to_amplitude(energy_deposit);
	      signal.set_shape_type_id("mctools::signal::triangle_signal_shape");
	      signal.set_shape_string_parameter("polarity", "-");
	      signal.set_shape_real_parameter_with_explicit_unit("t0", t0, "ns");
	      signal.set_shape_real_parameter_with_explicit_unit("t1", t1, "ns");
	      signal.set_shape_real_parameter_with_explicit_unit("t2", t2, "ns");
	      signal.set_shape_real_parameter_with_explicit_unit("amplitude", amplitude, "V");
	      signal.initialize_simple();
	      //signal.build_signal_shape("calo", signal);
	      

	      signal.tree_dump(std::clog, "Calo Hit signal : ");


	      std::clog << "Time stop : " << signal_time << std::endl;
	      std::clog << "Energy    : " << energy_deposit << std::endl;
	      std::clog << "Amplitude : " << amplitude << std::endl;
	      std::clog << "GID       : " << calo_gid << std::endl;
	      
	      // _pimpl_->ssb.create_signal_shape(
	    }
	}
		     
      return;
    }

  } // end of namespace asb

} // end of namespace snemo
