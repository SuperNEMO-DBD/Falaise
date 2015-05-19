// snemo/digitization/signal_to_geiger_tp_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>


// This project :
#include <snemo/digitization/clock_utils.h>

// Ourselves:
#include <snemo/digitization/signal_to_geiger_tp_algo.h>
#include <snemo/digitization/geiger_tp_data.h>

namespace snemo {
  
  namespace digitization {

    const unsigned int signal_to_geiger_tp_algo::TRIGGER_READING_CLOCKTICK_SHIFT;

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
      _initialized_   = false;
      _electronic_mapping_  = 0;
      _clocktick_ref_ = clock_utils::INVALID_CLOCKTICK;
      datatools::invalidate(_clocktick_shift_);
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

    void signal_to_geiger_tp_algo::initialize(electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SD to geiger tp algorithm is already initialized ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      for (int i = 0; i < geiger::tp::TP_SIZE; i++)
	{
	  _activated_bits_[i] = 0;
	}
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
      _electronic_mapping_ = 0;
      _clocktick_ref_ = -1;
      return;
    }

    void signal_to_geiger_tp_algo::set_clocktick_reference(int32_t clocktick_ref_)
    { 
      _clocktick_ref_ = clocktick_ref_;     
      return;
    }
			
    void signal_to_geiger_tp_algo::set_clocktick_shift(double clocktick_shift_)
    {
      _clocktick_shift_ = clocktick_shift_;
      return;
    }

    void signal_to_geiger_tp_algo::add_geiger_tp(const signal_to_tp_working_data & my_wd_data_,
						 int32_t signal_clocktick_,
						 int32_t hit_id_,
						 geiger_tp_data & my_geiger_tp_data_)
    {
      snemo::digitization::geiger_tp & gg_tp = my_geiger_tp_data_.add();
      geomtools::geom_id temporary_feb_id;
      temporary_feb_id.set_type(my_wd_data_.feb_id.get_type());
      temporary_feb_id.set_depth(mapping::BOARD_DEPTH);
      my_wd_data_.feb_id.extract_to(temporary_feb_id);
      gg_tp.set_header(hit_id_,
		       temporary_feb_id,
		       signal_clocktick_,
		       mapping::THREE_WIRES_TRACKER_MODE,
		       mapping::SIDE_MODE,
		       mapping::NUMBER_OF_CONNECTED_ROWS);
      gg_tp.set_gg_tp_active_bit(my_wd_data_.feb_id.get(mapping::CHANNEL_INDEX));
      gg_tp.set_auxiliaries(my_wd_data_.auxiliaries);
      _activated_bits_[my_wd_data_.feb_id.get(mapping::CHANNEL_INDEX)] = 1;
      // gg_tp.tree_dump(std::clog, "***** Geiger TP creation : *****", "INFO : "); 

      return;
    }

    void signal_to_geiger_tp_algo::update_gg_tp(const signal_to_tp_working_data & my_wd_data_,
						geiger_tp & my_geiger_tp_)
    {
      my_geiger_tp_.set_gg_tp_active_bit(my_wd_data_.feb_id.get(mapping::CHANNEL_INDEX));
      _activated_bits_[my_wd_data_.feb_id.get(mapping::CHANNEL_INDEX)] = 1;
      // my_geiger_tp_.tree_dump(std::clog, "***** Geiger TP Update : *****", "INFO : ");
      return;
    }

