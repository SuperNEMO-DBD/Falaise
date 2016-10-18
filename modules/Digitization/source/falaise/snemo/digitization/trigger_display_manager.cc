// snemo/digitization/trigger_display_manager.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/trigger_algorithm.h>

namespace snemo {

  namespace digitization {

    const uint32_t trigger_display_manager::NUMBER_OF_HORIZONTAL_CHAR;
    const uint32_t trigger_display_manager::NUMBER_OF_VERTICAL_CHAR;

    trigger_display_manager::trigger_display_manager()
    {
      _initialized_      = false;
      _calo_25ns_        = false;
      _calo_1600ns_      = false;
      _tracker_1600ns_   = false;
      _coinc_1600ns_     = false;
      _decision_trigger_ = false;

      for (unsigned int i = 0; i < NUMBER_OF_HORIZONTAL_CHAR; i++)
    	{
    	  for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    	    {
    	      _char_matrix_[i][j] = '.';
    	    }
    	}
      return;
    }

    trigger_display_manager::~trigger_display_manager()
    {
      if (is_initialized())
    	{
    	  reset();
    	}
      return;
    }

    void trigger_display_manager::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void trigger_display_manager::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger display manager is already initialized ! ");
      if (!is_calo_25ns()) {
    	if (config_.has_key("calo_25ns")) {
    	  bool calo_25ns_config = config_.fetch_boolean("calo_25ns");
    	  _calo_25ns_ = calo_25ns_config;
    	}
      }

      if (!is_calo_1600ns()) {
    	if (config_.has_key("calo_1600ns")) {
    	  bool calo_1600ns_config = config_.fetch_boolean("calo_1600ns");
    	  _calo_1600ns_ = calo_1600ns_config;
    	}
      }

      if (!is_tracker_1600ns()) {
    	if (config_.has_key("tracker_1600ns")) {
    	  bool tracker_1600ns_config = config_.fetch_boolean("tracker_1600ns");
    	  _tracker_1600ns_ = tracker_1600ns_config;
    	}
      }

      if (!is_coinc_1600ns()) {
    	if (config_.has_key("coinc_1600ns")) {
    	  bool coinc_1600ns_config = config_.fetch_boolean("coinc_1600ns");
    	  _coinc_1600ns_ = coinc_1600ns_config;
    	}
      }

      if (!is_decision_trigger()) {
    	if (config_.has_key("decision_trigger")) {
    	  bool decision_trigger_config = config_.fetch_boolean("decision_trigger");
    	  _decision_trigger_ = decision_trigger_config;
    	}
      }
      fill_matrix_pattern();
      _initialized_ = true;
      return;
    }

    bool trigger_display_manager::is_initialized() const
    {
      return _initialized_;
    }

