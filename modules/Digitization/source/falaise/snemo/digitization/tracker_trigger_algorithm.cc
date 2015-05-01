// snemo/digitization/tracker_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <vector>

// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// Ourselves:
#include <snemo/digitization/tracker_trigger_algorithm.h>
#include <snemo/digitization/geiger_tp_constants.h>

namespace snemo {
  
  namespace digitization {

    const int32_t tracker_trigger_algorithm::ZONING_BITSET_SIZE;

    tracker_trigger_algorithm::tracker_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      bool * vbool = static_cast<bool* > (&_geiger_matrix_[0][0][0]);
      static const size_t nmax = mapping::NUMBER_OF_SIDES * mapping::GEIGER_LAYERS_SIZE * mapping::GEIGER_ROWS_SIZE;

      for (int i = 0; i < nmax ; i ++)
	{
	  vbool[i] = false;
	}

      return;
    }

    tracker_trigger_algorithm::~tracker_trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void tracker_trigger_algorithm::initialize(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      _initialized_ = true;
      return;
    }

    bool tracker_trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }
    void tracker_trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      return;
    }

    uint32_t tracker_trigger_algorithm::get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const
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

    void tracker_trigger_algorithm::build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_, std::vector<geomtools::geom_id> & hit_cells_gids_) const
    {
      for (int i = 0; i < mapping::NUMBER_OF_FEBS_BY_CRATE; i++)
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
		  {
		    geomtools::geom_id dummy;
		    hit_cells_gids_.push_back(dummy);
		  }
		  geomtools::geom_id & hit_cell_gid = hit_cells_gids_.back();
		  _electronic_mapping_->convert_EID_to_GID(mapping::THREE_WIRES_TRACKER_MODE, temporary_electronic_id, hit_cell_gid);
		}	 
	    } // end of TP loop

	} // end of max number of FEB loop

      return;
    }

    void tracker_trigger_algorithm::fill_matrix(const std::vector<geomtools::geom_id> & hit_cells_gids_)
    {
      for (int i = 0; i < hit_cells_gids_.size(); i++)
	{
	  int side  = hit_cells_gids_[i].get(mapping::SIDE_INDEX);
	  int layer = hit_cells_gids_[i].get(mapping::LAYER_INDEX);
	  int row   = hit_cells_gids_[i].get(mapping::ROW_INDEX);
	  _geiger_matrix_[side][layer][row] = 1;
	}    
      return;
    }

    void tracker_trigger_algorithm::display_matrix() const
    { 
      std::clog << "  |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-| Board IDs " << std::endl;

      for (int i = 0; i < mapping::NUMBER_OF_SIDES; i++)
	{
	  if (i == 0)
	    {
	      for (int j = mapping::GEIGER_LAYERS_SIZE - 1; j >= 0; j--) // Value GEIGER_LAYER_SIZE = 9
		{
		  std::clog << j << ' ';
		  for (int k = 0; k < mapping::GEIGER_ROWS_SIZE; k++)
		    {
		      if( k == 0 )        std::clog<<"|";
		  
		      if (_geiger_matrix_[i][j][k] ) std::clog << "*";
		  
		      if(!_geiger_matrix_[i][j][k])  std::clog << "o";	  

		      if( k == 112)     std::clog<<"|";

		    } // end of row loop
		  std::clog<<std::endl;	

		  if (j == 0)
		    {
		      std::clog << "  |-----------------------------------------------------------------------------------------------------------------|" << std::endl;;
		    }

		} // end of layer loop

	    } // end of if == 0

	  if (i == 1)
	    {  
	      for (int j = 0; j < mapping::GEIGER_LAYERS_SIZE; j++)
		{
		  std::clog << j << ' ' ;
		  for (int k = 0; k < mapping::GEIGER_ROWS_SIZE; k++)
		    {
		      if( k == 0 )        std::clog<<"|";
		  
		      if (_geiger_matrix_[i][j][k] ) std::clog << "*";
		  
		      if(!_geiger_matrix_[i][j][k])  std::clog << "o";	  

		      if( k == 112)     std::clog<<"|";

		    } // end of row loop
		  std::clog<<std::endl;	    
  
		} // end of layer loop

	    } // end of if i==1

	} // end of side loop

      std::clog << "  |-0-1-2-3-4-5-6-7-8-9-1-2-3-4-5-6-7-8-9-0-1-2-3-4-5-6-7-89-1-2-3-4-5-6-7-8-9-0-1-2-3-4-5-6-7-8-9-1-2-3-4-5-6-7-8-9| Board IDs " << std::endl;
      std::clog << "  |                                     |                                    |                                      |" << std::endl;
      std::clog << "  |---------------Crate-0---------------|--------------Crate-1---------------|---------------Crate-2----------------|" << std::endl;
      std::clog << "  |                                     |                                    |                                      |" << std::endl;

      return;
    }
    
    void tracker_trigger_algorithm::reset_matrix()
    {
      for (int i = 0; i < mapping::NUMBER_OF_SIDES; i ++)
	{
	  for (int j = 0; j < mapping::GEIGER_LAYERS_SIZE; j ++)
	    {
	      for (int k = 0; k < mapping::GEIGER_ROWS_SIZE; k ++)
		{
		  _geiger_matrix_[i][j][k] = 0;
		}
	    }
	}
      return;
    } 
    
    void tracker_trigger_algorithm::fetch_zone_limits(int32_t side_, int32_t zone_index_, int32_t & row_index_begin_, int32_t & row_index_end_)
    {
      DT_THROW_IF((side_ > 1) || (side_ < 0), std::logic_error, "Value of side ["<< side_ <<"] is not valid ! ");
      DT_THROW_IF(zone_index_ > ZONE_9_INDEX, std::logic_error, "Value of zone index [" << zone_index_ << "] is not valid ! ");

      if (side_ == 0)
	{
	  switch (zone_index_)
	    {
	    case 0 :
	      row_index_begin_ = ZONE_0_BEGIN;
	      row_index_end_ = ZONE_0_END;
	      break;
	      
	    case 1:
	      row_index_begin_ = ZONE_1_BEGIN;
	      row_index_end_ = ZONE_1_END;
	      break;

	    case 2 :
	      row_index_begin_ = ZONE_2_BEGIN;
	      row_index_end_ = ZONE_2_END;
	      break;
	      
	    case 3:
	      row_index_begin_ = ZONE_3_BEGIN;
	      row_index_end_ = ZONE_3_END;
	      break;

	    case 4 :
	      row_index_begin_ = ZONE_4_BEGIN;
	      row_index_end_ = ZONE_4_END;
	      break;
	      
	    case 5:
	      row_index_begin_ = ZONE_5_BEGIN;
	      row_index_end_ = ZONE_5_END;
	      break;

	    case 6 :
	      row_index_begin_ = ZONE_6_BEGIN;
	      row_index_end_ = ZONE_6_END;
	      break;
	      
	    case 7:
	      row_index_begin_ = ZONE_7_BEGIN;
	      row_index_end_ = ZONE_7_END;
	      break;

	    case 8 :
	      row_index_begin_ = ZONE_8_BEGIN;
	      row_index_end_ = ZONE_8_END;
	      break;
	      
	    case 9:
	      row_index_begin_ = ZONE_9_BEGIN;
	      row_index_end_ = ZONE_9_END;
	      break;	      

	    default :
	      break;
	    }
	}
      return;
    }

    void tracker_trigger_algorithm::fetch_zone_index(int32_t side_, int32_t row_index_, int32_t & zone_index_)
    {
      DT_THROW_IF((side_ > 1) || (side_ < 0), std::logic_error, "Value of side ["<< side_ <<"] is not valid ! ");
      DT_THROW_IF((row_index_ >= mapping::NUMBER_OF_GEIGER_ROWS) || (row_index_ < 0), std::logic_error, "Value of row index [" << row_index_ << "] is not valid ! ");
      for (int i = 0; i < mapping::NUMBER_OF_SIDES; i++)
	{
	  if (i == 0)
	    {
	      if (row_index_ >= 0 && row_index_ <= ZONE_0_END) zone_index_ = ZONE_0_INDEX;
	      else if (row_index_ >= ZONE_1_BEGIN && row_index_ <= ZONE_1_END) zone_index_ = ZONE_1_INDEX;
	      else if (row_index_ >= ZONE_2_BEGIN && row_index_ <= ZONE_2_END) zone_index_ = ZONE_2_INDEX;
	      else if (row_index_ >= ZONE_3_BEGIN && row_index_ <= ZONE_3_END) zone_index_ = ZONE_3_INDEX;
	      else if (row_index_ >= ZONE_4_BEGIN && row_index_ <= ZONE_4_END) zone_index_ = ZONE_4_INDEX;
	      else if (row_index_ >= ZONE_5_BEGIN && row_index_ <= ZONE_5_END) zone_index_ = ZONE_5_INDEX;
	      else if (row_index_ >= ZONE_6_BEGIN && row_index_ <= ZONE_6_END) zone_index_ = ZONE_6_INDEX;
	      else if (row_index_ >= ZONE_7_BEGIN && row_index_ <= ZONE_7_END) zone_index_ = ZONE_7_INDEX;
	      else if (row_index_ >= ZONE_8_BEGIN && row_index_ <= ZONE_8_END) zone_index_ = ZONE_8_INDEX;
	      else if (row_index_ >= ZONE_9_BEGIN && row_index_ <= ZONE_9_END) zone_index_ = ZONE_9_INDEX;
	    }
	  else 
	    {
	    }
	}

      return;
    }
    
    void tracker_trigger_algorithm::build_trigger_level_one_bitsets()
    {
      int32_t side = 0;
      if (side == 0)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRACKER_TRIGGER_ZONES; izone++)
	    {  
	      int32_t zone_row_index_begin = -1;
	      int32_t zone_row_index_end   = -1;
	      fetch_zone_limits(side, izone, zone_row_index_begin, zone_row_index_end);
	      const int32_t zone_size = zone_row_index_end - zone_row_index_begin + 1; // +1 to take in account the shift of the row 0

	      const int32_t LAYER_BITSET_SIZE = 5;
	      const int32_t ROW_BITSET_SIZE   = 6;
	      const int32_t ROW_BITSET_SIDE_SIZE = 5;

	      std::bitset<LAYER_BITSET_SIZE> layer_bitset_sub_zone_0 = 0x0;
	      std::bitset<LAYER_BITSET_SIZE> layer_bitset_sub_zone_1 = 0x0;
	      std::bitset<LAYER_BITSET_SIZE> layer_bitset_sub_zone_2 = 0x0;
	      std::bitset<LAYER_BITSET_SIZE> layer_bitset_sub_zone_3 = 0x0;
	      // if (izone == 0 || izone == 9)
	      // 	{
	      std::bitset<ROW_BITSET_SIDE_SIZE> row_bitset_sub_zone_0 = 0x0;
	      std::bitset<ROW_BITSET_SIDE_SIZE> row_bitset_sub_zone_1 = 0x0;
	      std::bitset<ROW_BITSET_SIDE_SIZE> row_bitset_sub_zone_2 = 0x0;
	      std::bitset<ROW_BITSET_SIDE_SIZE> row_bitset_sub_zone_3 = 0x0;
	      // 	}
	      // if (izone == 5)
	      // 	{
		  
	      // 	}

	      // else 
	      // 	{
	      // std::bitset<ROW_BITSET_SIZE> row_bitset_sub_zone_0 = 0x0;
	      // std::bitset<ROW_BITSET_SIZE> row_bitset_sub_zone_1 = 0x0;
	      // std::bitset<ROW_BITSET_SIZE> row_bitset_sub_zone_2 = 0x0;
	      // std::bitset<ROW_BITSET_SIZE> row_bitset_sub_zone_3 = 0x0;
	      // 	}
	      
	      // for (int jrow = zone_row_index_begin; jrow <= zone_row_index_end; jrow++)
	      // 	{
	      // 	  for (int klayer = 0; klayer < mapping::NUMBER_OF_LAYERS; klayer++)
	      // 	    {
	      // 	      if (_geiger_matrix_[side][klayer][jrow])
	      // 		{
	      // 		  int32_t row_sub_zone = -1;
	      // 		  if (zone_size == 12 || zone_size == 11)
	      // 		    {
	      // 		      row_sub_zone = jrow - zone_row_index_begin;
	      // 		      std::clog << "DEBUG : klayer = " << klayer << " row sub zone = " << row_sub_zone << std::endl;
	      // 		    }
	      // 		  int32_t layer_bit_index = klayer + 1; // +1 to take in account layer 0
	      // 		  int32_t row_sub_zone   = row_sub_zone + 1; // +1 to take in account row 0
	      // 		  std::clog << "DEBUG : layer bit index = " << layer_bit_index << " row bit index = " << row_sub_zone <<std::endl;

	      // 		  if (klayer >= 0 && klayer <= 3 && row_sub_zone >= 0 && row_sub_zone <= 5)
	      // 		    {
	      // 		      std::clog << "DEBUG : Subzone 0 " << std::endl;
	      // 		      // Subzone 0
	      // 		      row_bitset_sub_zone_0.set(row_sub_zone, 1);
	      // 		      layer_bitset_sub_zone_0.set(layer_bit_index, 1);
	      // 		    }
	      // 		  if (klayer >= 5 && klayer <= 8 && row_sub_zone >= 0 && row_sub_zone <= 5 )
	      // 		    {
	      // 		      std::clog << "DEBUG : Subzone 1 " << std::endl;
	      // 		      // Subzone 1
	      // 		      row_bitset_sub_zone_1.set(row_sub_zone, 1);
	      // 		      layer_bitset_sub_zone_1.set(layer_bit_index - LAYER_BITSET_SIZE, 1);
	      // 		    }
	      // 		  if (klayer >= 0 && klayer <= 3 && row_sub_zone >= 6 && row_sub_zone <= 11  )
	      // 		    {
	      // 		      std::clog << "DEBUG : Subzone 2 " << std::endl;
	      // 		      // Subzone 2
	      // 		      row_bitset_sub_zone_2.set(row_sub_zone - ROW_BITSET_SIZE, 1);
	      // 		      layer_bitset_sub_zone_2.set(layer_bit_index, 1);
	      // 		    }
	      // 		  if (klayer >= 5 && klayer <= 8 && row_sub_zone >= 6 && row_sub_zone <= 11)
	      // 		    {
	      // 		      std::clog << "DEBUG : Subzone 3 " << std::endl;
	      // 		      // Subzone 3
	      // 		      row_bitset_sub_zone_3.set(row_sub_zone - ROW_BITSET_SIZE , 1);
	      // 		      layer_bitset_sub_zone_3.set(layer_bit_index - LAYER_BITSET_SIZE, 1);
	      // 		    } 
	      // 		  if (klayer == 4 && row_sub_zone >= 0 && row_sub_zone <= 5 )
	      // 		    {
	      // 		      std::clog << "DEBUG : Subzone 0 and Subzone 1 " << std::endl;
	      // 		      // Subzone 0 & 1
	      // 		      row_bitset_sub_zone_0.set(row_sub_zone, 1);
	      // 		      std::clog << "DEBUG 1" << std::endl;
	      // 		      layer_bitset_sub_zone_0.set(layer_bit_index, 1);
	      // 		      std::clog << "DEBUG 2" << std::endl;
	      // 		      row_bitset_sub_zone_1.set(row_sub_zone, 1);
	      // 		      std::clog << "DEBUG 3" << std::endl;
	      // 		      layer_bitset_sub_zone_1.set(layer_bit_index - LAYER_BITSET_SIZE, 1);
	      // 		      std::clog << "DEBUG 4" << std::endl;
	      // 		    }
	      // 		  if (klayer == 4 && row_sub_zone >= 6 && row_sub_zone <= 11)
	      // 		    {
	      // 		      std::clog << "DEBUG : Subzone 2 and Subzone 3 " << std::endl;
	      // 		      // Subzone 2 & 3
	      // 		      row_bitset_sub_zone_2.set(row_sub_zone - ROW_BITSET_SIZE, 1);
	      // 		      layer_bitset_sub_zone_2.set(layer_bit_index, 1);
	      // 		      row_bitset_sub_zone_3.set(row_sub_zone - ROW_BITSET_SIZE , 1);
	      // 		      layer_bitset_sub_zone_3.set(layer_bit_index - LAYER_BITSET_SIZE, 1);
	      // 		    }
	      // 		}
	      // 	      else
	      // 		{
	      // 		}
	      // 	    }
	      // 	}
	      // std::clog << "DEBUG : Zone index = " << izone
	      // 		<< " Row sub zone 0 bitset = " << row_bitset_sub_zone_0
	      // 		<< " Layer sub zone 0 bitset = " << layer_bitset_sub_zone_0
	      // 		<< " Row sub zone 1 bitset = " << row_bitset_sub_zone_1
	      // 		<< " Layer sub zone 1 bitset = " << layer_bitset_sub_zone_1
	      // 		<< " Row sub zone 2 bitset = " << row_bitset_sub_zone_2
	      // 		<< " Layer sub zone 2 bitset = " << layer_bitset_sub_zone_2
	      // 		<< " Row sub zone 3 bitset = " << row_bitset_sub_zone_3
	      // 		<< " Layer sub zone 3 bitset = " << layer_bitset_sub_zone_3
	      // 		<< std::endl << std::endl;
	    } // end of izone
	} // end of if

      return;
    }
    
    void tracker_trigger_algorithm::process(const geiger_ctw_data & geiger_ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't process ! ");
      for(int32_t i = geiger_ctw_data_.get_clocktick_min(); i <= geiger_ctw_data_.get_clocktick_max(); i++)
	{
	  std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick;
	  geiger_ctw_data_.get_list_of_geiger_ctw_per_clocktick(i, geiger_ctw_list_per_clocktick);
	  for (int j = 0; j < geiger_ctw_list_per_clocktick.size(); j++)
	    {
	      std::vector<geomtools::geom_id> hit_cells_gids;
	      build_hit_cells_gids_from_ctw(geiger_ctw_list_per_clocktick[j].get(), hit_cells_gids);
	      fill_matrix(hit_cells_gids);   
	    }
	  
	  build_trigger_level_one_bitsets();

	  std::clog << std::endl;
	  display_matrix();
	  reset_matrix();
	} 
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