    void signal_to_geiger_tp_algo::_prepare_working_data(const signal_data & signal_data_,
							 working_data_collection_type & wd_collection_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      unsigned int seed = 314159;
      std::srand(seed);
      size_t number_of_hits = signal_data_.get_geiger_signals().size(); //_number_of_step_hits("gg");
      std::clog << "DEBUG : BEGINING OF GEIGER PROCESS " << std::endl;
      std::clog << "**************************************************************" << std::endl;

      double time_reference = signal_data_.get_geiger_signals()[0].get().get_anode_avalanche_time();
	
      for (int i = 0; i < number_of_hits; i++)
	{
	  if (signal_data_.get_geiger_signals()[i].get().get_anode_avalanche_time() < time_reference)
	    {
	      time_reference = signal_data_.get_geiger_signals()[i].get().get_anode_avalanche_time();
	    }
	}
      for (int i = 0; i < number_of_hits; i++)
	{	 	    
	  const geiger_signal & a_geiger_signal    = signal_data_.get_geiger_signals()[i].get();
	  const geomtools::geom_id & geom_id       = a_geiger_signal.get_geom_id();
	  const datatools::properties & properties = a_geiger_signal.get_auxiliaries();
	  geomtools::geom_id electronic_id;

	  _electronic_mapping_->convert_GID_to_EID(mapping::THREE_WIRES_TRACKER_MODE, geom_id, electronic_id);

	  bool         existing = false;
	  unsigned int existing_index = 0;
	  double relative_time = a_geiger_signal.get_anode_avalanche_time() + _clocktick_shift_ - time_reference ;
	  int32_t a_geiger_signal_clocktick = _clocktick_ref_ + TRIGGER_READING_CLOCKTICK_SHIFT;

	  if (relative_time > 800)
	    {
	      a_geiger_signal_clocktick += static_cast<int32_t>(relative_time) / 800;
	    }
	  
	  signal_to_tp_working_data a_working_data;
	  a_working_data.signal_ref    = & a_geiger_signal;
	  a_working_data.feb_id        = electronic_id;
	  a_working_data.auxiliaries   = properties;
	  a_working_data.clocktick_800 = a_geiger_signal_clocktick;	      
	  wd_collection_.push_back(a_working_data);	
	}

      return ;
    }

    void signal_to_geiger_tp_algo::_sort_working_data(working_data_collection_type & wd_collection_)
    {
      std::sort(wd_collection_.begin(), wd_collection_.end());
      return;
    }

    void signal_to_geiger_tp_algo::_geiger_tp_process(const working_data_collection_type & wd_collection_,
						      geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      int32_t geiger_tp_hit_id = 0;
      std::clog << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Process Begin //////////////////////////////////////" << std::endl;
      for (int i = 0; i < wd_collection_.size(); i++)
	{
	  int32_t signal_clocktick  = wd_collection_[i].clocktick_800;	    
	  int existing_index = -1;
	  bool existing_eid  = false;
	  
	  for (int j = 0; j < clock_utils::ACTIVATED_GEIGER_CELLS_NUMBER; j++)
	    { 
	      bool existing_ct = false;
	      std::vector<datatools::handle<geiger_tp> > my_list_of_gg_tp_per_eid;

	      my_geiger_tp_data_.get_list_of_gg_tp_per_eid(wd_collection_[i].feb_id , my_list_of_gg_tp_per_eid);

	      if (my_list_of_gg_tp_per_eid.empty())
		{
		  existing_eid = false;
		  existing_index = -1;
		}
	      else
		{
		  existing_eid = true;
		  int iterator_list = 0;
		  while (existing_ct == false && iterator_list < my_list_of_gg_tp_per_eid.size())
		    {
		      if (signal_clocktick == my_list_of_gg_tp_per_eid[iterator_list].get().get_clocktick_800ns())
			{
			  existing_ct = true;
			  existing_index = iterator_list;
			}
		      iterator_list++;
		    } //end of while existing_ct == false
		}
	      // Eid is existing, clocktick is existing, gg tp update
	      if (existing_eid == true && existing_ct == true)
	      	{
		  update_gg_tp(wd_collection_[i], my_list_of_gg_tp_per_eid[existing_index].grab());
	      	}

	      // Eid is not existing or clocktick is different, geiger TP first creation
	      else
		{
		  add_geiger_tp(wd_collection_[i],
				signal_clocktick,
				geiger_tp_hit_id, 
				my_geiger_tp_data_);
		}
	      signal_clocktick++;
	    } // end of for (j < 10)

	} //end of for (i < wd_size)
      
      return;
    }
    
    void signal_to_geiger_tp_algo::process(const signal_data & signal_data_,
					   geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      _process(signal_data_, my_geiger_tp_data_);
      return;
    }

    void signal_to_geiger_tp_algo::_process(const signal_data & signal_data_,
					    geiger_tp_data & my_geiger_tp_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "SD to geiger TP algorithm is not initialized ! ");
      working_data_collection_type my_wd_collection;
      _prepare_working_data(signal_data_, my_wd_collection);
      _sort_working_data(my_wd_collection);
      _geiger_tp_process(my_wd_collection, my_geiger_tp_data_);
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
