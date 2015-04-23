// snemo/digitization/trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <vector>

// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// Ourselves:
#include <snemo/digitization/trigger_algorithm.h>
#include <snemo/digitization/geiger_tp_constants.h>

namespace snemo {
  
  namespace digitization {


    trigger_algorithm::trigger_algorithm()
    {
      _initialized_ = false;
     _electronic_mapping_ = 0;
     return;
    }

    trigger_algorithm::~trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void trigger_algorithm::initialize(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      _initialized_ = true;
      return;
    }

    bool trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }
    void trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      return;
    }

    uint32_t trigger_algorithm::get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const
    {
      std::bitset<geiger::tp::BOARD_ID_WORD_SIZE> temporary_board_bitset;
      for (int i = geiger::tp::BOARD_ID_BIT0; i <= geiger::tp::BOARD_ID_BIT4; i++)
	{
	  if (my_bitset_.test(i) == true)
	    {
	      temporary_board_bitset.set(i - geiger::tp::BOARD_ID_BIT0, 1);
	    }
	  else
	    {
	      temporary_board_bitset.set(i - geiger::tp::BOARD_ID_BIT0, 0);
	    }	 
	}
      uint32_t temporary_board_id = temporary_board_bitset.to_ulong();
      return temporary_board_id;
    }

    void trigger_algorithm::build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_, std::vector<geomtools::geom_id> & hit_cells_gids_) const
    {
      for (int i = 0; i < mapping::MAX_NUMBER_OF_FEB_BY_CRATE; i++)
	{
	  std::bitset<geiger::tp::FULL_SIZE> my_bitset;
	  my_geiger_ctw_.get_100_bits_in_ctw_word(i, my_bitset);
	  for (int32_t j = geiger::tp::TP_BEGIN; j <= geiger::tp::TP_THREE_WIRES_END; j++)
	    {
	      if (my_bitset.test(j))
		{
		  uint32_t ctw_type  = my_geiger_ctw_.get_geom_id().get_type();
		  uint32_t ctw_rack  = my_geiger_ctw_.get_geom_id().get(mapping::RACK_INDEX);
		  uint32_t ctw_crate = my_geiger_ctw_.get_geom_id().get(mapping::CRATE_INDEX);
		  uint32_t board_id = get_board_id(my_bitset);
		  uint32_t channel_id = j;
		  geomtools::geom_id temporary_electronic_id;
		  temporary_electronic_id.set_depth(mapping::CHANNEL_DEPTH);
		  temporary_electronic_id.set_type(ctw_type);
		  temporary_electronic_id.set(mapping::RACK_INDEX, ctw_rack);
		  temporary_electronic_id.set(mapping::CRATE_INDEX, ctw_crate);
		  temporary_electronic_id.set(mapping::BOARD_INDEX, board_id);
		  temporary_electronic_id.set(mapping::CHANNEL_INDEX, channel_id);
		  std::clog << "DEBUG : temporary EID = " << temporary_electronic_id << std::endl;

		  {
		    geomtools::geom_id dummy;
		    hit_cells_gids_.push_back(dummy);
		  }

		  geomtools::geom_id & hit_cell_gid = hit_cells_gids_.back();
		  _electronic_mapping_->convert_EID_to_GID(mapping::THREE_WIRES_TRACKER_MODE, temporary_electronic_id, hit_cell_gid);
		  std::clog << "DEBUG : hit_cell_gid  = " << hit_cell_gid << std::endl;
		}	 
	    } // end of TP loop
	} // end of max number of FEB loop
      
      return;
    }
   
    void trigger_algorithm::process(const geiger_ctw_data & geiger_ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't process ! ");
      std::vector<geomtools::geom_id> hit_cells_gids;
      for(int32_t i = geiger_ctw_data_.get_clocktick_min(); i <= geiger_ctw_data_.get_clocktick_max(); i++)
	{
	  std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick;
	  geiger_ctw_data_.get_list_of_geiger_ctw_per_clocktick(i, geiger_ctw_list_per_clocktick);
	  std::clog << "DEBUG : size of list = " << geiger_ctw_list_per_clocktick.size() << std::endl;
	  for (int j = 0; j < geiger_ctw_list_per_clocktick.size(); j++)
	    {
	      build_hit_cells_gids_from_ctw(geiger_ctw_list_per_clocktick[j].get(), hit_cells_gids);  
	    }
	}
      std::clog << "DEBUG : vector size = " << hit_cells_gids.size() << std::endl;    
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
