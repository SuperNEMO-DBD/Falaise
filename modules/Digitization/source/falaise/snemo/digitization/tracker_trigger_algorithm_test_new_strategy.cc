// snemo/digitization/tracker_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <vector>
#include <fstream>

// Boost : 
#include <boost/dynamic_bitset.hpp>

// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// Ourselves:
#include <snemo/digitization/tracker_trigger_algorithm_test_new_strategy.h>
#include <snemo/digitization/geiger_tp_constants.h>
#include <snemo/digitization/memory.h>

namespace snemo {
  
  namespace digitization {

    const int32_t tracker_trigger_algorithm_test_new_strategy::NUMBER_OF_NEAR_SOURCE_LAYERS;
    const int32_t tracker_trigger_algorithm_test_new_strategy::SUBZONE_PROJECTION_LAYER_SIZE;
    const int32_t tracker_trigger_algorithm_test_new_strategy::SUBZONE_PROJECTION_ROW_SIZE;
    const int32_t tracker_trigger_algorithm_test_new_strategy::TOTAL_ZONE_AFTER_PROJECTIONS_BITSET_SIZE;
    const int32_t tracker_trigger_algorithm_test_new_strategy::TOTAL_INTERZONE_AFTER_PROJECTIONS_BITSET_SIZE;
    const int32_t tracker_trigger_algorithm_test_new_strategy::FINAL_ZONE_BITSET_SIZE;
    const int32_t tracker_trigger_algorithm_test_new_strategy::FINAL_INTERZONE_BITSET_SIZE;    
    const int32_t tracker_trigger_algorithm_test_new_strategy::FINALE_DECISION_BITSET_SIZE;
    
    tracker_trigger_algorithm_test_new_strategy::tracker_record::tracker_record()
    {
      clocktick_1600ns = -1;
      finale_decision.reset();
      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::tracker_record::reset()
    {
      clocktick_1600ns = -1;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int jzone = 0; jzone < mapping::NUMBER_OF_TRIGGER_ZONES; jzone++)
	    {
	      final_tracker_trigger_info[iside][jzone].reset();
	    }
	}
      finale_decision.reset();
      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::tracker_record::display()
    {
      std::clog << "Tracker Trigger info record : " << std::endl; 
      std::clog << "Clocktick 1600    : " << clocktick_1600ns << std::endl;;  
      std::clog << "         | VOID = 00 | SHORT TRACK = 11 | LONG TRACK = 01" << std::endl;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	  std::clog << "[" << final_tracker_trigger_info[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	}
      std::clog << "Level one decision : [" << finale_decision << "]" <<  std::endl;

      return;
    }
    
