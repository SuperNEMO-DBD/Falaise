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

    tracker_trigger_algorithm_test_new_strategy::tracker_record::tracker_record()
    {
      clocktick_1600ns = -1;
      //finale_decision.reset();
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::tracker_record::reset()
    {
      clocktick_1600ns = -1;
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::tracker_record::display()
    {

      return;
    }

    tracker_trigger_algorithm_test_new_strategy::geiger_matrix::geiger_matrix()
    {
      for (int iside = 0; iside <tracker_info::NSIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < tracker_info::NLAYERS; jlayer++)
	    {
	      for (int krow = 0; krow < tracker_info::NROWS; krow++)
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
      for (int iside = 0; iside < tracker_info::NSIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < tracker_info::NLAYERS; jlayer++)
	    {
	      for (int krow = 0; krow < tracker_info::NROWS; krow++)
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
      static const size_t nmax = tracker_info::NSIDES * tracker_info::NLAYERS * tracker_info::NROWS;
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

    void tracker_trigger_algorithm_test_new_strategy::fill_mem1_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _sliding_zone_vertical_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_mem2_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _sliding_zone_horizontal_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_mem3_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _zone_vertical_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_mem4_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _zone_horizontal_memory_.load_from_file(filename_); 
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::fill_mem5_all(const std::string & filename_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Tracker trigger algorithm is already initialized ! ");
      _zone_vertical_for_horizontal_memory_.load_from_file(filename_); 
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

    bool tracker_trigger_algorithm_test_new_strategy::is_initialized() const
    {
      return _initialized_;
    }

    void tracker_trigger_algorithm_test_new_strategy::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Tracker trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      reset_matrix();
      _tracker_finale_decision_ = false;
      return;
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

      for (int i = 0; i < tracker_info::NSIDES; i++)
	{
	  if (i == 0)
	    {
	      for (int j = tracker_info::NLAYERS - 1; j >= 0; j--) // Value GEIGER_LAYER_SIZE = 9
		{
		  std::clog << j << ' ';
		  for (int k = 0; k < tracker_info::NROWS; k++)
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
	      for (int j = 0; j < tracker_info::NLAYERS; j++)
		{
		  std::clog << j << ' ' ;
		  for (int k = 0; k < tracker_info::NROWS; k++)
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
    
    
    void tracker_trigger_algorithm_test_new_strategy::reset_zones_informations()
    {
      for (int iside = 0; iside < tracker_info::NSIDES; iside++)
	{
	  for (int iszone = 0; iszone < tracker_info::NSLZONES; iszone++) 
	    {
	      _sliding_zones_[iside][iszone].reset();
	    }
	  for (int izone = 0; izone < tracker_info::NZONES; izone++) 
	    {
	      _zones_[iside][izone].reset();
	    }
	}

      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::build_sliding_zone(tracker_sliding_zone & szone_, int side_, int szone_id_)
    {
      _sliding_zones_[side_][szone_id_].side = side_;
      _sliding_zones_[side_][szone_id_].szone_id = szone_id_;

      int stop_row = tracker_sliding_zone::stop_row(szone_id_);
      int start_row = tracker_sliding_zone::start_row(szone_id_);  
      for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) 
	{
	  for (int irow = start_row; irow <= stop_row; irow++) 
	    {
	      _sliding_zones_[side_][szone_id_].cells[ilayer][irow - start_row] = _geiger_matrix_[side_][ilayer][irow];
	    }
	}
      _sliding_zones_[side_][szone_id_].compute_lr_proj();
      
      return;
    }	

    void tracker_trigger_algorithm_test_new_strategy::build_sliding_zones(tracker_trigger_mem_maker_new_strategy::mem1_type & mem1_,
									  tracker_trigger_mem_maker_new_strategy::mem2_type & mem2_)
    {
      for (int iside = 0; iside < tracker_info::NSIDES; iside++) {
	for (int iszone = 0; iszone < tracker_info::NSLZONES; iszone ++) {
	  build_sliding_zone(_sliding_zones_[iside][iszone], iside, iszone);
	  _sliding_zones_[iside][iszone].build_pattern(mem1_, mem2_);
	  _sliding_zones_[iside][iszone].print(std::clog);
	}
      }
      return;
    }	

    void tracker_trigger_algorithm_test_new_strategy::build_zones()
    {
      for (int iside = 0; iside < tracker_info::NSIDES; iside++) 
	{
	  for (int izone = 0; izone < tracker_info::NZONES; izone++) 
	    {
	      for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) 
		{
		  for (int irow = tracker_zone::start_row(izone);
		       irow <= tracker_zone::stop_row(izone);
		       irow++) 
		    {
		      _zones_[iside][izone].cells[ilayer][irow - tracker_zone::start_row(izone)] = _geiger_matrix_[iside][ilayer][irow];
		    }
		}
	    }
	}
      return;
    }

    void tracker_trigger_algorithm_test_new_strategy::print_zones(std::ostream & out_) const
      {
        out_ << "Zones: \n";
        for (int ilayer = tracker_info::NLAYERS - 1; ilayer >= 0; ilayer--) {
          for (int izone = 0; izone < tracker_info::NZONES; izone++) {
            for (int irow = 0; irow < tracker_zone::width(izone); irow++) {
              out_ << (_zones_[0][izone].cells[ilayer][irow] ? 'o' : '.');
            }
            out_ << ' ';
          }
          out_ << '\n';
        }
        for (int irow = 0; irow < tracker_info::NROWS + tracker_info::NZONES - 1; irow++) {
          out_ << '=';
        }
        out_ << '\n';
        for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) {
          for (int izone = 0; izone < tracker_info::NZONES; izone++) {
            for (int irow = 0; irow < tracker_zone::width(izone); irow++) {
              out_ << (_zones_[1][izone].cells[ilayer][irow] ? 'o' : '.');
            }
            out_ << ' ';
          }
          out_ << '\n';
        }
        out_ << '\n';

        return;
      }
    
    void tracker_trigger_algorithm_test_new_strategy::_process_for_a_clocktick(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_)
    {
      reset_matrix();
      reset_zones_informations();
      for (int isize = 0; isize < geiger_ctw_list_per_clocktick_.size(); isize++)
       	{
       	  std::vector<geomtools::geom_id> hit_cells_gids;
       	  build_hit_cells_gids_from_ctw(geiger_ctw_list_per_clocktick_[isize].get(), hit_cells_gids);
       	  fill_matrix(hit_cells_gids);   
	  display_matrix();
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
      
      build_zones();
      print_zones(std::clog);
      build_sliding_zones(_sliding_zone_vertical_memory_, _sliding_zone_horizontal_memory_);
      // build_zone_tracker_trigger_after_projections();
      // build_zone_tracker_trigger_info();
      // build_tracker_record();
      
      return;
    }
    
    void tracker_trigger_algorithm_test_new_strategy::_process(const geiger_ctw_data & geiger_ctw_data_,
							       std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_)
    { 
      _geiger_matrix_records_.clear();
      // Just even clockticks are processing (to take in account CB to TB serdes limitation
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