    void trigger_display_manager::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger display manager is not initialized, it can't be reset ! ");
      _initialized_      = false;
      _calo_25ns_        = false;
      _calo_1600ns_      = false;
      _tracker_1600ns_   = false;
      _coinc_1600ns_     = false;
      _decision_trigger_ = false;
      return;
    }

    bool trigger_display_manager::is_calo_25ns() const
    {
      return _calo_25ns_;
    }

    bool trigger_display_manager::is_calo_1600ns() const
    {
      return _calo_1600ns_;
    }

    bool trigger_display_manager::is_tracker_1600ns() const
    {
      return _tracker_1600ns_;
    }

    bool trigger_display_manager::is_coinc_1600ns() const
    {
      return _coinc_1600ns_;
    }

    bool trigger_display_manager::is_decision_trigger() const
    {
      return _decision_trigger_;
    }

    void trigger_display_manager::fill_calo_trigger_matrix_25ns(std::bitset<10> zoning_word_[trigger_info::NSIDES])
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
       	{
      	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9)
      		{
      		  if (zoning_word_[iside][izone] == true)
      		    {
      		      for (uint32_t j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
      			{
      			  if      (iside == 0 && izone == 0 && j > 3 && j < 11) _char_matrix_[0][j] = '*';
      			  else if (iside == 1 && izone == 0 && j > 3 && j < 11) _char_matrix_[20][j] = '*';
      			  else if (iside == 0 && izone == 9 && j > 107 && j < 115) _char_matrix_[0][j] = '*';
      			  else if (iside == 1 && izone == 9 && j > 107 && j < 115) _char_matrix_[20][j] = '*';
      			}
      		    }
      		  else
      		    {
      		      for (uint32_t j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
      			{
      			  if      (iside == 0 && izone == 0 && j > 3 && j < 11) _char_matrix_[0][j] = ' ';
      			  else if (iside == 1 && izone == 0 && j > 3 && j < 11) _char_matrix_[20][j] = ' ';
      			  else if (iside == 0 && izone == 9 && j > 107 && j < 115) _char_matrix_[0][j] = ' ';
      			  else if (iside == 1 && izone == 9 && j > 107 && j < 115) _char_matrix_[20][j] = ' ';
      			}
      		    }
      		} //end of if izone

      	      else if (izone == 5)
      		{
      		  if (zoning_word_[iside][izone] == true)
      		    {
      		      for (uint32_t j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
      			{
      			  if      (iside == 0 && j > 60 && j < 70) _char_matrix_[0][j] = '*';
      			  else if (iside == 1 && j > 60 && j < 70) _char_matrix_[20][j] = '*';
      			}
      		    }
      		  else
      		    {
      		      for (uint32_t j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
      			{
      			  if      (iside == 0 && j > 60 && j < 70) _char_matrix_[0][j] = ' ';
      			  else if (iside == 1 && j > 60 && j < 70) _char_matrix_[20][j] = ' ';
      			}
      		    }
      		} // end of else if izone == 5

      	      else
      		{
      		  if (zoning_word_[iside][izone] == true)
      		    {
      		      for (uint32_t j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
      			{
      			  if      (iside == 0 && izone == 1 && j > 12 && j < 23) _char_matrix_[0][j]   = '*';
      			  else if (iside == 1 && izone == 1 && j > 12 && j < 23) _char_matrix_[20][j]  = '*';
      			  else if (iside == 0 && izone == 2 && j > 24 && j < 35) _char_matrix_[0][j]   = '*';
      			  else if (iside == 1 && izone == 2 && j > 24 && j < 35) _char_matrix_[20][j]  = '*';
      			  else if (iside == 0 && izone == 3 && j > 36 && j < 47) _char_matrix_[0][j]   = '*';
      			  else if (iside == 1 && izone == 3 && j > 36 && j < 47) _char_matrix_[20][j]  = '*';
      			  else if (iside == 0 && izone == 4 && j > 48 && j < 59) _char_matrix_[0][j]   = '*';
      			  else if (iside == 1 && izone == 4 && j > 48 && j < 59) _char_matrix_[20][j]  = '*';
      			  else if (iside == 0 && izone == 6 && j > 71 && j < 82) _char_matrix_[0][j]   = '*';
      			  else if (iside == 1 && izone == 6 && j > 71 && j < 82) _char_matrix_[20][j]  = '*';
      			  else if (iside == 0 && izone == 7 && j > 83 && j < 94) _char_matrix_[0][j]   = '*';
      			  else if (iside == 1 && izone == 7 && j > 83 && j < 94) _char_matrix_[20][j]  = '*';
      			  else if (iside == 0 && izone == 8 && j > 95 && j < 106) _char_matrix_[0][j]  = '*';
      			  else if (iside == 1 && izone == 8 && j > 95 && j < 106) _char_matrix_[20][j] = '*';
      			}
      		    }
      		  else
      		    {
      		      for (uint32_t j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
      			{
      			  if      (iside == 0 && izone == 1 && j > 12 && j < 23) _char_matrix_[0][j]   = ' ';
      			  else if (iside == 1 && izone == 1 && j > 12 && j < 23) _char_matrix_[20][j]  = ' ';
      			  else if (iside == 0 && izone == 2 && j > 24 && j < 35) _char_matrix_[0][j]   = ' ';
      			  else if (iside == 1 && izone == 2 && j > 24 && j < 35) _char_matrix_[20][j]  = ' ';
      			  else if (iside == 0 && izone == 3 && j > 36 && j < 47) _char_matrix_[0][j]   = ' ';
      			  else if (iside == 1 && izone == 3 && j > 36 && j < 47) _char_matrix_[20][j]  = ' ';
      			  else if (iside == 0 && izone == 4 && j > 48 && j < 59) _char_matrix_[0][j]   = ' ';
      			  else if (iside == 1 && izone == 4 && j > 48 && j < 59) _char_matrix_[20][j]  = ' ';
      			  else if (iside == 0 && izone == 6 && j > 71 && j < 82) _char_matrix_[0][j]   = ' ';
      			  else if (iside == 1 && izone == 6 && j > 71 && j < 82) _char_matrix_[20][j]  = ' ';
      			  else if (iside == 0 && izone == 7 && j > 83 && j < 94) _char_matrix_[0][j]   = ' ';
      			  else if (iside == 1 && izone == 7 && j > 83 && j < 94) _char_matrix_[20][j]  = ' ';
      			  else if (iside == 0 && izone == 8 && j > 95 && j < 106) _char_matrix_[0][j]  = ' ';
      			  else if (iside == 1 && izone == 8 && j > 95 && j < 106) _char_matrix_[20][j] = ' ';
      			}
      		    }
      		} // end of else izone
      	    } // end of izone
      	} // end of iside
      return;
    }

    void trigger_display_manager::fill_calo_trigger_matrix_1600ns(std::bitset<10> zoning_word_[trigger_info::NSIDES])
    {
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
       	{
     	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9)
      		{
      		  if (zoning_word_[iside][izone] == true)
    		    {
    		      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    			{
    			  if      (iside == 0 && izone == 0 && j > 3 && j < 11) _char_matrix_[0][j] = '*';
    			  else if (iside == 1 && izone == 0 && j > 3 && j < 11) _char_matrix_[20][j] = '*';
    			  else if (iside == 0 && izone == 9 && j > 107 && j < 115) _char_matrix_[0][j] = '*';
    			  else if (iside == 1 && izone == 9 && j > 107 && j < 115) _char_matrix_[20][j] = '*';
    			}
    		    }
    		  else
    		    {
    		      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    			{
    			  if      (iside == 0 && izone == 0 && j > 3 && j < 11) _char_matrix_[0][j] = ' ';
    			  else if (iside == 1 && izone == 0 && j > 3 && j < 11) _char_matrix_[20][j] = ' ';
    			  else if (iside == 0 && izone == 9 && j > 107 && j < 115) _char_matrix_[0][j] = ' ';
    			  else if (iside == 1 && izone == 9 && j > 107 && j < 115) _char_matrix_[20][j] = ' ';
    			}
    		    }
    		} //end of if izone

    	      else if (izone == 5)
      		{
      		  if (zoning_word_[iside][izone] == true)
    		    {
    		      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    			{
    			  if      (iside == 0 && j > 60 && j < 70) _char_matrix_[0][j] = '*';
    			  else if (iside == 1 && j > 60 && j < 70) _char_matrix_[20][j] = '*';
    			}
    		    }
    		  else
    		    {
    		      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    			{
    			  if      (iside == 0 && j > 60 && j < 70) _char_matrix_[0][j] = ' ';
    			  else if (iside == 1 && j > 60 && j < 70) _char_matrix_[20][j] = ' ';
    			}
    		    }
    		} // end of else if izone == 5

    	      else
    		{
    		  if (zoning_word_[iside][izone] == true)
    		    {
    		      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    			{
    			  if      (iside == 0 && izone == 1 && j > 12 && j < 23) _char_matrix_[0][j]   = '*';
    			  else if (iside == 1 && izone == 1 && j > 12 && j < 23) _char_matrix_[20][j]  = '*';
    			  else if (iside == 0 && izone == 2 && j > 24 && j < 35) _char_matrix_[0][j]   = '*';
    			  else if (iside == 1 && izone == 2 && j > 24 && j < 35) _char_matrix_[20][j]  = '*';
    			  else if (iside == 0 && izone == 3 && j > 36 && j < 47) _char_matrix_[0][j]   = '*';
    			  else if (iside == 1 && izone == 3 && j > 36 && j < 47) _char_matrix_[20][j]  = '*';
    			  else if (iside == 0 && izone == 4 && j > 48 && j < 59) _char_matrix_[0][j]   = '*';
    			  else if (iside == 1 && izone == 4 && j > 48 && j < 59) _char_matrix_[20][j]  = '*';
    			  else if (iside == 0 && izone == 6 && j > 71 && j < 82) _char_matrix_[0][j]   = '*';
    			  else if (iside == 1 && izone == 6 && j > 71 && j < 82) _char_matrix_[20][j]  = '*';
    			  else if (iside == 0 && izone == 7 && j > 83 && j < 94) _char_matrix_[0][j]   = '*';
    			  else if (iside == 1 && izone == 7 && j > 83 && j < 94) _char_matrix_[20][j]  = '*';
    			  else if (iside == 0 && izone == 8 && j > 95 && j < 106) _char_matrix_[0][j]  = '*';
    			  else if (iside == 1 && izone == 8 && j > 95 && j < 106) _char_matrix_[20][j] = '*';

    			}
    		    }
    		  else
    		    {
    		      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    			{
    			  if      (iside == 0 && izone == 1 && j > 12 && j < 23) _char_matrix_[0][j]   = ' ';
    			  else if (iside == 1 && izone == 1 && j > 12 && j < 23) _char_matrix_[20][j]  = ' ';
    			  else if (iside == 0 && izone == 2 && j > 24 && j < 35) _char_matrix_[0][j]   = ' ';
    			  else if (iside == 1 && izone == 2 && j > 24 && j < 35) _char_matrix_[20][j]  = ' ';
    			  else if (iside == 0 && izone == 3 && j > 36 && j < 47) _char_matrix_[0][j]   = ' ';
    			  else if (iside == 1 && izone == 3 && j > 36 && j < 47) _char_matrix_[20][j]  = ' ';
    			  else if (iside == 0 && izone == 4 && j > 48 && j < 59) _char_matrix_[0][j]   = ' ';
    			  else if (iside == 1 && izone == 4 && j > 48 && j < 59) _char_matrix_[20][j]  = ' ';
    			  else if (iside == 0 && izone == 6 && j > 71 && j < 82) _char_matrix_[0][j]   = ' ';
    			  else if (iside == 1 && izone == 6 && j > 71 && j < 82) _char_matrix_[20][j]  = ' ';
    			  else if (iside == 0 && izone == 7 && j > 83 && j < 94) _char_matrix_[0][j]   = ' ';
    			  else if (iside == 1 && izone == 7 && j > 83 && j < 94) _char_matrix_[20][j]  = ' ';
    			  else if (iside == 0 && izone == 8 && j > 95 && j < 106) _char_matrix_[0][j]  = ' ';
    			  else if (iside == 1 && izone == 8 && j > 95 && j < 106) _char_matrix_[20][j] = ' ';
    			}
    		    }
    		} // end of else izone
    	    } // end of izone
    	} // end of iside

      return;
    }


    void trigger_display_manager::fill_tracker_trigger_matrix_1600ns(bool geiger_matrix_[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS])
    {

      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      	{
      	  unsigned int layer = 0;
      	  for (int jlayer = trigger_info::NLAYERS - 1; jlayer >= 0; jlayer--) // Value GEIGER_LAYER_SIZE = 9
      	    {
      	      for (unsigned int krow = 0; krow < trigger_info::NROWS; krow++)
      		{
      		  if (geiger_matrix_[iside][jlayer][krow]
      		      && iside == 0)
      		    {
      		      _char_matrix_[layer+1][krow+3] = '*';
      		    }
      		  else if (geiger_matrix_[iside][jlayer][krow]
      			   && iside == 1)
      		    {
      		      _char_matrix_[jlayer+11][krow+3] = '*';
      		    }
      		} // end of krow
      	      layer++;
      	    } // end of jlayer
      	} // end of iside
      return;
    }

    void trigger_display_manager::fill_coincidence_trigger_matrix_1600ns(std::bitset<10> zoning_word_[trigger_info::NSIDES], bool geiger_matrix_[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS])
    {
      reset_matrix_pattern();
      fill_calo_trigger_matrix_1600ns(zoning_word_);
      fill_tracker_trigger_matrix_1600ns(geiger_matrix_);
      return;
    }

    void trigger_display_manager::display_calo_trigger_25ns(trigger_algorithm & a_trigger_algo_, int32_t clocktick_25ns_)
    {
      DT_THROW_IF(!is_calo_25ns(), std::logic_error, "Boolean calo 25ns is not activated, it can't display ! ");

      calo_trigger_algorithm::calo_summary_record a_calo_summary_record;
      for (unsigned int i = 0; i < a_trigger_algo_._calo_records_.size(); i++)
    	{
    	  if (clocktick_25ns_ == a_trigger_algo_._calo_records_[i].clocktick_25ns) a_calo_summary_record = a_trigger_algo_._calo_records_[i];
    	}

      std::clog << "************************************************************************************" << std::endl;
      std::clog << "********************* Display Calorimeter trigger info @ 25 ns *********************" << std::endl;
      std::clog << "********************* Clocktick 25 ns = " <<  clocktick_25ns_ << " ********************* " << std::endl << std::endl;
      std::clog << "CT |XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl;
      std::clog << a_calo_summary_record.clocktick_25ns << ' ';
      std::clog << a_calo_summary_record.xt_info_bitset << ' ';
      std::clog << a_calo_summary_record.LTO_gveto << ' ';
      std::clog << a_calo_summary_record.total_multiplicity_gveto << ' ';
      std::clog << a_calo_summary_record.LTO_side_1 << ' ';
      std::clog << a_calo_summary_record.LTO_side_0 << ' ';
      std::clog << a_calo_summary_record.total_multiplicity_side_1 << ' ';
      std::clog << a_calo_summary_record.total_multiplicity_side_0 << ' ';
      for (int iside = trigger_info::NSIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = trigger_info::NZONES-1; izone >= 0; izone--)
      	    {
      	      std::clog << a_calo_summary_record.zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << a_calo_summary_record.single_side_coinc
      		<< "  |  Threshold total mult : "   << a_calo_summary_record.total_multiplicity_threshold
      		<< "  |  Trigger final decision : " << a_calo_summary_record.calo_finale_decision  << std::endl;

      std::clog << std::endl;

      std::bitset<10> zoning_word[trigger_info::NSIDES];
      for (unsigned int i = 0; i < trigger_info::NSIDES; i++)
    	{
	  zoning_word[i] = 0x0;
    	  zoning_word[i] = a_calo_summary_record.zoning_word[i];
    	}
      fill_calo_trigger_matrix_25ns(zoning_word);
      display_matrix();
      std::clog << std::endl << std::endl;

      return;
    }

    void trigger_display_manager::display_calo_trigger_25ns(trigger_algorithm & a_trigger_algo_)
    {
      unsigned int vector_size = a_trigger_algo_._calo_records_.size();
      if (vector_size == 0) {}
      else{
    	int32_t clocktick_25ns_begin = a_trigger_algo_._calo_records_[0].clocktick_25ns;
    	int32_t clocktick_25ns_end = a_trigger_algo_._calo_records_[vector_size - 1].clocktick_25ns;

    	for (int32_t iclocktick = clocktick_25ns_begin; iclocktick <= clocktick_25ns_end; iclocktick++)
    	  {
    	    reset_calo_display();
    	    display_calo_trigger_25ns(a_trigger_algo_, iclocktick);
    	  }
      }
      return;
    }

    void trigger_display_manager::display_calo_trigger_1600ns(trigger_algorithm & a_trigger_algo_, int32_t clocktick_1600ns_)
    {
      DT_THROW_IF(!is_calo_1600ns(), std::logic_error, "Boolean calo 1600ns is not activated, it can't display ! ");

      coincidence_trigger_algorithm::coincidence_calo_record a_coinc_calo_record;
      for (unsigned int i = 0; i < a_trigger_algo_._coinc_algo_._coincidence_calo_records_.size(); i++)
    	{
    	  if (clocktick_1600ns_ == a_trigger_algo_._coinc_algo_._coincidence_calo_records_[i].clocktick_1600ns) a_coinc_calo_record = a_trigger_algo_._coinc_algo_._coincidence_calo_records_[i];
    	}

      std::clog << "************************************************************************************" << std::endl;
      std::clog << "******************** Display Calorimeter trigger info @ 1600 ns ********************" << std::endl;
      std::clog << "******************** Clocktick 1600 ns = " << clocktick_1600ns_ << " ******************** " << std::endl << std::endl;
      std::clog << "CT |XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl;
      std::clog << a_coinc_calo_record.clocktick_1600ns << ' ';
      std::clog << a_coinc_calo_record.xt_info_bitset << ' ';
      std::clog << a_coinc_calo_record.LTO_gveto << ' ';
      std::clog << a_coinc_calo_record.total_multiplicity_gveto << ' ';
      std::clog << a_coinc_calo_record.LTO_side_1 << ' ';
      std::clog << a_coinc_calo_record.LTO_side_0 << ' ';
      std::clog << a_coinc_calo_record.total_multiplicity_side_1 << ' ';
      std::clog << a_coinc_calo_record.total_multiplicity_side_0 << ' ';
      for (int iside = trigger_info::NSIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = trigger_info::NZONES-1; izone >= 0 ; izone--)
      	    {
      	      std::clog << a_coinc_calo_record.calo_zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << a_coinc_calo_record.single_side_coinc
    		<< "  |  Threshold total mult : "   << a_coinc_calo_record.total_multiplicity_threshold
    		<< "  |  Trigger final decision : " << a_coinc_calo_record.decision  << std::endl;

      std::clog << std::endl;

      std::bitset<10> zoning_word[trigger_info::NSIDES];
      for (unsigned int i = 0; i < trigger_info::NSIDES; i++)
    	{
    	  zoning_word[i] = a_coinc_calo_record.calo_zoning_word[i];
    	}
      fill_calo_trigger_matrix_1600ns(zoning_word);
      display_matrix();
      std::clog << std::endl;
      return;
    }

    void trigger_display_manager::display_calo_trigger_1600ns(trigger_algorithm & a_trigger_algo_)
    {
      unsigned int vector_size = a_trigger_algo_._coinc_algo_._coincidence_calo_records_.size();
      if (vector_size == 0) {}
      else{

    	int32_t clocktick_1600ns_begin = a_trigger_algo_._coinc_algo_._coincidence_calo_records_[0].clocktick_1600ns;
    	int32_t clocktick_1600ns_end = a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_size - 1].clocktick_1600ns;

    	for (int32_t iclocktick = clocktick_1600ns_begin; iclocktick <= clocktick_1600ns_end; iclocktick++)
    	  {
    	    display_calo_trigger_1600ns(a_trigger_algo_, iclocktick);
    	    reset_calo_display();
    	  }
      }
      return;
    }

    void trigger_display_manager::display_tracker_trigger_1600ns(trigger_algorithm & a_trigger_algo_, int32_t clocktick_1600ns_)
    {
      DT_THROW_IF(!is_tracker_1600ns(), std::logic_error, "Boolean tracker 1600ns is not activated, it can't display ! ");

      tracker_trigger_algorithm::tracker_record a_tracker_record;
      tracker_trigger_algorithm::geiger_matrix  a_geiger_matrix;
      for (unsigned int i = 0; i < a_trigger_algo_._tracker_records_.size(); i++)
    	{
    	  if (clocktick_1600ns_ == a_trigger_algo_._tracker_records_[i].clocktick_1600ns
    	      && clocktick_1600ns_ == a_trigger_algo_._tracker_algo_._geiger_matrix_records_[i].clocktick_1600ns)
    	    {
    	      a_tracker_record = a_trigger_algo_._tracker_records_[i];
    	      a_geiger_matrix  = a_trigger_algo_._tracker_algo_._geiger_matrix_records_[i];
    	    }
    	}
      // check if matrix is not empty

      if (a_geiger_matrix.is_empty()) {}
	// {
	//   std::clog << "********************************************************************************" << std::endl;
    	//   std::clog << "******************** Display Tracker trigger info @ 1600 ns ********************" << std::endl;
    	//   std::clog << "******************** Clocktick 1600 ns = " << clocktick_1600ns_ << " ******************** " << std::endl << std::endl;
	//   std::clog << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
	//   for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	//     {
	//       std::clog << "Side = " << iside << " | ";
	//       for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	// 	{
	// 	  std::clog << "[" << a_tracker_record.finale_data_per_zone[iside][izone] << "] ";
	// 	} // end of izone
	//       std::clog << std::endl;
	//     }

	//   for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	//     {
	//       std::clog << "ZW pattern     : S" << iside << " : [";
	//       for (unsigned int ibit = 0; ibit < a_tracker_record.zoning_word_pattern[0].size(); ibit++)
	// 	{
	// 	  std::clog << a_tracker_record.zoning_word_pattern[iside][ibit];
	// 	}
	//       std::clog << "] ";
	//     }
	//   std::clog << std::endl;
	//   for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	//     {
	//       std::clog << "ZW near source : S" << iside << " : [";
	//       for (unsigned int ibit = 0; ibit < a_tracker_record.zoning_word_near_source[0].size(); ibit++)
	// 	{
	// 	  std::clog << a_tracker_record.zoning_word_near_source[iside][ibit];
	// 	}
	//       std::clog << "] ";
	//     }
	//   std::clog << std::endl;

	//   std::clog << "Tracker level one decision : [" << a_tracker_record.finale_decision << "]" <<  std::endl << std::endl;
	//   display_matrix();
	// }

      else
    	{
    	  std::clog << "********************************************************************************" << std::endl;
    	  std::clog << "******************** Display Tracker trigger info @ 1600 ns ********************" << std::endl;
    	  std::clog << "******************** Clocktick 1600 ns = " << clocktick_1600ns_ << " ******************** " << std::endl << std::endl;
	  std::clog << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
	  for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	    {
	      std::clog << "Side = " << iside << " | ";
	      for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
		{
		  std::clog << "[" << a_tracker_record.finale_data_per_zone[iside][izone] << "] ";
		} // end of izone
	      std::clog << std::endl;
	    }

	  for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	    {
	      std::clog << "ZW pattern     : S" << iside << " : [";
	      for (unsigned int ibit = 0; ibit < a_tracker_record.zoning_word_pattern[0].size(); ibit++)
		{
		  std::clog << a_tracker_record.zoning_word_pattern[iside][ibit];
		}
	      std::clog << "] ";
	    }
	  std::clog << std::endl;
	  for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	    {
	      std::clog << "ZW near source : S" << iside << " : [";
	      for (unsigned int ibit = 0; ibit < a_tracker_record.zoning_word_near_source[0].size(); ibit++)
		{
		  std::clog << a_tracker_record.zoning_word_near_source[iside][ibit];
		}
	      std::clog << "] ";
	    }
	  std::clog << std::endl;

	  std::clog << "Tracker level one decision : [" << a_tracker_record.finale_decision << "]" <<  std::endl << std::endl;

    	  bool geiger_matrix[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS];
    	  for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
    	    {
    	      for (int jlayer = trigger_info::NLAYERS - 1; jlayer >= 0; jlayer--)
    		{
    		  for (unsigned int krow = 0; krow < trigger_info::NROWS; krow++)
    		    {
    		      geiger_matrix[iside][jlayer][krow] = a_geiger_matrix.matrix[iside][jlayer][krow];
    		    } // end of krow
    		} // end of jlayer
    	    } // end of iside
    	  fill_tracker_trigger_matrix_1600ns(geiger_matrix);
    	  display_matrix();
    	}
      return;
    }

    void trigger_display_manager::display_tracker_trigger_1600ns(trigger_algorithm & a_trigger_algo_)
    {
      if (a_trigger_algo_._tracker_records_.size() == 0) {}
      else{
	auto it_tracker = a_trigger_algo_._tracker_records_.begin();
	for (; it_tracker != a_trigger_algo_._tracker_records_.end(); it_tracker++)
	  {
	    tracker_trigger_algorithm::tracker_record a_tracker_record = *it_tracker;
	    int32_t tracker_clocktick = a_tracker_record.clocktick_1600ns;
	    display_tracker_trigger_1600ns(a_trigger_algo_, tracker_clocktick);
	    reset_tracker_display();
	  }
      }
      return;
    }


    void trigger_display_manager::display_coincidence_trigger_1600ns(trigger_algorithm & a_trigger_algo_, int32_t clocktick_1600ns_)
    {
      DT_THROW_IF(!is_coinc_1600ns(), std::logic_error, "Boolean coinc 1600ns is not activated, it can't display ! ");

      coincidence_trigger_algorithm::coincidence_event_record a_coincidence_record;
      for (unsigned int i = 0; i < a_trigger_algo_._coincidence_records_.size(); i++)
    	{
	  if (clocktick_1600ns_ == a_trigger_algo_._coincidence_records_[i].clocktick_1600ns) a_coincidence_record = a_trigger_algo_._coincidence_records_[i];
	}

      tracker_trigger_algorithm::geiger_matrix  a_geiger_matrix;
      for (unsigned int i = 0; i < a_trigger_algo_._tracker_records_.size(); i++)
    	{
    	  if (clocktick_1600ns_ == a_coincidence_record.clocktick_1600ns
    	      && clocktick_1600ns_ == a_trigger_algo_._tracker_algo_._geiger_matrix_records_[i].clocktick_1600ns)
    	    {
    	      a_geiger_matrix  = a_trigger_algo_._tracker_algo_._geiger_matrix_records_[i];
    	    }
    	}

      std::clog << "************************************************************************************" << std::endl;
      std::clog << "******************** Display Coincidence trigger info @ 1600 ns ********************" << std::endl;
      std::clog << "******************** Clocktick 1600 ns = " <<  clocktick_1600ns_  << " ******************** " << std::endl << std::endl;
      std::clog << "CT |XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl;
      std::clog << a_coincidence_record.clocktick_1600ns << ' ';
      std::clog << a_coincidence_record.xt_info_bitset << ' ';
      std::clog << a_coincidence_record.LTO_gveto << ' ';
      std::clog << a_coincidence_record.total_multiplicity_gveto << ' ';
      std::clog << a_coincidence_record.LTO_side_1 << ' ';
      std::clog << a_coincidence_record.LTO_side_0 << ' ';
      std::clog << a_coincidence_record.total_multiplicity_side_1 << ' ';
      std::clog << a_coincidence_record.total_multiplicity_side_0 << ' ';
      for (int iside = trigger_info::NSIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = trigger_info::NZONES-1; izone >= 0 ; izone--)
      	    {
      	      std::clog << a_coincidence_record.calo_zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << a_coincidence_record.single_side_coinc
    		<< "  |  Threshold total mult : "   << a_coincidence_record.total_multiplicity_threshold
    		<< "  |  Trigger final decision : " << a_coincidence_record.decision  << std::endl;

      std::clog << std::endl;
      std::clog << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      	{
      	  std::clog << "Side = " << iside << " | ";
      	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      	    {
      	      std::clog << "[" << a_coincidence_record.tracker_finale_data_per_zone[iside][izone] << "] ";
      	    } // end of izone
      	  std::clog << std::endl;
      	}
      std::clog << std::endl;

      std::clog << "Coincidence Clocktick : [" << a_coincidence_record.clocktick_1600ns << "]" << std::endl;
      std::clog << "Coincidence zoning : S0 [" << a_coincidence_record.zoning_word[0] << "]" << std::endl;
      std::clog << "                     S1 [" << a_coincidence_record.zoning_word[1] << "]" << std::endl;

      std::clog << "Coincidence trigger mode : [" << a_coincidence_record.trigger_mode << "]" << std::endl;
      std::clog << "Coincidence event record decision  : [" << a_coincidence_record.decision << "]" <<  std::endl << std::endl;

      std::bitset<10> zoning_word[trigger_info::NSIDES];
      for (unsigned int i = 0; i < trigger_info::NSIDES; i++)
    	{
    	  zoning_word[i] = a_coincidence_record.calo_zoning_word[i];
    	}

      bool geiger_matrix[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS];
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      	{
      	  for (int jlayer = trigger_info::NLAYERS - 1; jlayer >= 0; jlayer--)
      	    {
      	      for (unsigned int krow = 0; krow < trigger_info::NROWS; krow++)
      		{
    		  geiger_matrix[iside][jlayer][krow] = a_geiger_matrix.matrix[iside][jlayer][krow];
    		} // end of krow
    	    } // end of jlayer
    	} // end of iside
      fill_coincidence_trigger_matrix_1600ns(zoning_word, geiger_matrix);
      //      fill_calo_trigger_matrix_1600ns(zoning_word);
      // fill_tracker_trigger_matrix_1600ns(geiger_matrix);
      display_matrix();

      return;
    }

    void trigger_display_manager::display_coincidence_trigger_1600ns(trigger_algorithm & a_trigger_algo_)
    {
      if (a_trigger_algo_._coincidence_records_.size() == 0) {}
      else{
	auto it_coinc = a_trigger_algo_._coincidence_records_.begin();
	for (; it_coinc != a_trigger_algo_._coincidence_records_.end(); it_coinc++)
	  {
	    coincidence_trigger_algorithm::coincidence_event_record a_coinc_record = *it_coinc;
	    int32_t coinc_clocktick = a_coinc_record.clocktick_1600ns;
	    display_coincidence_trigger_1600ns(a_trigger_algo_, coinc_clocktick);
	  }
      }

      return;
    }

    void trigger_display_manager::display_decision_trigger()
    {
      DT_THROW_IF(!is_decision_trigger(), std::logic_error, "Boolean decision trigger is not activated, it can't display ! ");
      return;
    }

    void trigger_display_manager::fill_matrix_pattern()
    {
      for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    	{
    	  if (j == 0) _char_matrix_[10][j] = ' ';
    	  else _char_matrix_[10][j] = '_';
    	}

      for (unsigned int i = 0; i < NUMBER_OF_VERTICAL_CHAR; i++)
    	{
    	  for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    	    {
    	      if (i == 0 || i == 20)
    		{
    		  if (j == 0 || j == 1) _char_matrix_[i][j] = ' ';
    		  else if (j == 2 || j == 116) _char_matrix_[i][j] = '|';
    		  else if (j == 3 || j == 12 || j == 24 || j == 36 || j == 48 || j == 60 || j == 71 || j == 83 || j == 95 || j == 107) _char_matrix_[i][j] = '[';
    		  else if (j == 11 || j == 23 || j == 35 || j == 47 || j == 59 || j == 70 || j == 82 || j == 94 || j == 106 || j == 115) _char_matrix_[i][j] = ']';
    		  else _char_matrix_[i][j] = ' ';
    		}

    	      else
    		{
    		  if (j == 0 || j == 1) _char_matrix_[i][j] = ' ';
    		  else if (j == 2 || j == 116) _char_matrix_[i][j] = '|';
    		}
    	    }
    	}
      for (unsigned int i = 1; i < NUMBER_OF_VERTICAL_CHAR - 1; i++)
    	{
    	  for (unsigned int j = 3; j < NUMBER_OF_HORIZONTAL_CHAR - 1; j++)
    	    {
    	      if (i == 10) {}
    	      else _char_matrix_[i][j] = '.';
    	    }
    	}
      return;
    }

    void trigger_display_manager::reset_matrix_pattern()
    {
      fill_matrix_pattern();
      return;
    }

    void trigger_display_manager::reset_calo_display()
    {
      for (unsigned int i = 0; i < NUMBER_OF_VERTICAL_CHAR; i++)
    	{
    	  for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    	    {
    	      if (i == 0 || i == 20)
    		{
    		  if (j == 0 || j == 1) _char_matrix_[i][j] = ' ';
    		  else if (j == 2 || j == 116) _char_matrix_[i][j] = '|';
    		  else if (j == 3 || j == 12 || j == 24 || j == 36 || j == 48 || j == 60 || j == 71 || j == 83 || j == 95 || j == 107) _char_matrix_[i][j] = '[';
    		  else if (j == 11 || j == 23 || j == 35 || j == 47 || j == 59 || j == 70 || j == 82 || j == 94 || j == 106 || j == 115) _char_matrix_[i][j] = ']';
    		  else _char_matrix_[i][j] = ' ';
    		}

    	      else
    		{
    		  if (j == 0 || j == 1) _char_matrix_[i][j] = ' ';
    		  else if (j == 2 || j == 116) _char_matrix_[i][j] = '|';
    		}
    	    }
    	}
      return;
    }

    void trigger_display_manager::reset_tracker_display()
    {
      for (unsigned int i = 1; i < NUMBER_OF_VERTICAL_CHAR - 1; i++)
    	{
    	  for (unsigned int j = 3; j < NUMBER_OF_HORIZONTAL_CHAR - 1; j++)
    	    {
    	      if (i == 10) {}
    	      else _char_matrix_[i][j] = '.';
    	    }
    	}
      return;
    }

    void trigger_display_manager::display_matrix()
    {
      std::clog << "     Zone0      Zone1       Zone2       Zone3       Zone4      Zone5       Zone6       Zone7      Zone 8     Zone9 " << std::endl;
      for (unsigned int i = 0; i < NUMBER_OF_VERTICAL_CHAR; i++)
    	{
    	  for (unsigned int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
    	    {
    	      std::clog << _char_matrix_[i][j];
    	    }
    	  std::clog << std::endl;
    	}
      std::clog << "     Zone0      Zone1       Zone2       Zone3       Zone4      Zone5       Zone6       Zone7      Zone 8     Zone9 " << std::endl;
      std::clog << std::endl;
       }

  } // end of namespace digitization

} // end of namespace snemo
