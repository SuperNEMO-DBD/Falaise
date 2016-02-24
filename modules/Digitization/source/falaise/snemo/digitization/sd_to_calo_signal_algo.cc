// snemo/digitization/sd_to_calo_signal_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Third party:
// - Bayeux/datatools :
#include <datatools/clhep_units.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
// Ourselves:
#include <snemo/digitization/sd_to_calo_signal_algo.h>

namespace snemo {
  
  namespace digitization {

    sd_to_calo_signal_algo::sd_to_calo_signal_algo()
    {
      _initialized_ = false;
      _geo_manager_ = 0;
      return;
    }

    sd_to_calo_signal_algo::sd_to_calo_signal_algo(const geomtools::manager & mgr_)
    {
      _initialized_ = false;
      set_geo_manager(mgr_);
      return;
    }

    sd_to_calo_signal_algo::~sd_to_calo_signal_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void sd_to_calo_signal_algo::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to calo signal algorithm is already initialized ! ");
      _initialized_ = true;
      return;
    }
    
    bool sd_to_calo_signal_algo::is_initialized() const
    {
      return _initialized_;
    }

    void sd_to_calo_signal_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo signal algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _geo_manager_ = 0;
      return;
    }

    void sd_to_calo_signal_algo::set_geo_manager(const geomtools::manager & mgr_ )
    {
      _geo_manager_ = & mgr_;
      return;
    }

    double sd_to_calo_signal_algo::_convert_energy_to_amplitude(const double energy_)
    {
      // 1 MeV is equivalent to 300 mV
      return energy_ * 300; // maybe units problem for the moment
    }

    void sd_to_calo_signal_algo::process(const mctools::simulated_data & sd_,
					 signal_data & signal_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo signal algorithm is not initialized ! ");	
      _process(sd_, signal_data_);
      return ;
    }

    int sd_to_calo_signal_algo::_process(const mctools::simulated_data & sd_,
					 signal_data & signal_data)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo signal algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 
	// pickup the ID mapping from the geometry manager:
	const geomtools::mapping & the_mapping = _geo_manager_->get_mapping();

	int32_t calo_signal_hit_id = 0;
	if (sd_.has_step_hits("calo"))
	  {
	    // Loop on Main calo step hits:
	    const size_t number_of_main_calo_hits = sd_.get_number_of_step_hits("calo");
	    
	    std::set<geomtools::geom_id> set_of_hit_gid;

	    for (size_t ihit = 0; ihit < number_of_main_calo_hits; ihit++)
	      {
		const mctools::base_step_hit & main_calo_hit = sd_.get_step_hit("calo", ihit);	
		const double signal_time    = main_calo_hit.get_time_stop();
		const double energy_deposit = main_calo_hit.get_energy_deposit();
		const double amplitude      = _convert_energy_to_amplitude(energy_deposit);
		const geomtools::geom_id & calo_gid = main_calo_hit.get_geom_id();
		if (signal_data.get_calo_signals().size() == 0)
		  {
		    set_of_hit_gid.insert(calo_gid);
		    calo_signal & calo_signal = signal_data.add_calo_signal();
		    calo_signal.set_header(calo_signal_hit_id, calo_gid);
		    calo_signal.set_data(signal_time, amplitude); 
		    calo_signal.grab_auxiliaries().store("hit.id", main_calo_hit.get_hit_id());
		    calo_signal_hit_id++;
		  }
		else 
		  {
		    int signal_data_size = signal_data.get_calo_signals().size();
		    bool already_hit = false;
		    
		    std::set<geomtools::geom_id>::iterator it_set = set_of_hit_gid.begin();
		    for (it_set; it_set != set_of_hit_gid.end(); it_set++)
		      {
			geomtools::geom_id a_gid = *it_set;
			if (calo_gid == a_gid) already_hit = true;
		      }
		    
		    if (already_hit) // update calo signal
		      {
			int hit_calo_signal_position = -1;
			for (int jsig = 0; jsig < signal_data_size; jsig++)
			  {
			    calo_signal a_calo_signal = signal_data.grab_calo_signals()[jsig].grab();
			    if (calo_gid == a_calo_signal.get_geom_id()) hit_calo_signal_position = jsig;
			  }
			
			calo_signal & already_hit_calo_signal = signal_data.grab_calo_signals()[hit_calo_signal_position].grab();
			if (main_calo_hit.get_geom_id() == already_hit_calo_signal.get_geom_id())
			  {
			    double max_time = already_hit_calo_signal.get_signal_time();
			    if (signal_time > max_time) max_time = signal_time;
			    const double amplitude_sum = already_hit_calo_signal.get_amplitude() + amplitude;
			    already_hit_calo_signal.set_data(max_time - calo_signal::DELAYED_PM_TIME, amplitude_sum);
			  }
		      }
		    else  // create new calo signal
		      {
			set_of_hit_gid.insert(calo_gid);
			calo_signal & calo_signal = signal_data.add_calo_signal();
			calo_signal.set_header(calo_signal_hit_id, calo_gid);
			calo_signal.set_data(signal_time, amplitude); 
			calo_signal.grab_auxiliaries().store("hit.id", main_calo_hit.get_hit_id());
			calo_signal_hit_id++;
		      }
		  }
	      }
	  } // end of if has main calo 
	
	if (sd_.has_step_hits("xcalo"))
	  {
	    // Loop on Xwall calo step hits:
	    const size_t number_of_xwall_calo_hits = sd_.get_number_of_step_hits("xcalo");
	    
	    std::set<geomtools::geom_id> set_of_hit_gid;

	    for (size_t ihit = 0; ihit < number_of_xwall_calo_hits; ihit++)
	      {
		const mctools::base_step_hit & xwall_calo_hit = sd_.get_step_hit("xcalo", ihit);	
		const double signal_time    = xwall_calo_hit.get_time_stop();
		const double energy_deposit = xwall_calo_hit.get_energy_deposit();
		const double amplitude      = _convert_energy_to_amplitude(energy_deposit);
		const geomtools::geom_id & calo_gid = xwall_calo_hit.get_geom_id();
		if (signal_data.get_calo_signals().size() == 0)
		  {
		    set_of_hit_gid.insert(calo_gid);
		    calo_signal & calo_signal = signal_data.add_calo_signal();
		    calo_signal.set_header(calo_signal_hit_id, calo_gid);
		    calo_signal.set_data(signal_time, amplitude); 
		    calo_signal.grab_auxiliaries().store("hit.id", xwall_calo_hit.get_hit_id());
		    calo_signal_hit_id++;
		  }
		else 
		  {
		    int signal_data_size = signal_data.get_calo_signals().size();
		    bool already_hit = false;
		    
		    std::set<geomtools::geom_id>::iterator it_set = set_of_hit_gid.begin();
		    for (it_set; it_set != set_of_hit_gid.end(); it_set++)
		      {
			geomtools::geom_id a_gid = *it_set;
			if (calo_gid == a_gid) already_hit = true;
		      }
		    
		    if (already_hit) // update calo signal
		      {
			int hit_calo_signal_position = -1;
			for (int jsig = 0; jsig < signal_data_size; jsig++)
			  {
			    calo_signal a_calo_signal = signal_data.grab_calo_signals()[jsig].grab();
			    if (calo_gid == a_calo_signal.get_geom_id()) hit_calo_signal_position = jsig;
			  }
			
			calo_signal & already_hit_calo_signal = signal_data.grab_calo_signals()[hit_calo_signal_position].grab();
			if (xwall_calo_hit.get_geom_id() == already_hit_calo_signal.get_geom_id())
			  {
			    double max_time = already_hit_calo_signal.get_signal_time();
			    if (signal_time > max_time) max_time = signal_time;
			    const double amplitude_sum = already_hit_calo_signal.get_amplitude() + amplitude;
			    already_hit_calo_signal.set_data(max_time - calo_signal::DELAYED_PM_TIME, amplitude_sum);
			  }
		      }
		    else  // create new calo signal
		      {
			set_of_hit_gid.insert(calo_gid);
			calo_signal & calo_signal = signal_data.add_calo_signal();
			calo_signal.set_header(calo_signal_hit_id, calo_gid);
			calo_signal.set_data(signal_time, amplitude); 
			calo_signal.grab_auxiliaries().store("hit.id", xwall_calo_hit.get_hit_id());
			calo_signal_hit_id++;
		      }
		  }
	      }
	  } // end of if has xcalo
      
	if (sd_.has_step_hits("gveto"))
	  {
	    // Loop on Gveto calo step hits:
	    const size_t number_of_gveto_calo_hits = sd_.get_number_of_step_hits("gveto");
	    
	    std::set<geomtools::geom_id> set_of_hit_gid;

	    for (size_t ihit = 0; ihit < number_of_gveto_calo_hits; ihit++)
	      {
		const mctools::base_step_hit & gveto_calo_hit = sd_.get_step_hit("gveto", ihit);	
		const double signal_time    = gveto_calo_hit.get_time_stop();
		const double energy_deposit = gveto_calo_hit.get_energy_deposit();
		const double amplitude      = _convert_energy_to_amplitude(energy_deposit);
		const geomtools::geom_id & calo_gid = gveto_calo_hit.get_geom_id();
		if (signal_data.get_calo_signals().size() == 0)
		  {
		    set_of_hit_gid.insert(calo_gid);
		    calo_signal & calo_signal = signal_data.add_calo_signal();
		    calo_signal.set_header(calo_signal_hit_id, calo_gid);
		    calo_signal.set_data(signal_time, amplitude); 
		    calo_signal.grab_auxiliaries().store("hit.id", gveto_calo_hit.get_hit_id());
		    calo_signal_hit_id++;
		  }
		else 
		  {
		    int signal_data_size = signal_data.get_calo_signals().size();
		    bool already_hit = false;
		    
		    std::set<geomtools::geom_id>::iterator it_set = set_of_hit_gid.begin();
		    for (it_set; it_set != set_of_hit_gid.end(); it_set++)
		      {
			geomtools::geom_id a_gid = *it_set;
			if (calo_gid == a_gid) already_hit = true;
		      }
		    
		    if (already_hit) // update calo signal
		      {
			int hit_calo_signal_position = -1;
			for (int jsig = 0; jsig < signal_data_size; jsig++)
			  {
			    calo_signal a_calo_signal = signal_data.grab_calo_signals()[jsig].grab();
			    if (calo_gid == a_calo_signal.get_geom_id()) hit_calo_signal_position = jsig;
			  }
			
			calo_signal & already_hit_calo_signal = signal_data.grab_calo_signals()[hit_calo_signal_position].grab();
			if (gveto_calo_hit.get_geom_id() == already_hit_calo_signal.get_geom_id())
			  {
			    double max_time = already_hit_calo_signal.get_signal_time();
			    if (signal_time > max_time) max_time = signal_time;
			    const double amplitude_sum = already_hit_calo_signal.get_amplitude() + amplitude;
			    already_hit_calo_signal.set_data(max_time - calo_signal::DELAYED_PM_TIME, amplitude_sum);
			  }
		      }
		    else  // create new calo signal
		      {
			set_of_hit_gid.insert(calo_gid);
			calo_signal & calo_signal = signal_data.add_calo_signal();
			calo_signal.set_header(calo_signal_hit_id, calo_gid);
			calo_signal.set_data(signal_time, amplitude); 
			calo_signal.grab_auxiliaries().store("hit.id", gveto_calo_hit.get_hit_id());
			calo_signal_hit_id++;
		      }
		  }
	      }
	  } // end of if has gveto calo
      
      }
      catch (std::exception & error) {
	DT_LOG_FATAL(logging, error.what());
	error_code = EXIT_FAILURE;
      }
      
      catch (...) {
	DT_LOG_FATAL(logging, "Unexpected error!");
	error_code = EXIT_FAILURE;
      }
      
      return error_code;
    }
    
  } // end of namespace digitization

} // end of namespace snemo
