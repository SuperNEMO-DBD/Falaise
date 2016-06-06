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
#include <snemo/digitization/tracker_trigger_algorithm.h>
#include <snemo/digitization/geiger_tp_constants.h>
#include <snemo/digitization/memory.h>

namespace snemo {
  
  namespace digitization {

    tracker_trigger_algorithm::tracker_record::tracker_record()
    {
      tracker_record::reset();
      return;
    }

    void tracker_trigger_algorithm::tracker_record::reset()
    {
      clocktick_1600ns = -1;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      finale_data_per_zone[iside][izone].reset();
	    }
	  zoning_word_pattern[iside].reset();
	  zoning_word_near_source[iside].reset();
	}
      single_side_coinc = false;
      finale_decision = false;
      return;
    }

    void tracker_trigger_algorithm::tracker_record::display()
    {
      std::clog << "Tracker Trigger info record : " << std::endl; 
      std::clog << "Clocktick 1600    : " << clocktick_1600ns << std::endl;;  

      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      std::clog << "[" << finale_data_per_zone[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	}
      
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::clog << "ZW pattern     : S" << iside << " : [";
	  for (unsigned int ibit = 0; ibit < zoning_word_pattern[0].size(); ibit++)
	    {
	      std::clog << zoning_word_pattern[iside][ibit];
	    }
	  std::clog << "] ";
	}     
      std::clog << std::endl;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::clog << "ZW near source : S" << iside << " : [";
	  for (unsigned int ibit = 0; ibit < zoning_word_near_source[0].size(); ibit++)
	    {
	      std::clog << zoning_word_near_source[iside][ibit];
	    }
	  std::clog << "] ";
	}
      std::clog << std::endl;
      std::clog << "Level one tracker decision : [" << finale_decision << "]" <<  std::endl << std::endl;
      return;
    }

    tracker_trigger_algorithm::geiger_matrix::geiger_matrix()
    {
      for (unsigned int iside = 0; iside <trigger_info::NSIDES; iside++)
	{
	  for (unsigned int jlayer = 0; jlayer < trigger_info::NLAYERS; jlayer++)
	    {
	      for (unsigned int krow = 0; krow < trigger_info::NROWS; krow++)
		{	
		  matrix[iside][jlayer][krow] = false;
		} // end of krow
	    } // end of jlayer
	} // end of iside
      clocktick_1600ns = -1;
    }

    bool tracker_trigger_algorithm::geiger_matrix::is_empty()
    {
      bool empty = true;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  for (unsigned int jlayer = 0; jlayer < trigger_info::NLAYERS; jlayer++)
	    {
	      for (unsigned int krow = 0; krow < trigger_info::NROWS; krow++)
		{	
		  if(matrix[iside][jlayer][krow] == true) empty = false;
		} // end of krow
	    } // end of jlayer
	} // end of iside
      return empty;
    }

    void tracker_trigger_algorithm::geiger_matrix::display_matrix_garrido() const
    {
      for (unsigned int i = 0; i < trigger_info::NSIDES; i++)
	{
	  if (i == 0)
	    {
	      for (unsigned int j = trigger_info::NLAYERS - 1; j > 0; j--) // Value GEIGER_LAYER_SIZE = 9
		{
		  //std::clog << ' ';
		  for (unsigned int k = 0; k < trigger_info::NROWS; k++)
		    {
		  
		      if (matrix[i][j][k] ) std::clog << "1";
		  
		      if(!matrix[i][j][k])  std::clog << "0";	  

		    } // end of row loop
		  std::clog<<std::endl;	

		} // end of layer loop

	    } // end of if == 0

	  if (i == 1)
	    {  
	      for (unsigned int j = 0; j < trigger_info::NLAYERS; j++)
		{
		  //std::clog << ' ' ;
		  for (unsigned int k = 0; k < trigger_info::NROWS; k++)
		    {
		  
		      if (matrix[i][j][k] ) std::clog << "1";
		  
		      if(!matrix[i][j][k])  std::clog << "0";	  


		    } // end of row loop
		  std::clog<<std::endl;	    
  
		} // end of layer loop

	    } // end of if i==1

	} // end of side loop
      std::clog << std::endl;

      return;
    }

    tracker_trigger_algorithm::tracker_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      bool * vbool = static_cast<bool* > (&_geiger_matrix_[0][0][0]);
      static const size_t nmax = trigger_info::NSIDES * trigger_info::NLAYERS * trigger_info::NROWS;
      for (unsigned int i = 0; i < nmax ; i ++)
      	{ 
      	  vbool[i] = false;
      	}
      _tracker_finale_decision_ = false;
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

    void tracker_trigger_algorithm::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    void tracker_trigger_algorithm::fill_mem1_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _sliding_zone_vertical_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm::fill_mem2_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _sliding_zone_horizontal_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm::fill_mem3_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _zone_vertical_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm::fill_mem4_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _zone_horizontal_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm::fill_mem5_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _zone_vertical_for_horizontal_memory_.load_from_file(filename_); 
      return;
    }


    void tracker_trigger_algorithm::initialize()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void tracker_trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      DT_THROW_IF(_electronic_mapping_ == 0, std::logic_error, "Missing electronic mapping ! " );

      if (config_.has_key("mem1_file")) {
	std::string mem1_filename = config_.fetch_string("mem1_file");
	datatools::fetch_path_with_env(mem1_filename);
	fill_mem1_all(mem1_filename);
      }

      if (config_.has_key("mem2_file")) {
	std::string mem2_filename = config_.fetch_string("mem2_file");
	datatools::fetch_path_with_env(mem2_filename);
	fill_mem2_all(mem2_filename);
      }

      if (config_.has_key("mem3_file")) {
	std::string mem3_filename = config_.fetch_string("mem3_file");
	datatools::fetch_path_with_env(mem3_filename);
	fill_mem3_all(mem3_filename);
      }

      if (config_.has_key("mem4_file")) {
	std::string mem4_filename = config_.fetch_string("mem4_file");
	datatools::fetch_path_with_env(mem4_filename);
	fill_mem4_all(mem4_filename);
      }

      if (config_.has_key("mem5_file")) {
	std::string mem5_filename = config_.fetch_string("mem5_file");
	datatools::fetch_path_with_env(mem5_filename);
	fill_mem5_all(mem5_filename);
      }

      _initialized_ = true;
      return;
    }

    bool tracker_trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void tracker_trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Tracker trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      reset_matrix();
      _tracker_finale_decision_ = false;
      return;
    }

    uint32_t tracker_trigger_algorithm::get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const
    {
      std::bitset<geiger::tp::BOARD_ID_WORD_SIZE> temporary_board_bitset;
      for (unsigned int i = geiger::tp::BOARD_ID_BIT0; i <= geiger::tp::BOARD_ID_BIT4; i++)
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

    void tracker_trigger_algorithm::build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_, 
								  std::vector<geomtools::geom_id> & hit_cells_gids_) const
    {
      for (unsigned int i = 0; i < mapping::NUMBER_OF_FEBS_BY_CRATE; i++)
	{
	  
	  // my_geiger_ctw_.tree_dump(std::clog, "MY GG CTW", "INFO : ");
	  // Take care here after definition change of Geiger CTW
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
		  uint32_t board_id  = get_board_id(my_bitset);
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

    bool tracker_trigger_algorithm::get_tracker_decision() const
    {
      return _tracker_finale_decision_;
    }

    void tracker_trigger_algorithm::fill_matrix(const std::vector<geomtools::geom_id> & hit_cells_gids_)
    {
      if (hit_cells_gids_.size() != 0)
	{
	  for (unsigned int i = 0; i < hit_cells_gids_.size(); i++)
	    {
	      unsigned int side  = hit_cells_gids_[i].get(mapping::SIDE_INDEX);
	      unsigned int layer = hit_cells_gids_[i].get(mapping::LAYER_INDEX);
	      unsigned int row   = hit_cells_gids_[i].get(mapping::ROW_INDEX);
	      _geiger_matrix_[side][layer][row] = 1;
	    }    
	}
      return;
    }

    void tracker_trigger_algorithm::display_matrix() const
    {
      std::clog << "  |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl;

      for (unsigned int i = 0; i < trigger_info::NSIDES; i++)
	{
	  if (i == 0)
	    {
	      for (unsigned int j = trigger_info::NLAYERS - 1; j > 0; j--) // Value NLAYERS = 9
		{
		  std::clog << j << ' ';
		  for (unsigned int k = 0; k < trigger_info::NROWS; k++)
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
	      for (unsigned int j = 0; j < trigger_info::NLAYERS; j++)
		{
		  std::clog << j << ' ' ;
		  for (unsigned int k = 0; k < trigger_info::NROWS; k++)
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

    void tracker_trigger_algorithm::reset_matrix()
    {
      for (unsigned int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (unsigned int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
	    {
	      for (unsigned int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		{
		  _geiger_matrix_[iside][jlayer][krow] = 0;
		} // end of krow
	    } // end of jlayer
	} // end of iside
      return;
    }
    
    
    void tracker_trigger_algorithm::reset_zones_informations()
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  for (unsigned int iszone = 0; iszone < trigger_info::NSLZONES; iszone++) 
	    {
	      _sliding_zones_[iside][iszone].reset();
	    }
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      _zones_[iside][izone].reset();
	    }
	}
      _tracker_record_finale_decision_.reset();
      return;
    }

    void tracker_trigger_algorithm::build_sliding_zone(tracker_sliding_zone & szone_, unsigned int side_, unsigned int szone_id_)
    {
      _sliding_zones_[side_][szone_id_].side = side_;
      _sliding_zones_[side_][szone_id_].szone_id = szone_id_;

      unsigned int stop_row = tracker_sliding_zone::stop_row(szone_id_);
      unsigned int start_row = tracker_sliding_zone::start_row(szone_id_);  
      for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) 
	{
	  for (unsigned int irow = start_row; irow <= stop_row; irow++) 
	    {
	      _sliding_zones_[side_][szone_id_].cells[ilayer][irow - start_row] = _geiger_matrix_[side_][ilayer][irow];
	    }
	}
      _sliding_zones_[side_][szone_id_].compute_lr_proj();
      
      return;
    }	

    void tracker_trigger_algorithm::build_sliding_zones(tracker_trigger_mem_maker::mem1_type & mem1_,
							tracker_trigger_mem_maker::mem2_type & mem2_)
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) {
	for (unsigned int iszone = 0; iszone < trigger_info::NSLZONES; iszone ++) {
	  build_sliding_zone(_sliding_zones_[iside][iszone], iside, iszone);
	  _sliding_zones_[iside][iszone].build_pattern(mem1_, mem2_);
	  //_sliding_zones_[iside][iszone].print(std::clog);
	}
      }
      return;
    }

    void tracker_trigger_algorithm::build_zone(tracker_zone & zone_, unsigned int side_, unsigned int zone_id_)
    {
      _zones_[side_][zone_id_].side = side_;
      _zones_[side_][zone_id_].zone_id = zone_id_;

      unsigned int stop_row = tracker_zone::stop_row(zone_id_);
      unsigned int start_row = tracker_zone::start_row(zone_id_);  
      
      for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) 
	{
	  for (unsigned int irow = start_row; irow <= stop_row; irow++) 
	    {
	      _zones_[side_][zone_id_].cells[ilayer][irow - start_row] = _geiger_matrix_[side_][ilayer][irow];
	    }
	}

      return;
    }
    
    void tracker_trigger_algorithm::build_zones()
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      build_zone(_zones_[iside][izone], iside, izone);
	      build_in_out_pattern(_zones_[iside][izone], _zone_vertical_memory_);
	      build_left_mid_right_pattern(_zones_[iside][izone], _zone_horizontal_memory_, _zone_vertical_for_horizontal_memory_);
	      build_near_source_pattern(_zones_[iside][izone]);
	    }
	}
      return;
    }    
    
    void tracker_trigger_algorithm::build_in_out_pattern(tracker_zone & zone_,
							 tracker_trigger_mem_maker::mem3_type & mem3_)
    {
      unsigned int side = zone_.side;
      unsigned int zone_id = zone_.zone_id;
      
      std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZA_IO = _sliding_zones_[side][zone_id * 3].data_IO_proj;
      std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZB_IO = _sliding_zones_[side][zone_id * 3 + 1].data_IO_proj;
      std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZC_IO = _sliding_zones_[side][zone_id * 3 + 2].data_IO_proj;
      std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZD_IO = _sliding_zones_[side][zone_id * 3 + 3].data_IO_proj;
      
      std::bitset<trigger_info::ZONE_ADDR_IO_PATTERN_SIZE> ZONE_ADDR_IO = 0x0;

      if (SZD_IO.test(0)) ZONE_ADDR_IO.set(0, true);
      if (SZD_IO.test(1)) ZONE_ADDR_IO.set(1, true);
      if (SZC_IO.test(0)) ZONE_ADDR_IO.set(2, true);
      if (SZC_IO.test(1)) ZONE_ADDR_IO.set(3, true);
      if (SZB_IO.test(0)) ZONE_ADDR_IO.set(4, true);
      if (SZB_IO.test(1)) ZONE_ADDR_IO.set(5, true);
      if (SZA_IO.test(0)) ZONE_ADDR_IO.set(6, true);
      if (SZA_IO.test(1)) ZONE_ADDR_IO.set(7, true);
      
      zone_.addr_in_out_pattern = ZONE_ADDR_IO;
      
      mem3_.fetch(ZONE_ADDR_IO, zone_.data_in_out_pattern);
      
      return;
    }
		
    void tracker_trigger_algorithm::build_left_mid_right_pattern(tracker_zone & zone_,
								 tracker_trigger_mem_maker::mem4_type & mem4_,
								 tracker_trigger_mem_maker::mem5_type & mem5_)
    {
      unsigned int side = zone_.side;
      unsigned int zone_id = zone_.zone_id;
      
      std::bitset<trigger_info::SLZONE_DATA_LR_PROJ> SZA_LR = _sliding_zones_[side][zone_id * 3].data_LR_proj;
      std::bitset<trigger_info::SLZONE_DATA_LR_PROJ> SZB_LR = _sliding_zones_[side][zone_id * 3 + 1].data_LR_proj;
      std::bitset<trigger_info::SLZONE_DATA_LR_PROJ> SZC_LR = _sliding_zones_[side][zone_id * 3 + 2].data_LR_proj;
      std::bitset<trigger_info::SLZONE_DATA_LR_PROJ> SZD_LR = _sliding_zones_[side][zone_id * 3 + 3].data_LR_proj;
      
      std::bitset<trigger_info::ZONE_ADDR_LMR_PATTERN_SIZE+2> ZONE_ADDR_LR = 0x0;

      ZONE_ADDR_LR[0] = SZD_LR[0];
      ZONE_ADDR_LR[1] = SZD_LR[1];
      ZONE_ADDR_LR[2] = SZC_LR[0];
      ZONE_ADDR_LR[3] = SZC_LR[1];
      ZONE_ADDR_LR[4] = SZB_LR[0];
      ZONE_ADDR_LR[5] = SZB_LR[1];
      ZONE_ADDR_LR[6] = SZA_LR[0];
      ZONE_ADDR_LR[7] = SZA_LR[1];

      std::bitset<trigger_info::ZONE_ADDR_LMR_PATTERN_SIZE> ZONE_ADDR_LR_REDUCTED = 0x0;
      for (unsigned int i = 0; i < ZONE_ADDR_LR_REDUCTED.size(); i++)
	{
	  ZONE_ADDR_LR_REDUCTED[i] = ZONE_ADDR_LR[i+1];
	}

      zone_.addr_left_mid_right_pattern = ZONE_ADDR_LR_REDUCTED;

      if (ZONE_ADDR_LR_REDUCTED != 0)
	{	  	  
	  mem4_.fetch(ZONE_ADDR_LR_REDUCTED, zone_.data_left_mid_right_pattern);
	}
      else
	{
	  std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZA_IO = _sliding_zones_[side][zone_id * 3].data_IO_proj;
	  std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZB_IO = _sliding_zones_[side][zone_id * 3 + 1].data_IO_proj;
	  std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZC_IO = _sliding_zones_[side][zone_id * 3 + 2].data_IO_proj;
	  std::bitset<trigger_info::SLZONE_DATA_IO_PROJ> SZD_IO = _sliding_zones_[side][zone_id * 3 + 3].data_IO_proj;
	  
	  std::bitset<trigger_info::ZONE_ADDR_LMR_WIO_PATTERN_SIZE> ZONE_ADDR_IO = 0x0;
	  
	  ZONE_ADDR_IO[0] = SZD_IO[0];
	  ZONE_ADDR_IO[1] = SZD_IO[1];
	  ZONE_ADDR_IO[2] = SZC_IO[0];
	  ZONE_ADDR_IO[3] = SZC_IO[1];
	  ZONE_ADDR_IO[4] = SZB_IO[0];
	  ZONE_ADDR_IO[5] = SZB_IO[1];
	  ZONE_ADDR_IO[6] = SZA_IO[0];
	  ZONE_ADDR_IO[7] = SZA_IO[1];

	  zone_.addr_left_mid_right_with_in_out_pattern = ZONE_ADDR_IO;

	  mem5_.fetch(ZONE_ADDR_IO, zone_.data_left_mid_right_pattern);
	}
      return;
    }

    void tracker_trigger_algorithm::build_near_source_pattern(tracker_zone & zone_)
    {     
      unsigned int zone_id = zone_.zone_id;
      unsigned int zone_width = tracker_zone::width(zone_id);
      unsigned int zone_middle = zone_width / 2.;

      zone_.data_near_source = 0x0;
      
      for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) 
	{
	  for (unsigned int irow = 0; irow < zone_width; irow++)
	    {
	      if (zone_.cells[ilayer][irow]) 
		{
		  if (ilayer < trigger_info::NUMBER_OF_LAYERS_HIT_FOR_NEAR_SOURCE_BIT) 
		    {
		      if (zone_middle % 2 == 1)
			{
			  if (irow <= zone_middle) zone_.data_near_source.set(tracker_zone::DATA_NEAR_SOURCE_BIT_LEFT, true);
			  if (irow >= zone_middle) zone_.data_near_source.set(tracker_zone::DATA_NEAR_SOURCE_BIT_RIGHT, true);
			}
		      else 
			{
			  if (irow < zone_middle) zone_.data_near_source.set(tracker_zone::DATA_NEAR_SOURCE_BIT_LEFT, true);
			  else zone_.data_near_source.set(tracker_zone::DATA_NEAR_SOURCE_BIT_RIGHT, true);
			}
		    }
		}
	    }
	}
      
      return;
    }
    
    void tracker_trigger_algorithm::build_tracker_record()
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_INNER]   = _zones_[iside][izone].data_in_out_pattern[0];
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_OUTER]   = _zones_[iside][izone].data_in_out_pattern[1];
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_RIGHT]   = _zones_[iside][izone].data_left_mid_right_pattern[0];
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_MIDDLE]  = _zones_[iside][izone].data_left_mid_right_pattern[1];
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_LEFT]    = _zones_[iside][izone].data_left_mid_right_pattern[2];
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_NSZ_RIGHT] = _zones_[iside][izone].data_near_source[0];
	      _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_NSZ_LEFT]  = _zones_[iside][izone].data_near_source[1];
	    }
	}
      
      // Build tracker zoning words 
      // - mode LMR "normal" delayed (pattern) -> Tracker / Tracker + calo
      // - mode Near source (1 cell min in the first 4 layers) -> Tracker / tracker

      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      std::bitset<3> hpattern_for_a_zone = 0x0;
	      std::bitset<2> near_source_for_a_zone = 0x0;
	      	  
	      hpattern_for_a_zone[0]    = _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_RIGHT];     // RIGHT
	      hpattern_for_a_zone[1]    = _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_MIDDLE];    // MIDDLE
	      hpattern_for_a_zone[2]    = _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_LEFT];      // LEFT
	      near_source_for_a_zone[0] = _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_NSZ_RIGHT]; // NSZRIGHT
	      near_source_for_a_zone[1] = _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][tracker_record::FINALE_DATA_BIT_NSZ_LEFT ]; // NSZLEFT
	      
	      if (hpattern_for_a_zone.any()) _tracker_record_finale_decision_.zoning_word_pattern[iside].set(izone);
	      if (near_source_for_a_zone.any()) _tracker_record_finale_decision_.zoning_word_near_source[iside].set(izone);
	    }
	}
      
      std::bitset<5> finale_pattern_per_zone = 0x0;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      for (unsigned int i = 0; i < finale_pattern_per_zone.size(); i++)
		{
		  finale_pattern_per_zone[i] = _tracker_record_finale_decision_.finale_data_per_zone[iside][izone][i];
		}
	      
	      if (finale_pattern_per_zone.any())
		{
		  _tracker_record_finale_decision_.finale_decision = true;
		} 
	    }
	}
      return;
    }

    void tracker_trigger_algorithm::print_zones(std::ostream & out_) const
    {
      out_ << "Zones: \n";
      for (unsigned int ilayer = trigger_info::NLAYERS - 1; ilayer > 0; ilayer--) {
	for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) {
	  for (unsigned int irow = 0; irow < tracker_zone::width(izone); irow++) {
	    out_ << (_zones_[0][izone].cells[ilayer][irow] ? 'o' : '.');
	  }
	  out_ << ' ';
	}
	out_ << '\n';
      }
      for (unsigned int irow = 0; irow < trigger_info::NROWS + trigger_info::NZONES - 1; irow++) {
	out_ << '=';
      }
      out_ << '\n';
      for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) {
	for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) {
	  for (unsigned int irow = 0; irow < tracker_zone::width(izone); irow++) {
	    out_ << (_zones_[1][izone].cells[ilayer][irow] ? 'o' : '.');
	  }
	  out_ << ' ';
	}
	out_ << '\n';
      }
      out_ << '\n';
         
      return;
    }
    
    void tracker_trigger_algorithm::print_zone_information(std::ostream & out_) const
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) {

	    out_ << "SIDE : " << iside << " Zone ID : " << izone << "\n";
	    _zones_[iside][izone].print_projections(out_);
	    out_ << "\n";
	  }
	}   
      return;
    }
    
    
    void tracker_trigger_algorithm::print_sliding_zones(std::ostream & out_) const
    {
      out_ << "Sliding zones: \n";
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  out_ << "*************************** \n";
	  out_ << "Side = " << iside << "\n";
	  for (unsigned int islzone = 0; islzone < trigger_info::NSLZONES; islzone++)
	    {
	      if (islzone == 0) out_ << "ZONE ID : 0 \n";
	      if (islzone == 0) {
		out_ << "SLZ_A : \n";
		_sliding_zones_[iside][islzone].print_projections(out_);
	      }
	      if (islzone % 3 == 1) {
		out_ << "SLZ_B : \n";
		_sliding_zones_[iside][islzone].print_projections(out_);
	      }
	      if (islzone % 3 == 2) {
		out_ << "SLZ_C : \n";
		_sliding_zones_[iside][islzone].print_projections(out_);
	      }
	      if (islzone % 3 == 0 && islzone > 0) {
		out_ << "SLZ_D : \n";
		_sliding_zones_[iside][islzone].print_projections(out_);
		out_ << "\n";
		if (_sliding_zones_[iside][islzone].szone_id / 3 == 10) {}
		else
		  {
		    out_ << "ZONE ID : " << _sliding_zones_[iside][islzone].szone_id / 3 << "\n";
		    out_ << "SLZ_A : \n";
		    _sliding_zones_[iside][islzone].print_projections(out_);
		  }
	      }
	    } // end of islzone
	} // end of iside
      return;
    }

    void tracker_trigger_algorithm::_process_for_a_clocktick(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_)
    {
      reset_matrix();
      reset_zones_informations();
      for (unsigned int isize = 0; isize < geiger_ctw_list_per_clocktick_.size(); isize++)
       	{
       	  std::vector<geomtools::geom_id> hit_cells_gids;
       	  build_hit_cells_gids_from_ctw(geiger_ctw_list_per_clocktick_[isize].get(),
					hit_cells_gids);
       	  fill_matrix(hit_cells_gids);   
	} // end of isize
      geiger_matrix a_geiger_matrix;
      for (unsigned int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      	{
      	  for (unsigned int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
      	    {
      	      for (unsigned int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
      		{
      		  a_geiger_matrix.matrix[iside][jlayer][krow] = _geiger_matrix_[iside][jlayer][krow];
      		} // end of krow
      	    } // end of jlayer
      	} // end of iside
      a_geiger_matrix.clocktick_1600ns = geiger_ctw_list_per_clocktick_[0].get().get_clocktick_800ns() / 2;
      _geiger_matrix_records_.push_back(a_geiger_matrix);
      _tracker_record_finale_decision_.clocktick_1600ns = geiger_ctw_list_per_clocktick_[0].get().get_clocktick_800ns() / 2;
      build_sliding_zones(_sliding_zone_vertical_memory_, _sliding_zone_horizontal_memory_); 
      build_zones();      
      build_tracker_record();

      // Display for Debug with Thierry & Garrido :
      //print_zones(std::clog);
      //if (geiger_ctw_list_per_clocktick_[0].get().get_clocktick_800ns() / 2 == 2) print_sliding_zones(std::clog);
      //print_zones(std::clog);
      //print_zone_information(std::clog);
      //_tracker_record_finale_decision_.display();
      // display_matrix();
      return;
    }
    
    void tracker_trigger_algorithm::_process(const geiger_ctw_data & geiger_ctw_data_,
					     std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_)
    { 
      _geiger_matrix_records_.clear();
      _tracker_finale_decision_ = false;
      // Just even clockticks 800 ns are processing (to take in account CB to TB serdes limitation)
      int32_t iclocktick_800 = geiger_ctw_data_.get_clocktick_min();
      if (iclocktick_800 % 2 == 1) iclocktick_800 += 1;
      for (iclocktick_800; iclocktick_800 <= geiger_ctw_data_.get_clocktick_max(); iclocktick_800 +=2)
       	{
       	  std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick;
      	  geiger_ctw_data_.get_list_of_geiger_ctw_per_clocktick(iclocktick_800, geiger_ctw_list_per_clocktick);
	  if (geiger_ctw_list_per_clocktick.size() != 0)
      	    {
	      _process_for_a_clocktick(geiger_ctw_list_per_clocktick);
	      if (_tracker_record_finale_decision_.finale_decision) _tracker_finale_decision_ = true;
	      tracker_records_.push_back(_tracker_record_finale_decision_);
	    } // end of if ctw list != 0
       	} // end of iclocktick
      
      // if (_geiger_matrix_records_.size() != 0)
      // 	{
      // 	  _geiger_matrix_records_[2].display_matrix_garrido();
      // 	}

      return;
    }

    void tracker_trigger_algorithm::process(const geiger_ctw_data & geiger_ctw_data_,
					    std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Tracker trigger algorithm is not initialized, it can't process ! ");
      _process(geiger_ctw_data_, tracker_records_);
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
