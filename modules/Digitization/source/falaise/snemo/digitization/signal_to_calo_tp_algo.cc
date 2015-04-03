// snemo/digitization/sd_to_calo_tp_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/signal_to_calo_tp_algo.h>

namespace snemo {
  
  namespace digitization {

    sd_to_calo_tp_algo::sd_to_calo_tp_algo()
    {
      _initialized_ = false;
      _ID_convertor_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    sd_to_calo_tp_algo::~sd_to_calo_tp_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void sd_to_calo_tp_algo::initialize(int32_t & clocktick_ref_,
					int32_t & clocktick_shift_,
					const ID_convertor & my_ID_convertor_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to calo tp algorithm is already initialized ! ");
      _clocktick_ref_ = clocktick_ref_;
      _clocktick_shift_ = clocktick_shift_;
      _ID_convertor_ = & my_ID_convertor_;
      
      _initialized_ = true;
      return;
    }
    
    bool sd_to_calo_tp_algo::is_initialized() const
    {
      return _initialized_;
    }

    void sd_to_calo_tp_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo tp algorithm is already initialized ! ");
      _initialized_ = false;
      _ID_convertor_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    // void sd_to_calo_tp_algo::set_clocktick_reference(int32_t clocktick_ref_)
    // { 
    //   _clocktick_ref_ = clocktick_ref_;     
    //   return;
    // }
			
    // void sd_to_calo_tp_algo::set_clocktick_shift(int32_t clocktick_shift_)
    // {
    //   _clocktick_shift_ = clocktick_shift_;
    //   return;
    // }

    int sd_to_calo_tp_algo::process(const mctools::simulated_data & sd_,
				    calo_tp_data & my_calo_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 

	_process(sd_, my_calo_tp_data_);

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

    // unsigned int sd_to_calo_tp_algo::_existing_same_electronic_id(const geomtools::geom_id & electronic_id_,
    // 								  calo_tp_data & my_calo_tp_data_)
    // {
    //   // 2 VVALUES to return , bool and uint (is existing and the index of existing calo tp)
    //   unsigned int existing_index = 0;

    //   for (int i = 0; i < my_calo_tp_data_.get_calo_tps().size(); i++)
    // 	{
    // 	  if (my_calo_tp_data_.get_calo_tps()[i].get().get_geom_id() == electronic_id_)
    // 	    {
    // 	      existing_index = j;
    // 	    }
    // 	}

    //   return existing_index;
    // } 

    int sd_to_calo_tp_algo::_process(const mctools::simulated_data & sd_,
				     calo_tp_data & my_calo_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to calo TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 
	unsigned int seed = 314159;
	std::srand(seed);
	size_t number_of_hits = sd_.get_number_of_step_hits("calo");
	std::clog << "DEBUG : BEGINING OF PROCESS " << std::endl;
	std::clog << "**************************************************************" << std::endl;

	double time_reference = sd_.get_step_hit("calo", 0).get_time_start();
	std::clog << "DEBUG : TIME REFERENCE = " << time_reference << std::endl;
	for (int i = 0; i < number_of_hits; i++)
	  {
	    if (sd_.get_step_hit("calo", i).get_time_start() < time_reference)
	      {
		time_reference = sd_.get_step_hit("calo", i).get_time_start();
	      }
	  }
	std::clog << "DEBUG : TIME REFERENCE = " << time_reference << std::endl;

	for (int i = 0; i < number_of_hits; i++)
	  {	 	    
	    const mctools::base_step_hit & calo_hit = sd_.get_step_hit("calo", i);
	    calo_hit.tree_dump(std::clog);
	   
	    const geomtools::geom_id & geom_id = calo_hit.get_geom_id();
	    
	    geomtools::geom_id electronic_id;
	    
	    electronic_id = _ID_convertor_->convert_GID_to_EID(geom_id);
	    
	    std::clog << "DEBUG sd_to_calo_tp_algo : hit id = " << calo_hit.get_hit_id() << ": GID = " << calo_hit.get_geom_id() << " EID = " << electronic_id << std::endl;	   
	    
	    bool existing = false;
	    unsigned int existing_index = 0;

	    double relative_time = calo_hit.get_time_start() - time_reference ;
	    int32_t calo_hit_clocktick = _clocktick_ref_;

	    if (relative_time > 25)
	      {
		calo_hit_clocktick += static_cast<int32_t>(relative_time) / 25;
	      }

	    for (int j = 0; j < my_calo_tp_data_.get_calo_tps().size(); j++)
	      {
		if (my_calo_tp_data_.get_calo_tps()[j].get().get_geom_id() == electronic_id
		    && my_calo_tp_data_.get_calo_tps()[j].get().get_clocktick_25ns() == calo_hit_clocktick )
		  {
		    std::clog << "DEBUG : Case 1 EID = EID in calo tp data " <<  std::endl;
		    existing = true;
		    existing_index = j;
		  }
	      }

	    if (existing == false)
	      {
		std::clog << "DEBUG : Case 2 EID != EID in calo tp data " <<  std::endl;
		// double relative_time = calo_hit.get_time_start() - time_reference ;
		// int32_t calo_tp_clocktick = _clocktick_ref_;
		// if (relative_time > 25)
		//   {
		//     calo_hit_clocktick += static_cast<int32_t>(relative_time) / 25;
		//   }
		// double random_number = drand48();

		snemo::digitization::calo_tp & ctp = my_calo_tp_data_.add();
		ctp.set_header(calo_hit.get_hit_id(), electronic_id, calo_hit_clocktick);
		ctp.set_htm(1);
		ctp.tree_dump(std::clog, "CTP : ", "INFO : ");
		//ctp.lock_tp();
	      }
	
	    else 
	      {
		unsigned int old_htm = my_calo_tp_data_.get_calo_tps()[existing_index].get().get_htm();
		bool old_lto = my_calo_tp_data_.get_calo_tps()[existing_index].get().is_lto();
		bool old_xt = my_calo_tp_data_.get_calo_tps()[existing_index].get().is_xt();
		bool old_spare = my_calo_tp_data_.get_calo_tps()[existing_index].get().is_spare();

		my_calo_tp_data_.grab_calo_tps()[existing_index].grab().set_data(old_htm + 1, old_lto, old_xt, old_spare);
		my_calo_tp_data_.grab_calo_tps()[existing_index].get().tree_dump(std::clog, "CTP : ", "INFO : ");
		
		// my_calo_tp_data_.get_calo_tps()[existing_index]
		// update calo TP (HTM / LTO) 	    
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
