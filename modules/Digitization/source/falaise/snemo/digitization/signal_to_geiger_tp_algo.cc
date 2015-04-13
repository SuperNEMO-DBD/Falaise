// snemo/digitization/signal_to_geiger_tp_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>


// This project :
#include <snemo/digitization/clock_utils.h>

// Ourselves:
#include <snemo/digitization/signal_to_geiger_tp_algo.h>

namespace snemo {
  
  namespace digitization {
 
    signal_to_geiger_tp_algo::signal_to_tp_working_data::signal_to_tp_working_data()
    {
      reset();
    }

    void signal_to_geiger_tp_algo::signal_to_tp_working_data::reset()
    {
      signal_ref = 0;
      feb_id.reset();
      clocktick_800 = clock_utils::INVALID_CLOCKTICK;
      shift_800     = clock_utils::INVALID_CLOCKTICK; 
    }

    bool signal_to_geiger_tp_algo::signal_to_tp_working_data::operator<(const signal_to_tp_working_data & other_) const
    {
      return this-> clocktick_800 < other_.clocktick_800;
    }
    
    signal_to_geiger_tp_algo::signal_to_geiger_tp_algo()
    {
      _initialized_ = false;
      _ID_convertor_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    signal_to_geiger_tp_algo::~signal_to_geiger_tp_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void signal_to_geiger_tp_algo::initialize(const ID_convertor & my_ID_convertor_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to geiger tp algorithm is already initialized ! ");
      _ID_convertor_ = & my_ID_convertor_;
      
      _initialized_ = true;
      return;
    }
    
    bool signal_to_geiger_tp_algo::is_initialized() const
    {
      return _initialized_;
    }

    void signal_to_geiger_tp_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger tp algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _ID_convertor_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    void signal_to_geiger_tp_algo::set_clocktick_reference(int32_t clocktick_ref_)
    { 
      _clocktick_ref_ = clocktick_ref_;     
      return;
    }
			
    void signal_to_geiger_tp_algo::set_clocktick_shift(int32_t clocktick_shift_)
    {
      _clocktick_shift_ = clocktick_shift_;
      return;
    }
    
    // void signal_to_geiger_tp_algo::retrieve_gg_bitset(unsigned int active_bits_counter[])
    // {
    //   for (int i = 0; i < active_bits_counter
    //   return ;
    // }

    

    int signal_to_geiger_tp_algo::_prepare_working_data(const signal_data & signal_data_,
						       working_data_collection_type & wd_collection_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 
	unsigned int seed = 314159;
	std::srand(seed);
	size_t number_of_hits = signal_data_.get_geiger_signals().size(); //_number_of_step_hits("gg");
	std::clog << "DEBUG : BEGINING OF GEIGER PROCESS " << std::endl;
	std::clog << "**************************************************************" << std::endl;

	double time_reference = signal_data_.get_geiger_signals()[0].get().get_anode_avalanche_time();
	std::clog << "DEBUG : TIME REFERENCE = " << time_reference << std::endl;
	
	for (int i = 0; i < number_of_hits; i++)
	  {
	    if (signal_data_.get_geiger_signals()[i].get().get_anode_avalanche_time() < time_reference)
	      {
		time_reference = signal_data_.get_geiger_signals()[i].get().get_anode_avalanche_time();
	      }
	  }
	std::clog << "DEBUG : TIME REFERENCE = " << time_reference << std::endl;
	
	for (int i = 0; i < number_of_hits; i++)
	  {	 	    
	    const geiger_signal & a_geiger_signal = signal_data_.get_geiger_signals()[i].get();
	    const geomtools::geom_id & geom_id = a_geiger_signal.get_geom_id();
	    
	    geomtools::geom_id electronic_id;

	    int bit_index = 0;
	    
	    _ID_convertor_->find_gg_eid_and_tp_bit_index(mapping::THREE_WIRES_TRACKER_MODE,
							 geom_id,
							 electronic_id,
							 bit_index);
	    bool         existing = false;
	    unsigned int existing_index = 0;

	    double relative_time = a_geiger_signal.get_anode_avalanche_time() - time_reference ;
	    int32_t a_geiger_signal_clocktick = _clocktick_ref_;

	    if (relative_time > 800)
	      {
		a_geiger_signal_clocktick += static_cast<int32_t>(relative_time) / 800;
	      }

	    signal_to_tp_working_data a_working_data;
	    a_working_data.signal_ref    =& a_geiger_signal;
	    a_working_data.feb_id        = electronic_id;
	    a_working_data.clocktick_800 = a_geiger_signal_clocktick;
	    a_working_data.bit_index     = bit_index;
	      
	    wd_collection_.push_back(a_working_data);
	    std::clog << "DEBUG : VECTOR COLLECTION DATA SIZE = " << wd_collection_.size() << std::endl;	
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

    void signal_to_geiger_tp_algo::_sort_working_data(working_data_collection_type & wd_collection_)
    {
      std::sort(wd_collection_.begin(), wd_collection_.end());
      return;
    }

    int signal_to_geiger_tp_algo::_geiger_tp_process(const working_data_collection_type & wd_collection_,
						    geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 

	int32_t geiger_tp_hit_id = 0;

	for (int i = 0; i < wd_collection_.size(); i++)
	  {	    	    
	    const bool trigger_mode = 0;
	    const bool side_mode = 1;
	    const int  number_of_rows = 7;
	    bool existing = false;
	    unsigned int existing_index = 0;

	    for (int j = 0; j < my_geiger_tp_data_.get_geiger_tps().size(); j++)
	      {	
		if (my_geiger_tp_data_.get_geiger_tps()[j].get().get_geom_id() == wd_collection_[i].feb_id
		    && my_geiger_tp_data_.get_geiger_tps()[j].get().get_clocktick_800ns() == wd_collection_[i].clocktick_800)
		  {
		    existing = true;
		    existing_index = j;
		  }
	      } 
	      
	    if (existing == false)
	      {
		std::clog << "DEBUG : CASE 1 : none existing geiger TP " << std::endl;

		snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data_.add();
		gg_tp.set_header(geiger_tp_hit_id,
				 wd_collection_[i].feb_id,
				 wd_collection_[i].clocktick_800,
				 trigger_mode,
				 side_mode,
				 number_of_rows);
		gg_tp.set_gg_tp_active_bit(wd_collection_[i].bit_index);
		_active_bits_counter_[wd_collection_[i].bit_index] = 10;
		gg_tp.tree_dump(std::clog, "***** Geiger TP first creation : *****", "INFO : ");
	      }
	
	    else 
	      {
		for (int k = 0; k < TP_SIZE; k++)
		  {
		    if (_active_bits_counter_[k] != 0)
		      {
			
			
			
		      }
		    
		  }
		
		
		
		std::clog << "DEBUG : CASE 2 : already existing geiger TP " << std::endl;
		my_geiger_tp_data_.grab_geiger_tps()[existing_index].grab().set_gg_tp_active_bit(wd_collection_[i].bit_index);
		my_geiger_tp_data_.get_geiger_tps()[existing_index].get().tree_dump(std::clog, "***** Geiger TP Update : *****", "INFO : "); 
	      } 
	    geiger_tp_hit_id++;
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
    
    

    int signal_to_geiger_tp_algo::process(const signal_data & signal_data_,
					  geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 

	_process(signal_data_, my_geiger_tp_data_);

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


    int signal_to_geiger_tp_algo::_process(const signal_data & signal_data_,
					   geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int error_code = EXIT_SUCCESS;
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      try { 
	working_data_collection_type my_wd_collection;
	_prepare_working_data(signal_data_, my_wd_collection);
	_sort_working_data(my_wd_collection);
	_geiger_tp_process(my_wd_collection, my_geiger_tp_data_);
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
