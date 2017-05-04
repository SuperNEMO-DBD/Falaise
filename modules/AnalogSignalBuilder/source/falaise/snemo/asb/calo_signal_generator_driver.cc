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


namespace snemo {

  namespace asb {
    
    calo_signal_generator_driver::calo_signal_generator_driver(const std::string & id_)
      :	base_signal_generator_driver(id_)
    {
      _mode_ = MODE_INVALID;
      return;
    }

    calo_signal_generator_driver::calo_signal_generator_driver(const mode_type mode_,
                                                               const std::string & id_)
      : base_signal_generator_driver(id_),
	_mode_(mode_)
    {
      return;
    }

    calo_signal_generator_driver::~calo_signal_generator_driver()
    {
      if (is_initialized()) {
        this->calo_signal_generator_driver::reset();
      }
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

    void calo_signal_generator_driver::_process_triangle_mode_(const mctools::simulated_data & sim_data_,
                                                                mctools::signal::signal_data & sim_signal_data_)
    {
      DT_THROW_IF(!sim_data_.has_step_hits("calo"), std::logic_error, "Simulated Datas have no step hits 'calo'");
      
      // For the moment, each calo hit is represented by a triangle calo signal. 
      // The next step is to take into account multi hit into one GID. Several
      // 'small' signals must construct a 'multi signal' for one GID (sum)

      if (sim_data_.has_step_hits("calo"))
	{
	  const size_t number_of_calo_hits = sim_data_.get_number_of_step_hits("calo");
	  
	  double event_time_ref;
	  datatools::invalidate(event_time_ref);
	  std::set<geomtools::geom_id> set_of_gids;
	  std::map<geomtools::geom_id, unsigned int> map_gid_hit_in_gid;
	  std::vector<mctools::signal::base_signal> atomic_signal_collection;

	  // Search calo time reference for the event :
	  for (size_t ihit = 0; ihit < number_of_calo_hits; ihit++)
	    {
	      const double signal_time    = sim_data_.get_step_hit("calo", ihit).get_time_start() * CLHEP::ns;
	      if (!datatools::is_valid(event_time_ref)) event_time_ref = signal_time;
	      if (signal_time < event_time_ref) event_time_ref = signal_time;
	    }
	  
	  for (size_t ihit = 0; ihit < number_of_calo_hits; ihit++)
	    {
	      const mctools::base_step_hit & main_calo_hit = sim_data_.get_step_hit("calo", ihit);
	      unsigned int calo_hit_id    = main_calo_hit.get_hit_id();
	      const double signal_time    = main_calo_hit.get_time_start() * CLHEP::ns;
	      const double energy_deposit = main_calo_hit.get_energy_deposit() * CLHEP::MeV;
	      const geomtools::geom_id & calo_gid = main_calo_hit.get_geom_id();
	      
	      auto it = map_gid_hit_in_gid.find(calo_gid);
	      bool already_exist = false;
	      if (it != map_gid_hit_in_gid.end()) already_exist = true;
	      if (already_exist) map_gid_hit_in_gid.find(calo_gid)->second+=1;
	      else map_gid_hit_in_gid.insert(std::pair<geomtools::geom_id, int> (calo_gid, 1));

	      set_of_gids.insert(calo_gid);

	      mctools::signal::base_signal a_signal;

	      a_signal.set_hit_id(calo_hit_id);
	      a_signal.set_geom_id(calo_gid);

	      const double t0 = signal_time - event_time_ref;
	      const double t1 = t0 + 8 * CLHEP::ns; // Rise time on calo signal (from Bordeaux wavecatcher signals)
	      const double t2 = t1 + 70 * CLHEP::ns; // Fall time on calo signal (from Bordeaux wavecatcher signals)
	      const double amplitude = _convert_energy_to_amplitude(energy_deposit);

	      a_signal.set_category("calo");
	      a_signal.set_time_ref(event_time_ref);
	      a_signal.set_shape_type_id("mctools::signal::triangle_signal_shape");
	      a_signal.set_shape_string_parameter("polarity", "-");
	      a_signal.set_shape_real_parameter_with_explicit_unit("t0", t0, "ns");
	      a_signal.set_shape_real_parameter_with_explicit_unit("t1", t1, "ns");
	      a_signal.set_shape_real_parameter_with_explicit_unit("t2", t2, "ns");
	      a_signal.set_shape_real_parameter_with_explicit_unit("amplitude", amplitude, "V");
	      a_signal.initialize_simple();
	      atomic_signal_collection.push_back(a_signal);

	      a_signal.tree_dump(std::clog, "Calo Hit signal in driver : ");
	      
	      std::clog << "Time stop : " << signal_time << std::endl;
	      std::clog << "Energy    : " << energy_deposit << std::endl;
	      std::clog << "Amplitude : " << amplitude << std::endl;
	      std::clog << "GID       : " << calo_gid << std::endl;
	    }
	  
	  // Merge signals which are in the same calo block (thanks to GID) :
	  for (auto it = map_gid_hit_in_gid.begin(); it != map_gid_hit_in_gid.end(); it++)
	    {
	      if (it->second == 1)
		{
		  // Signal alone :
		  for (size_t isig = 0; isig < atomic_signal_collection.size(); isig++)
		    {
		      if (atomic_signal_collection[isig].get_geom_id() == it->first)
			{
			  mctools::signal::base_signal & signal = sim_signal_data_.add_signal("calo");
			  signal = atomic_signal_collection[isig];
			}
		    }
		}
	      else 
		{
		  // Multi signal :
		  mctools::signal::base_signal & signal = sim_signal_data_.add_signal("calo");
		  datatools::properties multi_signal_config;
		  signal.set_shape_type_id("mctools::signal::multi_signal_shape");
		  for (size_t isig = 0; isig < atomic_signal_collection.size(); isig++)
		    {
		      if (atomic_signal_collection[isig].get_geom_id() == it->first)
			{
			  // One atomic signal useful to construct the multi signal
			  
			  // ici : builder qui permet de recréer les signaux pour construire le multi signal (et le push back dans sim signal data ??)
			  
			  

			  //std::vector<std::string> comp_labels({"hit0","hit1","hit2"});
			  // s5_cfg.store("components", comp_labels);

			  // multi_signal_config.store("components.hit0.key", "s1");
			  // multi_signal_config.store_real_with_explicit_unit("components.hit0.time_shift", 3.0 * CLHEP::ns);
			  // multi_signal_config.set_unit_symbol("components.hit0.time_shift", "ns");
			  // multi_signal_config.store_real("components.hit0.scaling", 0.5);

			  // multi_signal_config.store("components.hit1.key", "s2");
			  // multi_signal_config.store_real_with_explicit_unit("components.hit1.time_shift", -2.0 * CLHEP::ns);
			  // multi_signal_config.set_unit_symbol("components.hit1.time_shift", "ns");
			  // multi_signal_config.store_real("components.hit1.scaling", 1.0);

			  // multi_signal_config.store("components.hit2.key", "s4");
			  // multi_signal_config.store_real_with_explicit_unit("components.hit2.time_shift", 1.5 * CLHEP::ns);
			  // multi_signal_config.set_unit_symbol("components.hit2.time_shift", "ns");
			  // multi_signal_config.store_real("components.hit2.scaling", 1.3);
			  
			  //atomic_signal_collection[isig]
			}
		    } 
		}
	    }
			      
	      
	  // for (auto it_set = set_of_gids.begin(); it_set != set_of_gids.end(); it_set++)
	  //   {
	  //     geomtools::geom_id gid_to_search
	      
	  
	  for (size_t isignal = 0; isignal < atomic_signal_collection.size(); isignal++)
	    {
	      // mctools::signal::base_signal & signal = sim_signal_data_.add_signal("calo");
	      
	    }
	  
	  
	  

	  
	}
	      
      return;
    }
    
    void calo_signal_generator_driver::_tree_dump(std::ostream & out_,
						  const std::string & /* title_ */,
						  const std::string & indent_,
						  bool /* inherit_ */) const
    {
      
      std::string mode_str = "";
      if (get_mode() == MODE_INVALID) mode_str = "invalid";
      else if (get_mode() == MODE_TRIANGLE) mode_str = "triangle";
      
      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Mode : '" << mode_str << "'"
           << std::endl;
    }


  } // end of namespace asb

} // end of namespace snemo
