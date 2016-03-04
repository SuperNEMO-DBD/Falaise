/// \file falaise/snemo/digitization/fake_trigger_algo.cc
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <falaise/snemo/digitization/fake_trigger_algo.h>

namespace snemo {
  
  namespace digitization {
    
    fake_trigger_algo::fake_trigger_algo()
    {
      return;
    }
  
    fake_trigger_algo::~fake_trigger_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    bool fake_trigger_algo::is_initialized() const
    {
      return _initialized_;
    }
         
    void fake_trigger_algo::initialize()
    {
      
      _initialized_ = true;
      return;
    }
    
    void fake_trigger_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Fake trigger algo is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }
            
    bool fake_trigger_algo::process(const mctools::simulated_data & SD_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Fake trigger algo is not initialized, it can't process ! ");
      bool process_response = false;
      process_response = _process(SD_);
      return process_response;
    }
    
    bool fake_trigger_algo::_process(const mctools::simulated_data & SD_)
    {
      std::size_t number_of_calorimeters = 0;
      std::size_t number_of_geiger_cells = 0;
      
      if (SD_.has_step_hits("calo"))
	{
	  // Loop on main calo step hits:
	  const size_t number_of_main_calo_hits = SD_.get_number_of_step_hits("calo");
	  std::set<geomtools::geom_id> set_of_gid;
	  
	  for (size_t ihit = 0; ihit < number_of_main_calo_hits; ihit++)
	    {
	      const mctools::base_step_hit & main_calo_hit = SD_.get_step_hit("calo", ihit);	
	      const double signal_time    = main_calo_hit.get_time_stop();
	      const double energy_deposit = main_calo_hit.get_energy_deposit();
	      const geomtools::geom_id & calo_gid = main_calo_hit.get_geom_id();
	      
	      if (!set_of_gid.count(calo_gid))
		{
		  // Calorimeter first hit :
		  set_of_gid.insert(calo_gid);
		}
	      else 
		{
		  // Already hit :
		}
	    }
	  number_of_calorimeters += set_of_gid.size();
	}
   
      if (SD_.has_step_hits("xcalo"))
	{
	  // Loop on xcalo step hits:
	  const size_t number_of_xwall_calo_hits = SD_.get_number_of_step_hits("xcalo");
	  
	  std::set<geomtools::geom_id> set_of_gid;
	  
	  for (size_t ihit = 0; ihit < number_of_xwall_calo_hits; ihit++)
	    {
	      const mctools::base_step_hit & xcalo_hit = SD_.get_step_hit("xcalo", ihit);	
	      const double signal_time    = xcalo_hit.get_time_stop();
	      const double energy_deposit = xcalo_hit.get_energy_deposit();
	      const geomtools::geom_id & xcalo_gid = xcalo_hit.get_geom_id();
	      
	      if (!set_of_gid.count(xcalo_gid))
		{
		  // Calorimeter first hit :
		  set_of_gid.insert(xcalo_gid);
		}
	      else 
		{
		  // Already hit :
		}
	    }
	  number_of_calorimeters += set_of_gid.size();
	}
     
      if (SD_.has_step_hits("gveto"))
	{
	  // Loop on gveto calo step hits:
	  const size_t number_of_gveto_calo_hits = SD_.get_number_of_step_hits("gveto");
	  
	  std::set<geomtools::geom_id> set_of_gid;
	  
	  for (size_t ihit = 0; ihit < number_of_gveto_calo_hits; ihit++)
	    {
	      const mctools::base_step_hit & gveto_hit = SD_.get_step_hit("gveto", ihit);	
	      const double signal_time    = gveto_hit.get_time_stop();
	      const double energy_deposit = gveto_hit.get_energy_deposit();
	      const geomtools::geom_id & gveto_gid = gveto_hit.get_geom_id();
	      
	      if (!set_of_gid.count(gveto_gid))
		{
		  // Calorimeter first hit :
		  set_of_gid.insert(gveto_gid);
		}
	      else 
		{
		  // Already hit :
		}
	    }
	  number_of_calorimeters += set_of_gid.size();
	}	    
      
      if (SD_.has_step_hits("gg"))
	{
	  // Loop on Geiger step hits:
	  const size_t number_of_hits = SD_.get_number_of_step_hits("gg");
	  
	  std::set<geomtools::geom_id> set_of_gg_gid;
	  for (size_t ihit = 0; ihit < number_of_hits; ihit++)
	    {
	      const mctools::base_step_hit & geiger_hit = SD_.get_step_hit("gg", ihit);	    
	      const geomtools::geom_id & gg_gid = geiger_hit.get_geom_id();
	      if (!set_of_gg_gid.count(gg_gid))
		{
		  // First hit
		  set_of_gg_gid.insert(gg_gid);
		}
	      else 
		{
		  // Already hit
		}
	    }
	  number_of_geiger_cells = set_of_gg_gid.size();	  
	}
      
      if (number_of_calorimeters >= 1 && number_of_geiger_cells >= 3) _status_for_a_SD_ = true;
      else _status_for_a_SD_ = false;
            
      return _status_for_a_SD_;
    }
    
  } // end of namespace digitization

} // end of namespace snemo