    tracker_trigger_algorithm_test_new_strategy::geiger_matrix::geiger_matrix()
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
	    {
	      for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		{	
		  matrix[iside][jlayer][krow] = false;
		} // end of krow
	    } // end of jlayer
	} // end of iside
      clocktick_1600ns = -1;
    }
    
    bool tracker_trigger_algorithm_test_new_strategy::geiger_matrix::is_empty()
    {
      bool empty = true;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
	    {
	      for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		{	
		  if(matrix[iside][jlayer][krow] == true) empty = false;
		} // end of krow
	    } // end of jlayer
	} // end of iside
      return empty;
    }
    
    tracker_trigger_algorithm_test_new_strategy::tracker_trigger_algorithm_test_new_strategy()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      bool * vbool = static_cast<bool* > (&_geiger_matrix_[0][0][0]);
      static const size_t nmax = mapping::NUMBER_OF_SIDES * mapping::GEIGER_LAYERS_SIZE * mapping::GEIGER_ROWS_SIZE;
      for (int i = 0; i < nmax ; i ++)
      	{ 
      	  vbool[i] = false;
      	}
      _tracker_finale_decision_ = false;
      return;
    }

    tracker_trigger_algorithm_test_new_strategy::~tracker_trigger_algorithm_test_new_strategy()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::initialize()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      DT_THROW_IF(_electronic_mapping_ == 0, std::logic_error, "Missing electronic mapping ! " );

      if (config_.has_key("memory_row_file")) {
	std::string mem_def_filename = config_.fetch_string("memory_row_file");
	datatools::fetch_path_with_env(mem_def_filename);
	fill_all_row_memory(mem_def_filename);
      }
      
      if (config_.has_key("memory_layer_file")) {
	std::string mem_def_filename = config_.fetch_string("memory_layer_file");
	datatools::fetch_path_with_env(mem_def_filename);
	fill_all_layer_memory(mem_def_filename);
      }

      if (config_.has_key("memory_a4_d2_file")) {
	std::string mem_def_filename = config_.fetch_string("memory_a4_d2_file");
	datatools::fetch_path_with_env(mem_def_filename);
	fill_all_a4_d2_memory(mem_def_filename);
      }

      _initialized_ = true;
      return;
    }

    bool tracker_trigger_algorithm_test_new_strategy::is_initialized() const
    {
      return _initialized_;
    }

    void tracker_trigger_algorithm_test_new_strategy::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Tracker trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      reset_trigger_info();
      reset_matrix();
      _tracker_finale_decision_ = false;
      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::reset_trigger_info()
    {
      for (int i = 0; i < mapping::NUMBER_OF_SIDES; i++)
	{
	  for (int j = 0; j < mapping::NUMBER_OF_TRIGGER_ZONES; j++)
	    {
	      _zone_tracker_trigger_info_after_projections_[i][j].reset();
	      _zone_tracker_trigger_info_[i][j].reset();
	      if (j < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES)
		{
		  _interzone_tracker_trigger_info_after_projections_[i][j].reset();
		  _interzone_tracker_trigger_info_[i][j].reset();
		}
	    }
	}
      _tracker_record_finale_decision_.reset();
      return;
    }

    const tracker_trigger_algorithm_test_new_strategy::tracker_record tracker_trigger_algorithm_test_new_strategy::get_tracker_record_finale_decision_structure() const
    {
      return _tracker_record_finale_decision_;
    }

    uint32_t tracker_trigger_algorithm_test_new_strategy::get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const
    {
      std::bitset<geiger::tp::BOARD_ID_WORD_SIZE> temporary_board_bitset;
      for (int i = geiger::tp::BOARD_ID_BIT0; i <= geiger::tp::BOARD_ID_BIT4; i++)
	{
	  if (my_bitset_.test(i) == true)
	    {
	      temporary_board_bitset.set(i - geiger::tp::BOARD_ID_BIT0, true);
	    }
	  else
	    {
	      temporary_board_bitset.set(i - geiger::tp::BOARD_ID_BIT0, 0);
	    }	 
	}
      uint32_t temporary_board_id = temporary_board_bitset.to_ulong();
      return temporary_board_id;
    }

    void tracker_trigger_algorithm_test_new_strategy::build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_, 
								  std::vector<geomtools::geom_id> & hit_cells_gids_) const
    {
      for (int i = 0; i < mapping::NUMBER_OF_FEBS_BY_CRATE; i++)
	{
	  std::bitset<geiger::tp::FULL_SIZE> my_bitset;
	  my_geiger_ctw_.get_100_bits_in_ctw_word(i, my_bitset);

	  std::bitset<geiger::tp::TP_SIZE> my_tp_bitset;
	  my_geiger_ctw_.get_55_bits_in_ctw_word(i, my_tp_bitset);
	 	  	  
	  for (int32_t j = geiger::tp::TP_BEGIN; j <= geiger::tp::TP_THREE_WIRES_END; j++)
	    {
	      if (my_tp_bitset.test(j))
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

    const bool tracker_trigger_algorithm_test_new_strategy::get_tracker_decision() const
    {
      return _tracker_finale_decision_;
    }


    void tracker_trigger_algorithm_test_new_strategy::fill_matrix(const std::vector<geomtools::geom_id> & hit_cells_gids_)
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

    void tracker_trigger_algorithm_test_new_strategy::display_matrix() const
    {
      std::clog << "  |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl;

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
		  
		      if(!_geiger_matrix_[i][j][k])  std::clog << ".";	  

		      if( k == 112)     std::clog<<"|";

		    } // end of row loop
		  std::clog<<std::endl;	

		  if (j == 0)
		    {
		      std::clog << "  |_________________________________________________________________________________________________________________|" << std::endl;
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
		  
		      if(!_geiger_matrix_[i][j][k])  std::clog << ".";	  

		      if( k == 112)     std::clog<<"|";

		    } // end of row loop
		  std::clog<<std::endl;	    
  
		} // end of layer loop

	    } // end of if i==1

	} // end of side loop

      std::clog << "  |-0-1-2-3-4-5-6-7-8-9-1-2-3-4-5-6-7-8-9-0-1-2-3-4-5-6-7-89-1-2-3-4-5-6-7-8-9-0-1-2-3-4-5-6-7-8-9-1-2-3-4-5-6-7-8-9| Board IDs " << std::endl;

      std::clog << "  |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl;
      std::clog << "  |                                     |                                    |                                      |" << std::endl;
      std::clog << "  |---------------Crate-0---------------|--------------Crate-1---------------|---------------Crate-2----------------|" << std::endl;
      std::clog << "  |                                     |                                    |                                      |" << std::endl;
      std::clog << std::endl;

      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::reset_matrix()
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
	    {
	      for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		{
		  _geiger_matrix_[iside][jlayer][krow] = 0;
		} // end of krow
	    } // end of jlayer
	} // end of iside
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::display_intermediate_tracker_trigger_info() const
    {
      std::clog << "After projections tracker trigger info display : " << std::endl;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      std::clog << "[" << _zone_tracker_trigger_info_after_projections_[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	  std::clog << "                 ";
	  for (int iinterzone = 0; iinterzone < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES; iinterzone++)
	    {
	      std::clog << "[" << _interzone_tracker_trigger_info_after_projections_[iside][iinterzone] << "] ";
	    } // end of iinterzone
	  std::clog << std::endl;
	} // end of iside

      std::clog << "Zone and interzones tracker trigger info display : " << std::endl;
      std::clog << "         | VOID = 00 | SHORT TRACK = 11 | LONG TRACK = 01" << std::endl;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      std::clog << "[" << _zone_tracker_trigger_info_[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	  std::clog << "              ";
	  for (int iinterzone = 0; iinterzone < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES; iinterzone++)
	    {
	      std::clog << "[" << _interzone_tracker_trigger_info_[iside][iinterzone] << "] ";
	    } // end of iinterzone
	  std::clog << std::endl;
	} // end of iside
      
      return;
    }
   
    void tracker_trigger_algorithm_test_new_strategy::fill_row_memory_per_subzone(const std::string & filename_,
								int32_t side_,
								int32_t zone_,
								int32_t subzone_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _mem_projection_row_[side_][zone_][subzone_].load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_all_row_memory(const std::string & filename_)
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      for (int isubzone = 0; isubzone < mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES; isubzone++)
		{
		  fill_row_memory_per_subzone(filename_, iside, izone, isubzone);
		} // end of isubzone
	    } // end of izone
 	} // end of iside
      return;
    }
 
    void tracker_trigger_algorithm_test_new_strategy::fill_layer_memory_per_subzone(const std::string & filename_,
								  int32_t side_,
								  int32_t zone_,
								  int32_t subzone_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _mem_projection_layer_[side_][zone_][subzone_].load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_all_layer_memory(const std::string & filename_)
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      for (int isubzone = 0; isubzone < mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES; isubzone++)
		{
		  fill_layer_memory_per_subzone(filename_, iside, izone, isubzone);
		} // end of isubzone
	    } // end of izone
 	} // end of iside
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_a4_d2_memory_per_zone(const std::string & filename_,
							       int32_t side_,
							       int32_t zone_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _mem_pattern_for_a_zone_[side_][zone_].load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_all_a4_d2_memory(const std::string & filename_)
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      fill_a4_d2_memory_per_zone(filename_, iside, izone);
	    } // end of izone
 	} // end of iside
      return;
    }
   
    void tracker_trigger_algorithm_test_new_strategy::fetch_zone_limits(int32_t side_,
						      int32_t zone_index_,
						      int32_t & row_index_begin_,
						      int32_t & row_index_end_)
    {
      DT_THROW_IF((side_ > 1) || (side_ < 0), std::logic_error, "Value of side ["<< side_ <<"] is not valid ! ");
      DT_THROW_IF(zone_index_ > ZONE_9_INDEX, std::logic_error, "Value of zone index [" << zone_index_ << "] is not valid ! ");

      switch (zone_index_)
	{
	case 0 :
	  row_index_begin_ = ZONE_0_BEGIN;
	  row_index_end_   = ZONE_0_END;
	  break;
	      
	case 1:
	  row_index_begin_ = ZONE_1_BEGIN;
	  row_index_end_   = ZONE_1_END;
	  break;

	case 2 :
	  row_index_begin_ = ZONE_2_BEGIN;
	  row_index_end_   = ZONE_2_END;
	  break;
	      
	case 3:
	  row_index_begin_ = ZONE_3_BEGIN;
	  row_index_end_   = ZONE_3_END;
	  break;

	case 4 :
	  row_index_begin_ = ZONE_4_BEGIN;
	  row_index_end_   = ZONE_4_END;
	  break;
	      
	case 5:
	  row_index_begin_ = ZONE_5_BEGIN;
	  row_index_end_   = ZONE_5_END;
	  break;

	case 6 :
	  row_index_begin_ = ZONE_6_BEGIN;
	  row_index_end_   = ZONE_6_END;
	  break;
	      
	case 7:
	  row_index_begin_ = ZONE_7_BEGIN;
	  row_index_end_   = ZONE_7_END;
	  break;

	case 8 :
	  row_index_begin_ = ZONE_8_BEGIN;
	  row_index_end_   = ZONE_8_END;
	  break;
	      
	case 9:
	  row_index_begin_ = ZONE_9_BEGIN;
	  row_index_end_   = ZONE_9_END;
	  break;	      

	default :
	  break;
	}
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fetch_zone_index(int32_t side_,
						     int32_t row_index_,
						     int32_t & zone_index_)
    {
      DT_THROW_IF((side_ > 1) || (side_ < 0), std::logic_error, "Value of side ["<< side_ <<"] is not valid ! ");
      DT_THROW_IF((row_index_ >= mapping::NUMBER_OF_GEIGER_ROWS) || (row_index_ < 0), std::logic_error, "Value of row index [" << row_index_ << "] is not valid ! ");

      for (int i = 0; i < mapping::NUMBER_OF_SIDES; i++)
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

      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fetch_subzone_limits(int32_t side_,
							 int32_t subzone_index_,
							 int32_t & subzone_row_index_begin_,
							 int32_t & subzone_row_index_end_,
							 int32_t & subzone_layer_index_begin_)
    {
      int32_t zone_index = subzone_index_ / 4;
      int32_t zone_row_index_begin = -1;
      int32_t zone_row_index_end = -1;
      int32_t zone_size = -1;
	  
      fetch_zone_limits(side_, 
			zone_index, 
			zone_row_index_begin, 
			zone_row_index_end);
	  
      zone_size = zone_row_index_end - zone_row_index_begin + 1; // +1 to take in order that rows begin at index 0
	 
      switch (subzone_index_ % 4)
	{
	case 0 : 
	  subzone_row_index_begin_   = zone_row_index_begin;
	  subzone_row_index_end_     = zone_row_index_end - (zone_size / 2) ;
	  subzone_layer_index_begin_ = 0;
	  break;

	case 1 : 
	  subzone_row_index_begin_   = zone_row_index_begin;
	  subzone_row_index_end_     = zone_row_index_end - (zone_size / 2);
	  subzone_layer_index_begin_ = 4;
	  break;

	case 2 : 
	  subzone_row_index_begin_   = zone_row_index_begin + (zone_size / 2);
	  subzone_row_index_end_     = zone_row_index_end;
	  subzone_layer_index_begin_ = 0;
	  break;

	case 3 : 
	  subzone_row_index_begin_   = zone_row_index_begin + (zone_size / 2);
	  subzone_row_index_end_     = zone_row_index_end;
	  subzone_layer_index_begin_ = 4;
	  break;

	default :
	  break;
	}

      return;
    }
      
    void tracker_trigger_algorithm_test_new_strategy::build_zone_tracker_trigger_after_projections()
    {
      for (int32_t iside = 0; iside < mapping::NUMBER_OF_SIDES; iside ++)
	{
	  for (int i = 0; i < mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES_PER_SIDE; i ++)
	    {
	      const int32_t zone_index  = i / 4;
	      const int32_t subzone     = i % 4;
	  
	      fetch_subzone_limits(iside,
				   i,
				   _sub_zone_location_info_[iside][i].row_begin,
				   _sub_zone_location_info_[iside][i].row_end,
				   _sub_zone_location_info_[iside][i].layer_begin);
	  
	      const int32_t row_begin   = _sub_zone_location_info_[iside][i].row_begin;
	      const int32_t row_end     = _sub_zone_location_info_[iside][i].row_end;
	      const int32_t layer_begin = _sub_zone_location_info_[iside][i].layer_begin;
	      const int32_t layer_end   = _sub_zone_location_info_[iside][i].layer_begin + SUBZONE_PROJECTION_LAYER_SIZE;

	      std::bitset<SUBZONE_PROJECTION_ROW_SIZE>   subzone_row_bitset;
	      std::bitset<SUBZONE_PROJECTION_LAYER_SIZE> subzone_layer_bitset;

	      for (int jrow = row_begin; jrow <= row_end; jrow++)
		{
		  for (int klayer = layer_begin; klayer < layer_end; klayer++)
		    {
		      if (_geiger_matrix_[iside][klayer][jrow])
			{ 
			  subzone_row_bitset.set(jrow - row_begin, true);
			} // end of if
		    } // end of klayer
		} // end of jrow
	  
	      for (int klayer = layer_begin; klayer < layer_end; klayer++)
		{
		  for (int jrow = row_begin; jrow <= row_end; jrow++)
		    {
		      if (_geiger_matrix_[iside][klayer][jrow])
			{
			  subzone_layer_bitset.set(klayer - layer_begin, true);
			} // end of if
		    } // end of jrow
		} // end of klayer

	      std::bitset<SUBZONE_PROJECTION_ROW_SIZE> subzone_row_bitset_address = subzone_row_bitset;
	      std::bitset<1> subzone_row_bitset_data;
	      _mem_projection_row_[iside][zone_index][subzone].fetch(subzone_row_bitset_address, subzone_row_bitset_data);
	      
	      std::bitset<SUBZONE_PROJECTION_LAYER_SIZE> subzone_layer_bitset_address = subzone_layer_bitset;
	      std::bitset<1> subzone_layer_bitset_data;
	      _mem_projection_layer_[iside][zone_index][subzone].fetch(subzone_layer_bitset_address, subzone_layer_bitset_data);
	      
	      int32_t layer_bit_index = -1;
	      int32_t row_bit_index = -1;

	      switch (subzone)
		{
		case SUBZONE_0_INDEX :
		  layer_bit_index = SUBZONE_0_LAYER_INDEX;
		  row_bit_index = SUBZONE_0_ROW_INDEX;
		  break;
		case SUBZONE_1_INDEX :
		  layer_bit_index = SUBZONE_1_LAYER_INDEX;
		  row_bit_index = SUBZONE_1_ROW_INDEX;
		  break;
		case SUBZONE_2_INDEX :
		  layer_bit_index = SUBZONE_2_LAYER_INDEX;
		  row_bit_index = SUBZONE_2_ROW_INDEX;
		  break;
		case SUBZONE_3_INDEX :
		  layer_bit_index = SUBZONE_3_LAYER_INDEX;
		  row_bit_index = SUBZONE_3_ROW_INDEX;
		  break;
		default :
		  break;
		};
	      
	      if (subzone == SUBZONE_0_INDEX)
		{
		  std::bitset<NUMBER_OF_NEAR_SOURCE_LAYERS> near_source_layer_bitset;
		  for (int i = 0; i < NUMBER_OF_NEAR_SOURCE_LAYERS; i++)
		    {
		      if (subzone_layer_bitset_address.test(i) == true) near_source_layer_bitset.set(i, true);
		      else near_source_layer_bitset.set(i, false);
		    }
		  if (near_source_layer_bitset.any()) _zone_tracker_trigger_info_after_projections_[iside][zone_index].set(LEFT_NEAR_SOURCE_AP_INDEX, true);
		}
     
	      if (subzone == SUBZONE_2_INDEX)
		{
		  std::bitset<NUMBER_OF_NEAR_SOURCE_LAYERS> near_source_layer_bitset;
		  for (int i = 0; i < NUMBER_OF_NEAR_SOURCE_LAYERS; i++)
		    {
		      if (subzone_layer_bitset_address.test(i) == true) near_source_layer_bitset.set(i, true);
		      else near_source_layer_bitset.set(i, false);
		    }
		  if (near_source_layer_bitset.any()) _zone_tracker_trigger_info_after_projections_[iside][zone_index].set(RIGHT_NEAR_SOURCE_AP_INDEX, true);
		}


	      if (subzone_layer_bitset_data.test(0) == true)
		{
		  _zone_tracker_trigger_info_after_projections_[iside][zone_index].set(layer_bit_index, true);
		}
	      if (subzone_row_bitset_data.test(0) == true)
		{
		  _zone_tracker_trigger_info_after_projections_[iside][zone_index].set(row_bit_index, true);
		}

	    } // end of isubzones
	} // end of isides
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::build_zone_tracker_trigger_info()
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{ 
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES; izone++)
	    {
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone].test(SUBZONE_2_LAYER_INDEX) == true)  
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_0_LAYER_INDEX, true);
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone].test(SUBZONE_2_ROW_INDEX)   == true) 
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_0_ROW_INDEX, true);
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone].test(SUBZONE_3_LAYER_INDEX) == true)  
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_1_LAYER_INDEX, true);
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone].test(SUBZONE_3_ROW_INDEX)   == true)  
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_1_ROW_INDEX, true);

	      if (_zone_tracker_trigger_info_after_projections_[iside][izone+1].test(SUBZONE_0_LAYER_INDEX) == true) 
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_2_LAYER_INDEX, true);
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone+1].test(SUBZONE_0_ROW_INDEX)   == true)
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_2_ROW_INDEX, true);
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone+1].test(SUBZONE_1_LAYER_INDEX) == true) 
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_3_LAYER_INDEX, true);
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone+1].test(SUBZONE_1_ROW_INDEX)   == true) 
		_interzone_tracker_trigger_info_after_projections_[iside][izone].set(SUBZONE_3_ROW_INDEX, true);
	    } // end of izone
	} // end of iside
			
      // Intermediate bitset size for a zone from 8 bits after projections and near source bits
      const int32_t INTERMEDIATE_ZONE_PATTERN_BITSET_SIZE = 4;
      // Intermediate bitset size for an interzone from 8 bits after projections only
      const int32_t INTERMEDIATE_PATTERN_DATA_BITSET_SIZE = 2;

      std::bitset<INTERMEDIATE_ZONE_PATTERN_BITSET_SIZE> intermediate_zone_pattern_tracker_trigger_info[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES];
      std::bitset<INTERMEDIATE_ZONE_PATTERN_BITSET_SIZE> intermediate_interzone_pattern_tracker_trigger_info[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES];

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      for (int jindex = 0; jindex <=  SUBZONE_3_ROW_INDEX; jindex += 2)
		{
		  
		  // Filling intermediate level one zone bitset
		  if ( _zone_tracker_trigger_info_after_projections_[iside][izone].test(jindex) == true || _zone_tracker_trigger_info_after_projections_[iside][izone].test(jindex+1) == true)
		    {
		      intermediate_zone_pattern_tracker_trigger_info[iside][izone].set(jindex/2, true);
		    }

		  // Filling intermediate level one interzone bitset
		  if (izone < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES)
		    {
		      if (_interzone_tracker_trigger_info_after_projections_[iside][izone].test(jindex) == true || _interzone_tracker_trigger_info_after_projections_[iside][izone].test(jindex) == true)
			{
			  intermediate_interzone_pattern_tracker_trigger_info[iside][izone].set(jindex/2, true);
			}
		    } // end of if
		} // end of jindex

	      // Filling level two zone bitset
	      std::bitset<INTERMEDIATE_ZONE_PATTERN_BITSET_SIZE> intermediate_zone_bitset_address = intermediate_zone_pattern_tracker_trigger_info[iside][izone];
	      std::bitset<INTERMEDIATE_PATTERN_DATA_BITSET_SIZE> zone_bitset_data;
	      _mem_pattern_for_a_zone_[iside][izone].fetch(intermediate_zone_bitset_address, zone_bitset_data);
	      if (zone_bitset_data.test(0) == true) // Test index 0 of the bitset
		{
		  _zone_tracker_trigger_info_[iside][izone].set(PATTERN_BIT_0_FINAL_INDEX, true); // Set index 0 of the bitset
		}

	      if (zone_bitset_data.test(1) == true) // Test index 1 of the bitset
		{
		  _zone_tracker_trigger_info_[iside][izone].set(PATTERN_BIT_1_FINAL_INDEX, true); // Set index 1 of the bitset
		}

	      // Filling level two interzone bitset
	      if (izone < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES)
		{
		  std::bitset<INTERMEDIATE_ZONE_PATTERN_BITSET_SIZE> intermediate_interzone_bitset_address = intermediate_interzone_pattern_tracker_trigger_info[iside][izone];
		  std::bitset<INTERMEDIATE_PATTERN_DATA_BITSET_SIZE> interzone_bitset_data;
		  _mem_pattern_for_a_zone_[iside][izone].fetch(intermediate_interzone_bitset_address, interzone_bitset_data);
		  if (interzone_bitset_data.test(0) == true) // Test index 0 of the bitset
		    {
		      _interzone_tracker_trigger_info_[iside][izone].set(PATTERN_BIT_0_FINAL_INDEX, true); // Set index 0 of the bitset
		    }
		  if (interzone_bitset_data.test(1) == true) // Test index 1 of the bitset
		    {
		      _interzone_tracker_trigger_info_[iside][izone].set(PATTERN_BIT_1_FINAL_INDEX, true); // Set index 1 of the bitset
		    }
		} //end of if
	      
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone].test(RIGHT_NEAR_SOURCE_AP_INDEX) == true) _zone_tracker_trigger_info_[iside][izone].set(RIGHT_NEAR_SOURCE_FINAL_INDEX,true);
	      
	      if (_zone_tracker_trigger_info_after_projections_[iside][izone].test(LEFT_NEAR_SOURCE_AP_INDEX) == true) _zone_tracker_trigger_info_[iside][izone].set(LEFT_NEAR_SOURCE_FINAL_INDEX,true);
	      
	    } // end of izone
	} // end of iside

      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::build_tracker_record()
    {
      // Merging level two and level two prime :
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone] = _zone_tracker_trigger_info_[iside][izone];
	    }
	}
     
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      // Test if there is a full track (01) in the interzone and set zoning bitset if there is
	      if (_interzone_tracker_trigger_info_[iside][izone].test(0)    == true     // test bit index 0 
	      	  && _interzone_tracker_trigger_info_[iside][izone].test(1) == false    // test bit index 1
		  && izone < mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES) 
	      	{	
		  _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone].set(0,1);
		  _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone].set(1,0);
		  _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone+1].set(0,1);
		  _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone+1].set(1,0);
	      	}
	    } // end of izone	  
	} // end of iside

      // Compute boolean decision :
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      if (_tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone].test(0) == true
		  &&_tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone].test(1) == false)
		{
		  _tracker_record_finale_decision_.finale_decision.set(0, true);
		  _tracker_record_finale_decision_.finale_decision.set(1, false);
		}
	      
	      else if (_tracker_record_finale_decision_.finale_decision != 1
		       && _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone].test(0) == true
		       && _tracker_record_finale_decision_.final_tracker_trigger_info[iside][izone].test(1) == true)
		{
		  _tracker_record_finale_decision_.finale_decision.set(0, true);
		  _tracker_record_finale_decision_.finale_decision.set(1, true);
		}
	     
	    } // end of izone	  
	} // end of iside

      
      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::_process_for_a_clocktick(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_)
    {
      reset_matrix();
      reset_trigger_info();
      for (int isize = 0; isize < geiger_ctw_list_per_clocktick_.size(); isize++)
	{
	  std::vector<geomtools::geom_id> hit_cells_gids;
	  build_hit_cells_gids_from_ctw(geiger_ctw_list_per_clocktick_[isize].get(), hit_cells_gids);
	  fill_matrix(hit_cells_gids);   
	} // end of isize
      geiger_matrix a_geiger_matrix;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
	    {
	      for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		{
		  a_geiger_matrix.matrix[iside][jlayer][krow] = _geiger_matrix_[iside][jlayer][krow];	
		} // end of krow
	    } // end of jlayer
	} // end of iside
      a_geiger_matrix.clocktick_1600ns = geiger_ctw_list_per_clocktick_[0].get().get_clocktick_800ns() / 2;
      _geiger_matrix_records_.push_back(a_geiger_matrix);
      _tracker_record_finale_decision_.clocktick_1600ns = geiger_ctw_list_per_clocktick_[0].get().get_clocktick_800ns() / 2;
      build_zone_tracker_trigger_after_projections();
      build_zone_tracker_trigger_info();
      build_tracker_record();	
      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::_process(const geiger_ctw_data & geiger_ctw_data_,
					     std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_)
    { 
      _geiger_matrix_records_.clear();
      // Just even clockticks are processing (to take in account CB to TB serdes limitation
      int32_t iclocktick_800 = geiger_ctw_data_.get_clocktick_min();
      if (iclocktick_800 % 2 == 1) iclocktick_800 += 1;
      for(iclocktick_800; iclocktick_800 <= geiger_ctw_data_.get_clocktick_max(); iclocktick_800 +=2)
	{
	  std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick;
	  geiger_ctw_data_.get_list_of_geiger_ctw_per_clocktick(iclocktick_800, geiger_ctw_list_per_clocktick);
	  if (geiger_ctw_list_per_clocktick.size() != 0)
	    {
	      _process_for_a_clocktick(geiger_ctw_list_per_clocktick);
	      if (_tracker_record_finale_decision_.finale_decision != 0) _tracker_finale_decision_ = true;
	      
	      tracker_records_.push_back(_tracker_record_finale_decision_);
	    } // end of if ctw list != 0
	} // end of iclocktick
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::process(const geiger_ctw_data & geiger_ctw_data_,
							      std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Tracker trigger algorithm is not initialized, it can't process ! ");
      _process(geiger_ctw_data_, tracker_records_);
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
