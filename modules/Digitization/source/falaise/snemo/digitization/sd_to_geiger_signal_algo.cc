// snemo/digitization/sd_to_geiger_signal_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/sd_to_geiger_signal_algo.h>

namespace snemo {
  
  namespace digitization {

    sd_to_geiger_signal_algo::sd_to_geiger_signal_algo()
    {
      _initialized_ = false;
      _geo_manager_ = 0;
      return;
    }

    sd_to_geiger_signal_algo::sd_to_geiger_signal_algo(const geomtools::manager & mgr_)
    {
      _initialized_ = false;
      set_geo_manager(mgr_);
      return;
    }

    sd_to_geiger_signal_algo::~sd_to_geiger_signal_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void sd_to_geiger_signal_algo::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to geiger signal algorithm is already initialized ! ");
      _initialized_ = true;
      return;
    }
    
    bool sd_to_geiger_signal_algo::is_initialized() const
    {
      return _initialized_;
    }

    void sd_to_geiger_signal_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger signal algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _geo_manager_ = 0;
      return;
    }

    void sd_to_geiger_signal_algo::set_geo_manager(const geomtools::manager & mgr_ )
    {
      _geo_manager_ = & mgr_;
      return;
    }

    int sd_to_geiger_signal_algo::process(const mctools::simulated_data & sd_,
					  geiger_signal_data & geiger_signal_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger signal algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try 
	{ 	
	  _process(sd_, geiger_signal_);
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

    const double sd_to_geiger_signal_algo::_anode_drift_time_calculation(const double drift_distance_)
    {
      // 2,3 cm/us drift close to the anode
      // 1 cm/us drift far away the anode
         
      return 2.3 * drift_distance_;
    }

    int sd_to_geiger_signal_algo::_process(const mctools::simulated_data & sd_,
					   geiger_signal_data & geiger_signal_data)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger signal algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 
	std::clog << "DEBUG : BEGINING OF PROCESS " << std::endl;
	std::clog << "**************************************************************" << std::endl;
	// pickup the ID mapping from the geometry manager:
	const geomtools::mapping & the_mapping = _geo_manager_->get_mapping();

	// Loop on Geiger step hits:
	const size_t number_of_hits = sd_.get_number_of_step_hits("gg");
	for (size_t ihit = 0; ihit < number_of_hits; ihit++)
	  {
	    const mctools::base_step_hit & geiger_hit = sd_.get_step_hit("gg", ihit);
	    
	    // extract the corresponding geom ID:
	    const geomtools::geom_id & geiger_gid = geiger_hit.get_geom_id();

	    // extract the geom info of the corresponding cell:
	    const geomtools::geom_info & ginfo = the_mapping.get_geom_info(geiger_gid);

	    // the position of the ion/electron pair creation within the cell volume:
	    const geomtools::vector_3d & ionization_world_pos = geiger_hit.get_position_start();

	    // the position of the Geiger avalanche impact on the anode wire:
	    const geomtools::vector_3d & avalanche_impact_world_pos = geiger_hit.get_position_stop();

	    // compute the position of the anode impact in the drift cell coordinates reference frame:
	    geomtools::vector_3d avalanche_impact_cell_pos;
	    ginfo.get_world_placement().mother_to_child(avalanche_impact_world_pos,
							avalanche_impact_cell_pos);
	    // longitudinal position:
	    const double longitudinal_position = avalanche_impact_cell_pos.z();

	    // true drift distance:
	    const double drift_distance =(avalanche_impact_world_pos - ionization_world_pos).mag();

	    // the time of the ion/electron pair creation:
	    const double ionization_time = geiger_hit.get_time_start();

	    /*** Anode TDC ***/
	    // randomize the expected Geiger drift time:
	    const double expected_drift_time = _anode_drift_time_calculation(drift_distance);

	    const double anode_time          = ionization_time + expected_drift_time;
	    
	    geiger_signal & gg_signal = geiger_signal_data.add();
	    gg_signal.set_anode_avalanche_time(anode_time);
	    
	    gg_signal.tree_dump(std::clog, "Geiger signal : ", "INFO : ");


	  }	  
	
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
