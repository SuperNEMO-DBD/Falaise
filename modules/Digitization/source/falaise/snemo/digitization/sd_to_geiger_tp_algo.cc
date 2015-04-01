// snemo/digitization/sd_to_geiger_tp_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/sd_to_geiger_tp_algo.h>

namespace snemo {
  
  namespace digitization {

    sd_to_geiger_tp_algo::sd_to_geiger_tp_algo()
    {
      _initialized_ = false;
      _ID_convertor_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    sd_to_geiger_tp_algo::~sd_to_geiger_tp_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void sd_to_geiger_tp_algo::initialize(int32_t & clocktick_ref_,
					  int32_t & clocktick_shift_,
					  const ID_convertor & my_ID_convertor_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to geiger tp algorithm is already initialized ! ");
      _clocktick_ref_ = clocktick_ref_;
      _clocktick_shift_ = clocktick_shift_;
      _ID_convertor_ = & my_ID_convertor_;
      
      _initialized_ = true;
      return;
    }
    
    bool sd_to_geiger_tp_algo::is_initialized() const
    {
      return _initialized_;
    }

    void sd_to_geiger_tp_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger tp algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _ID_convertor_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    // void sd_to_geiger_tp_algo::set_clocktick_reference(int32_t clocktick_ref_)
    // { 
    //   _clocktick_ref_ = clocktick_ref_;     
    //   return;
    // }
			
    // void sd_to_geiger_tp_algo::set_clocktick_shift(int32_t clocktick_shift_)
    // {
    //   _clocktick_shift_ = clocktick_shift_;
    //   return;
    // }

    int sd_to_geiger_tp_algo::process(const mctools::simulated_data & sd_,
				      geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 

	_process(sd_, my_geiger_tp_data_);

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

    // unsigned int sd_to_geiger_tp_algo::_existing_same_electronic_id(const geomtools::geom_id & electronic_id_,
    // 								  geiger_tp_data & my_geiger_tp_data_)
    // {
    //   // 2 VVALUES to return , bool and uint (is existing and the index of existing geiger tp)
    //   unsigned int existing_index = 0;

    //   for (int i = 0; i < my_geiger_tp_data_.get_geiger_tps().size(); i++)
    // 	{
    // 	  if (my_geiger_tp_data_.get_geiger_tps()[i].get().get_geom_id() == electronic_id_)
    // 	    {
    // 	      existing_index = j;
    // 	    }
    // 	}

    //   return existing_index;
    // } 

    int sd_to_geiger_tp_algo::_process(const mctools::simulated_data & sd_,
				       geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 
	unsigned int seed = 314159;
	std::srand(seed);
	size_t number_of_hits = sd_.get_number_of_step_hits("gg");
	std::clog << "DEBUG : BEGINING OF PROCESS " << std::endl;
	std::clog << "**************************************************************" << std::endl;

	double time_reference = sd_.get_step_hit("gg", 0).get_time_start();
	std::clog << "DEBUG : TIME REFERENCE = " << time_reference << std::endl;
	for (int i = 0; i < number_of_hits; i++)
	  {
	    if (sd_.get_step_hit("gg", i).get_time_start() < time_reference)
	      {
		time_reference = sd_.get_step_hit("gg", i).get_time_start();
	      }
	  }
	std::clog << "DEBUG : TIME REFERENCE = " << time_reference << std::endl;

	for (int i = 0; i < number_of_hits; i++)
	  {	 	    
	    const mctools::base_step_hit & geiger_hit = sd_.get_step_hit("gg", i);
	    geiger_hit.tree_dump(std::clog);
	   
	    const geomtools::geom_id & geom_id = geiger_hit.get_geom_id();
	    
	    geomtools::geom_id electronic_id;
	    
	    bool trigger_mode = 0;
	    bool side_mode = 1;
	    int number_of_rows = 7;
	      
	    //electronic_id = _ID_convertor_->convert_GID_to_EID(geom_id);
	    int bit_index = 0;
	    _ID_convertor_->find_gg_eid_and_tp_bit_index(geom_id, electronic_id, bit_index);
	    
	    std::clog << "DEBUG sd_to_geiger_tp_algo : hit id = " << geiger_hit.get_hit_id() << ": GID = " << geiger_hit.get_geom_id() << " EID = " << electronic_id << std::endl;	   
	    
	    bool existing = false;
	    unsigned int existing_index = 0;

	    double relative_time = geiger_hit.get_time_start() - time_reference ;
	    int32_t geiger_hit_clocktick = _clocktick_ref_;

	    if (relative_time > 3)
	      {
		geiger_hit_clocktick += static_cast<int32_t>(relative_time) / 3;
	      }

	    for (int j = 0; j < my_geiger_tp_data_.get_geiger_tps().size(); j++)
	      {
		if (my_geiger_tp_data_.get_geiger_tps()[j].get().get_geom_id() == electronic_id
		    && my_geiger_tp_data_.get_geiger_tps()[j].get().get_clocktick_800ns() == geiger_hit_clocktick )
		  {
		    existing = true;
		    existing_index = j;
		  }
	      }

	    if (existing == false)
	      {
		std::clog << "DEBUG : CASE 1 : none existing geiger TP " << std::endl;

		snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data_.add();
		gg_tp.set_header(geiger_hit.get_hit_id(),
				 electronic_id,
				 geiger_hit_clocktick,
				 trigger_mode,
				 side_mode,
				 number_of_rows);
		gg_tp.set_gg_tp_active_bit(bit_index);
		gg_tp.tree_dump(std::clog, "Geiger TP first creation : ", "INFO : ");
		//gg_tp.lock_tp();
	      }
	
	    else 
	      {
		std::clog << "DEBUG : CASE 2 : already existing geiger TP " << std::endl;
		my_geiger_tp_data_.grab_geiger_tps()[existing_index].grab().set_gg_tp_active_bit(bit_index);
		my_geiger_tp_data_.get_geiger_tps()[existing_index].get().tree_dump(std::clog, "Geiger TP Update : ", "INFO : ");
		// my_geiger_tp_data_.get_geiger_tps()[existing_index]
		// update geiger TP 	    
	      } 
	    

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
