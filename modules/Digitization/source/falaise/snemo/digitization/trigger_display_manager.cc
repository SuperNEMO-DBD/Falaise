// snemo/digitization/trigger_display_manager.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/trigger_algorithm.h>

namespace snemo {
  
  namespace digitization {

    const uint8_t trigger_display_manager::NUMBER_OF_HORIZONTAL_CHAR;
    const uint8_t trigger_display_manager::NUMBER_OF_VERTICAL_CHAR;
    
    trigger_display_manager::trigger_display_manager()
    {
      _initialized_      = false;
      _calo_25ns_        = false;
      _calo_1600ns_      = false;
      _tracker_1600ns_   = false;
      _coinc_1600ns_     = false;
      _decision_trigger_ = false;
      
      for (int i = 0; i < NUMBER_OF_HORIZONTAL_CHAR; i++)
	{
	  for (int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
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

    const bool trigger_display_manager::is_calo_25ns() const
    {
      return _calo_25ns_;
    }

    const bool trigger_display_manager::is_calo_1600ns() const
    {
      return _calo_1600ns_; 
    }

    const bool trigger_display_manager::is_tracker_1600ns() const
    {
      return _tracker_1600ns_;
    }

    const bool trigger_display_manager::is_coinc_1600ns() const
    {
      return _coinc_1600ns_;
    }

    const bool trigger_display_manager::is_decision_trigger() const
    {
      return _decision_trigger_;
    }
    
    void trigger_display_manager::display_calo_trigger_25ns(trigger_algorithm & a_trigger_algo_, int vector_position_25ns_) const
    {
      DT_THROW_IF(!is_calo_25ns(), std::logic_error, "Boolean calo 25ns is not activated, it can't display ! ");
      std::clog << "***********************************************************************************************************************************" << std::endl;
      std::clog <<  "******************** Display calorimeter trigger info @ 25 ns ********************" << std::endl;
      std::clog << "******************** Clocktick 25 ns = " <<  a_trigger_algo_._calo_records_[vector_position_25ns_].clocktick_25ns  << " ******************** " << std::endl; 
      std::clog << "CT |XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl; 
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].clocktick_25ns << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].xt_info_bitset << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].LTO_gveto << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].total_multiplicity_gveto << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].LTO_side_1 << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].LTO_side_0 << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].total_multiplicity_side_1 << ' ';
      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].total_multiplicity_side_0 << ' ';
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = mapping::NUMBER_OF_TRIGGER_ZONES-1; izone >= 0 ; izone--)
      	    {
      	      std::clog << a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}      
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << a_trigger_algo_._calo_records_[vector_position_25ns_].single_side_coinc 
		<< "  |  Threshold total mult : "   << a_trigger_algo_._calo_records_[vector_position_25ns_].total_multiplicity_threshold 
		<< "  |  Trigger final decision : " << a_trigger_algo_._calo_records_[vector_position_25ns_].calo_finale_decision  << std::endl;
      
      std::clog << std::endl;

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      	{
      	  if (iside == 1)
      	    {
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][0] == true) std::clog << "   |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][9] == true) std::clog << "| Side 1" << std::endl;
      	      else std::clog << "| Side 1" << std::endl;
      	    }
      	  if (iside == 0) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  std::clog << " |";
      	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9) 
      		{
      		  if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][izone] == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][izone] == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][izone] == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
      	    } // end of izone
      	  std::clog << "|" << std::endl;
      	  if (iside == 1) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  if (iside == 0)
      	    {
      	      if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][0] == true) std::clog << " |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (a_trigger_algo_._calo_records_[vector_position_25ns_].zoning_word[iside][9] == true) std::clog << "| Side 0" << std::endl;
      	      else std::clog << "| Side 0" << std::endl;
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      std::clog << " |-----------------------------------------------------------------------------------------------------------------| Source foil" << std::endl;
      	    }	  
      	} // end of iside 
      std::clog << "***********************************************************************************************************************************" << std::endl;
      std::clog << std::endl << std::endl;
      return;
    }

    void trigger_display_manager::display_calo_trigger_25ns(trigger_algorithm & a_trigger_algo_) const
    {
      for (int ipos = 0; ipos < a_trigger_algo_._calo_records_.size(); ipos++)
	{
	  display_calo_trigger_25ns(a_trigger_algo_, ipos);
	}
      return;	  
  }

    void trigger_display_manager::display_calo_trigger_1600ns(trigger_algorithm & a_trigger_algo_, int vector_position_1600ns_) const
    {
      DT_THROW_IF(!is_calo_1600ns(), std::logic_error, "Boolean calo 1600ns is not activated, it can't display ! ");
      std::clog << "***********************************************************************************************************************************" << std::endl;
      std::clog <<  "******************** Display calorimeter trigger info @ 1600 ns ********************" << std::endl;
      std::clog << "******************** Clocktick 1600 ns = " <<  a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].clocktick_1600ns  << " ******************** " << std::endl; 
      std::clog << "CT |XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl; 
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].clocktick_1600ns << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].xt_info_bitset << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].LTO_gveto << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].total_multiplicity_gveto << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].LTO_side_1 << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].LTO_side_0 << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].total_multiplicity_side_1 << ' ';
      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].total_multiplicity_side_0 << ' ';
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = mapping::NUMBER_OF_TRIGGER_ZONES-1; izone >= 0 ; izone--)
      	    {
      	      std::clog << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}      
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].single_side_coinc 
		<< "  |  Threshold total mult : "   << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].total_multiplicity_threshold 
		<< "  |  Trigger final decision : " << a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].calo_finale_decision  << std::endl;
      
      std::clog << std::endl;

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      	{
      	  if (iside == 1)
      	    {
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][0] == true) std::clog << "   |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][9] == true) std::clog << "| Side 1" << std::endl;
      	      else std::clog << "| Side 1" << std::endl;
      	    }
      	  if (iside == 0) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  std::clog << " |";
      	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9) 
      		{
      		  if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][izone] == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][izone] == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][izone] == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
      	    } // end of izone
      	  std::clog << "|" << std::endl;
      	  if (iside == 1) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  if (iside == 0)
      	    {
      	      if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][0] == true) std::clog << " |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (a_trigger_algo_._coinc_algo_._coincidence_calo_records_[vector_position_1600ns_].zoning_word[iside][9] == true) std::clog << "| Side 0" << std::endl;
      	      else std::clog << "| Side 0" << std::endl;
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      std::clog << " |-----------------------------------------------------------------------------------------------------------------| Source foil" << std::endl;
      	    }	  
      	} // end of iside 
      std::clog << "***********************************************************************************************************************************" << std::endl;
      std::clog << std::endl << std::endl;
      return;
    }

    void trigger_display_manager::display_calo_trigger_1600ns(trigger_algorithm & a_trigger_algo_) const
    {
      for (int ipos = 0; ipos < a_trigger_algo_._coinc_algo_._coincidence_calo_records_.size(); ipos++)
	{
	  display_calo_trigger_1600ns(a_trigger_algo_, ipos);
	} 
      return;
    }
			
    void trigger_display_manager::display_tracker_trigger_1600ns(trigger_algorithm & a_trigger_algo_, int vector_position_1600ns_) const
    {
      DT_THROW_IF(!is_tracker_1600ns(), std::logic_error, "Boolean tracker 1600ns is not activated, it can't display ! ");
      
      std::clog << "Tracker Trigger info record : " << std::endl; 
      std::clog << "Clocktick 1600    : " << a_trigger_algo_._tracker_records_[vector_position_1600ns_].clocktick_1600ns << std::endl;
      std::clog << "         | VOID = 00 | SHORT TRACK = 11 | LONG TRACK = 01" << std::endl;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	  std::clog << "[" << a_trigger_algo_._tracker_records_[vector_position_1600ns_].final_tracker_trigger_info[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	}
      std::clog << "Level one decision : [" << a_trigger_algo_._tracker_records_[vector_position_1600ns_].level_one_finale_decision << "]" <<  std::endl;
      
      std::clog << "  |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl;

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  if (iside == 0)
	    {
	      for (int jlayer = mapping::GEIGER_LAYERS_SIZE - 1; jlayer >= 0; jlayer--) // Value GEIGER_LAYER_SIZE = 9
		{
		  std::clog << jlayer << ' ';
		  for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		    {
		      if( krow == 0 )        std::clog<<"|";
		  
		      if (a_trigger_algo_._tracker_records_[vector_position_1600ns_].geiger_matrix[iside][jlayer][krow] ) std::clog << "*";
		  
		      if(!a_trigger_algo_._tracker_records_[vector_position_1600ns_].geiger_matrix[iside][jlayer][krow])  std::clog << ".";	  

		      if( krow == 112)     std::clog<<"|";

		    } // end of row loop
		  std::clog<<std::endl;	

		  if (jlayer == 0)
		    {
		      std::clog << "  |_________________________________________________________________________________________________________________|" << std::endl;
		    }

		} // end of layer loop

	    } // end of if == 0

	  if (iside == 1)
	    {  
	      for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
		{
		  std::clog << jlayer << ' ' ;
		  for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		    {
		      if( krow == 0 )        std::clog<<"|";
		  
		      if (a_trigger_algo_._tracker_records_[vector_position_1600ns_].geiger_matrix[iside][jlayer][krow] ) std::clog << "*";
		  
		      if(!a_trigger_algo_._tracker_records_[vector_position_1600ns_].geiger_matrix[iside][jlayer][krow])  std::clog << ".";	  

		      if( krow == 112)     std::clog<<"|";

		    } // end of krow
		  std::clog<<std::endl;	    
  
		} // end of jlayer

	    } // end of if iside==1

	} // end of iside

      std::clog << "  |-0-1-2-3-4-5-6-7-8-9-1-2-3-4-5-6-7-8-9-0-1-2-3-4-5-6-7-89-1-2-3-4-5-6-7-8-9-0-1-2-3-4-5-6-7-8-9-1-2-3-4-5-6-7-8-9| Board IDs " << std::endl;

      std::clog << "  |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl;
      std::clog << "  |                                     |                                    |                                      |" << std::endl;
      std::clog << "  |---------------Crate-0---------------|--------------Crate-1---------------|---------------Crate-2----------------|" << std::endl;
      std::clog << "  |                                     |                                    |                                      |" << std::endl;
      std::clog << std::endl;
      
      return;
    }

    void trigger_display_manager::display_tracker_trigger_1600ns(trigger_algorithm & a_trigger_algo_) const
    {
      for (int ipos = 0; ipos < a_trigger_algo_._tracker_records_.size(); ipos++)
	{
	  display_tracker_trigger_1600ns(a_trigger_algo_, ipos);
	} 
      return;
    }

    void trigger_display_manager::display_coincidence_trigger_1600ns() const
    {
      DT_THROW_IF(!is_coinc_1600ns(), std::logic_error, "Boolean coinc 1600ns is not activated, it can't display ! ");
      return;
    }

    void trigger_display_manager::display_decision_trigger() const			
    {
      DT_THROW_IF(!is_decision_trigger(), std::logic_error, "Boolean decision trigger is not activated, it can't display ! ");
      return;
    }
    
    void trigger_display_manager::fill_matrix()
    {
      for (int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
	{
	  _char_matrix_[11][j] = '_';
	}
      return;
    }
    
    void trigger_display_manager::display_matrix()
    {
      fill_matrix();
      for (int i = 0; i < NUMBER_OF_VERTICAL_CHAR; i++)
	{
	  for (int j = 0; j < NUMBER_OF_HORIZONTAL_CHAR; j++)
	    {
	      std::clog << _char_matrix_[i][j];
	    }
	  std::clog << std::endl;
	}
      std::clog << std::endl;
    }
    
  } // end of namespace digitization

} // end of namespace snemo
