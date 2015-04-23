// snemo/digitization/geiger_tp_to_ctw_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>

namespace snemo {
  
  namespace digitization {

    geiger_tp_to_ctw_algo::geiger_tp_to_ctw_algo()
    {
      _initialized_ = false;
      return;
    }

    geiger_tp_to_ctw_algo::~geiger_tp_to_ctw_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    bool geiger_tp_to_ctw_algo::is_initialized() const
    {
      return _initialized_;
    }

    void geiger_tp_to_ctw_algo::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Geiger tp to ctw algo is already initialized ! ")

      _initialized_ = true;
      return;
    }

    void geiger_tp_to_ctw_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Geiger tp to ctw algo is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }
    
    unsigned int geiger_tp_to_ctw_algo::board_to_index_mapping(uint32_t board_id_)
    {
      return board_id_;   
    }    

    void geiger_tp_to_ctw_algo::_fill_a_geiger_ctw(const geiger_tp & my_geiger_tp_, geiger_ctw & a_geiger_ctw_)
    {  
      geomtools::geom_id temporary_feb_id;
      temporary_feb_id.set_type(my_geiger_tp_.get_geom_id().get_type());
      temporary_feb_id.set_depth(mapping::BOARD_DEPTH);
      my_geiger_tp_.get_geom_id().extract_to(temporary_feb_id);
      temporary_feb_id.set(mapping::BOARD_INDEX, mapping::CONTROL_BOARD_ID);

      a_geiger_ctw_.set_header(my_geiger_tp_.get_hit_id(),
			       temporary_feb_id,
			       my_geiger_tp_.get_clocktick_800ns());
      unsigned int block_index = my_geiger_tp_.get_geom_id().get(mapping::BOARD_INDEX);
      if (block_index >  mapping::CONTROL_BOARD_ID) block_index -= 1;
      std::bitset<5> hardware_status = 0x0;
      std::bitset<2> crate_id = 0x0;
      my_geiger_tp_.get_hardware_status_bitset(hardware_status);
      my_geiger_tp_.get_crate_id(crate_id);
      a_geiger_ctw_.set_100_bits_in_ctw_word(block_index, my_geiger_tp_.get_gg_bitset());
      a_geiger_ctw_.set_full_hardware_status(hardware_status);
      a_geiger_ctw_.set_full_crate_id(crate_id);
      return;    
    }
    
    void geiger_tp_to_ctw_algo::_process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<geiger_tp> > & my_list_of_geiger_tp_,  geiger_ctw & a_geiger_ctw_) 
    {       
      for(int i = 0; i < my_list_of_geiger_tp_.size(); i++)
	{
	  const geiger_tp & my_geiger_tp = my_list_of_geiger_tp_[i].get();
	  _fill_a_geiger_ctw(my_geiger_tp, a_geiger_ctw_);	  
	}
      return;
    }

    void geiger_tp_to_ctw_algo::process(const geiger_tp_data & geiger_tp_data_,  geiger_ctw_data & geiger_ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Geiger tp to ctw algo is not initialized, it can't process ! ");

      for(int32_t i = geiger_tp_data_.get_clocktick_min(); i <= geiger_tp_data_.get_clocktick_max(); i++)
      	{
      	  for(int j = 0 ; j <= mapping::MAX_NUMBER_OF_CRATE ; j++) 
      	    {	  
      	      std::vector<datatools::handle<geiger_tp> > geiger_tp_list_per_clocktick_per_crate;
      	      geiger_tp_data_.get_list_of_gg_tp_per_clocktick_per_crate(i, j, geiger_tp_list_per_clocktick_per_crate);
      	      if(!geiger_tp_list_per_clocktick_per_crate.empty())
      		{
      		  geiger_ctw & a_geiger_ctw_ = geiger_ctw_data_.add();
      		  _process_for_a_ctw_for_a_clocktick(geiger_tp_list_per_clocktick_per_crate, a_geiger_ctw_);
      		  a_geiger_ctw_.tree_dump(std::clog, "a_geiger_ctw : ", "INFO : ");
      		}
      	    }
      	}
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
