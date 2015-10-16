// snemo/digitization/calo_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/calo_ctw.h>

namespace snemo {
  
  namespace digitization {
     
    const int32_t calo_trigger_algorithm::LEVEL_ONE_MULT_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::ZONING_PER_SIDE_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::ZONING_GVETO_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::INFO_BITSET_SIZE;
    
    calo_trigger_algorithm::calo_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _circular_buffer_depth_ = -1;
      _activated_back_to_back_coinc_ = 0;
      _activated_single_side_coinc_ = 0;
      return;
    }

    calo_trigger_algorithm::~calo_trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void calo_trigger_algorithm::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }
    
    void calo_trigger_algorithm::set_circular_buffer_depth(unsigned int & circular_buffer_depth_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo circular buffer depth can't be set ! ");
      _circular_buffer_depth_ = circular_buffer_depth_;
      return;
    }
    
    void calo_trigger_algorithm::set_back_to_back_coinc()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, boolean back to back coinc can't be set ! ");      
      _activated_back_to_back_coinc_ = true;
      return;
    }    

    bool calo_trigger_algorithm::is_activated_back_to_back_coinc() const
    {
      return _activated_back_to_back_coinc_;
    }

    void calo_trigger_algorithm::set_single_side_coinc()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, boolean single side coinc can't be set ! ");
      _activated_single_side_coinc_ = true;
      return;
    }     

    bool calo_trigger_algorithm::is_activated_single_side_coinc() const
    {
      return _activated_single_side_coinc_;
    }

    void calo_trigger_algorithm::set_threshold_total_multiplicity_coinc(unsigned int & threshold_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo threshold can't be set ! ");
      _threshold_total_multiplicity_ = threshold_;
      return;
    }

    const std::bitset<calo::ctw::HTM_BITSET_SIZE> calo_trigger_algorithm::get_threshold_total_multiplicity_coinc() const
    {
      return _threshold_total_multiplicity_;
    }

    void calo_trigger_algorithm::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void calo_trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized ! ");
      DT_THROW_IF(_electronic_mapping_ == 0, std::logic_error, "Missing electronic mapping ! " );
      DT_THROW_IF(_circular_buffer_depth_ <= 0, std::logic_error, "Calo circular buffer depth value [" << _circular_buffer_depth_ << "] is missing ! ");
      _initialized_ = true;
      return;
    }

    bool calo_trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void calo_trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _activated_back_to_back_coinc_ = 0;
      _activated_single_side_coinc_ = 0;
      _circular_buffer_depth_ = -1;
      reset_trigger_info();
      _gate_circular_buffer_.reset();
      return;
    }
    
    void calo_trigger_algorithm::reset_trigger_info()
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izones = 0; izones < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izones++)
	    {
	      _level_one_trigger_info_[iside][izones] = false;
	    }
	}
      _gveto_info_bitset_.reset();
      _other_info_bitset_.reset();
      _total_multiplicity_for_a_clocktick_ = 0;
      return;
    }

    const calo_trigger_algorithm::trigger_summary_record calo_trigger_algorithm::get_calo_level_1_finale_decision() const
    {
      return _calo_level_1_finale_decision_;
    }

    void calo_trigger_algorithm::display_trigger_info()
    {
      std::clog << "Level One calo trigger info display : " << std::endl;

      std::clog << _other_info_bitset_ << ' ';
      std::clog << _gveto_info_bitset_ << ' ';
      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
	{
	  for (int izone = mapping::NUMBER_OF_CALO_TRIGGER_ZONES-1; izone >= 0 ; izone--)
	    {
	      std::clog << _level_one_trigger_info_[iside][izone];
	    }
	  std::clog << ' ';
	}
      std::clog << _total_multiplicity_for_a_clocktick_ << std::endl << std::endl;
      

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  if (iside == 1)
	    {
	      std::clog << "   |                                                                                                                 |" << std::endl;
	      if (_level_one_trigger_info_[iside][0] == true) std::clog << "[*]|";
	      else std::clog << "[ ]|";
	      std::clog << "                                                                                                                 ";
	      if (_level_one_trigger_info_[iside][9] == true) std::clog << "|[*]" << std::endl;
	      else std::clog << "|[ ]" << std::endl;
	    }
	  std::clog << "   |";
	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
	    {
	      if (izone == 0 || izone == 9) 
		{
		  if (_level_one_trigger_info_[iside][izone] == true) std::clog << "[*******]";
		  else std::clog  << "[       ]";
		}
	      else if (izone == 5) 
		{
		  if (_level_one_trigger_info_[iside][izone] == true) std::clog  << "[*********]";
		  else std::clog  << "[         ]";
		}
	      else 
		{
		  if (_level_one_trigger_info_[iside][izone] == true) std::clog  << "[**********]";
		  else std::clog << "[          ]";
		}
	    } // end of izone
	  std::clog << "|" << std::endl;
	  if (iside == 0)
	    {
	      if (_level_one_trigger_info_[iside][0] == true) std::clog << "[*]|";
	      else std::clog << "[ ]|";
	      std::clog << "                                                                                                                 ";
	      if (_level_one_trigger_info_[iside][9] == true) std::clog << "|[*]" << std::endl;
	      else std::clog << "|[ ]" << std::endl;
	      std::clog << "   |                                                                                                                 |" << std::endl;
	      std::clog << "   |_________________________________________________________________________________________________________________|" << std::endl;
	    }
	  
	} // end of iside
      std::clog << std::endl;
      return;
    }

    void calo_trigger_algorithm::_display_trigger_summary(trigger_summary_record & my_trigger_summary_record_)
    {	 
      if (_activated_back_to_back_coinc_ == true) _compute_back_to_back_decision(my_trigger_summary_record_);
      if (_activated_single_side_coinc_ == true) _compute_single_side_decision(my_trigger_summary_record_);
      if (_threshold_total_multiplicity_ != 0) _compute_threshold_total_multiplicity_decision(my_trigger_summary_record_);
      _compute_trigger_finale_decision(my_trigger_summary_record_);
      if (my_trigger_summary_record_.trigger_finale_decision == true) _calo_level_1_finale_decision_ = my_trigger_summary_record_;
      
      std::clog << "Summary : " << my_trigger_summary_record_.clocktick_25ns << ' ';
      std::clog << my_trigger_summary_record_.info_bitset << ' ';
      std::clog << my_trigger_summary_record_.gveto_zoning_word << ' ';      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
      	{
      	  std::clog << my_trigger_summary_record_.zoning_word[iside];
      	  std::clog << ' ';
      	}
      std::clog << my_trigger_summary_record_.total_multiplicity << std::endl;
      std::clog << "b2b bit : " << my_trigger_summary_record_.back_to_back_coinc << " || ss bit : " << my_trigger_summary_record_.single_side_coinc << " || HT bit : " << my_trigger_summary_record_.threshold_total_multiplicity << " || Fin bit : " << my_trigger_summary_record_.trigger_finale_decision << std::endl << std::endl;
      


      std::clog << "Finale decision : " << _calo_level_1_finale_decision_.clocktick_25ns << ' ';
      std::clog << _calo_level_1_finale_decision_.info_bitset << ' ';
      std::clog << _calo_level_1_finale_decision_.gveto_zoning_word << ' ';      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
      	{
      	  std::clog << _calo_level_1_finale_decision_.zoning_word[iside];
      	  std::clog << ' ';
      	}
      std::clog << _calo_level_1_finale_decision_.total_multiplicity << std::endl;
      std::clog << _calo_level_1_finale_decision_.back_to_back_coinc << _calo_level_1_finale_decision_.single_side_coinc << _calo_level_1_finale_decision_.threshold_total_multiplicity << _calo_level_1_finale_decision_.trigger_finale_decision << std::endl << std::endl;

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      	{
      	  if (iside == 1)
      	    {
      	      std::clog << "   |                                                                                                                 |" << std::endl;
      	      if (my_trigger_summary_record_.zoning_word[iside].test(ZONE_0_INDEX) == true) std::clog << "[*]|";
      	      else std::clog << "[ ]|";
      	      std::clog << "                                                                                                                 ";
      	      if (my_trigger_summary_record_.zoning_word[iside].test(ZONE_9_INDEX) == true) std::clog << "|[*]" << std::endl;
      	      else std::clog << "|[ ]" << std::endl;
      	    }
      	  std::clog << "   |";
      	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9) 
      		{
      		  if (my_trigger_summary_record_.zoning_word[iside].test(izone) == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (my_trigger_summary_record_.zoning_word[iside].test(izone) == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (my_trigger_summary_record_.zoning_word[iside].test(izone) == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
      	    } // end of izone
      	  std::clog << "|" << std::endl;
      	  if (iside == 0)
      	    {
      	      if (my_trigger_summary_record_.zoning_word[iside].test(ZONE_0_INDEX) == true) std::clog << "[*]|";
      	      else std::clog << "[ ]|";
      	      std::clog << "                                                                                                                 ";
      	      if (my_trigger_summary_record_.zoning_word[iside].test(ZONE_9_INDEX) == true) std::clog << "|[*]" << std::endl;
      	      else std::clog << "|[ ]" << std::endl;
      	      std::clog << "   |                                                                                                                 |" << std::endl;
      	      std::clog << "   |_________________________________________________________________________________________________________________|" << std::endl;
      	    }	  
      	} // end of iside
      std::clog << std::endl;

      return;
    }
    
    void calo_trigger_algorithm::build_level_one_bitsets(const calo_ctw & my_calo_ctw_)
    {  
      uint32_t crate_index = my_calo_ctw_.get_geom_id().get(mapping::CRATE_INDEX);  
      DT_THROW_IF(crate_index < 0 || crate_index > 2, std::logic_error, "Crate index '"<< crate_index << "' is not defined, check your value ! ");
      
      std::bitset<calo::ctw::ZONING_BITSET_SIZE> ctw_zoning_bitset_word;
      my_calo_ctw_.get_zoning_word(ctw_zoning_bitset_word);

      unsigned long ctw_multiplicity  = my_calo_ctw_.get_htm_pc_info();
      if (crate_index == mapping::XWALL_CALO_CRATE)
	{
	  for (int izone = calo::ctw::ZONING_XWALL_BIT0; izone < (calo::ctw::ZONING_XWALL_BIT0 + mapping::NUMBER_OF_XWALL_CALO_TRIGGER_ZONES); izone++)
	    {
	      switch (izone)
		{
		case calo::ctw::ZONING_XWALL_BIT0 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {		      
		      _level_one_trigger_info_[0][0] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT1 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      _level_one_trigger_info_[0][9] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT2 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      _level_one_trigger_info_[1][0] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT3 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {		      
		      _level_one_trigger_info_[1][9] = true;
		    }
		  break;
		default :
		  break;
		}
	    }
	}

      else // (crate_index == mapping::MAIN_CALO_SIDE_0_CRATE || crate_index == mapping::MAIN_CALO_SIDE_1_CRATE)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
	    {
	      if (ctw_zoning_bitset_word.test(izone) == true)
	  	{
	  	  _level_one_trigger_info_[crate_index][izone] = true;
	  	}
	    }  
	}
      
      if (my_calo_ctw_.is_lto()) _other_info_bitset_.set(LT_INFO_BIT, 1);
      if (my_calo_ctw_.is_xt()) _other_info_bitset_.set(XT_INFO_BIT, 1);
      std::bitset<calo::ctw::CONTROL_BITSET_SIZE> control_word;
      my_calo_ctw_.get_control_word(control_word);
      for (int i = 0; i < calo::ctw::CONTROL_BITSET_SIZE; i++)
      	{
      	  if (control_word.test(i) == true)
      	    {
      	      _other_info_bitset_.set(i+CONTROL_INFO_BIT_0);
      	    }
      	}

      if (_total_multiplicity_for_a_clocktick_ != 0)
	{
	  unsigned long new_multiplicity  = _total_multiplicity_for_a_clocktick_.to_ulong() + ctw_multiplicity; 
	  std::bitset<calo::ctw::HTM_BITSET_SIZE> temporary_ctw_multiplicity_bitset(new_multiplicity);
	  _total_multiplicity_for_a_clocktick_ = temporary_ctw_multiplicity_bitset;
	}

      else
	{
          std::bitset<calo::ctw::HTM_BITSET_SIZE> temporary_ctw_multiplicity_bitset(ctw_multiplicity);
	  _total_multiplicity_for_a_clocktick_ = temporary_ctw_multiplicity_bitset;
	}	

      return;
    }    

    void calo_trigger_algorithm::_build_trigger_record_structure(trigger_record & my_trigger_record_)
    {
      my_trigger_record_.total_multiplicity = _total_multiplicity_for_a_clocktick_;
      my_trigger_record_.gveto_zoning_word = _gveto_info_bitset_;
      my_trigger_record_.info_bitset = _other_info_bitset_;
      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
	{
	  for (int izone = mapping::NUMBER_OF_CALO_TRIGGER_ZONES-1; izone >= 0 ; izone--)
	    {
	      if (_level_one_trigger_info_[iside][izone] == true)
		{
		  my_trigger_record_.zoning_word[iside].set(izone, 1);
		}
	      else
		{
		  my_trigger_record_.zoning_word[iside].set(izone, 0);
		}
	    }
	}
      _gate_circular_buffer_->push_back(my_trigger_record_); 
    }
    
    void calo_trigger_algorithm::_build_trigger_record_summary_structure(trigger_summary_record & my_trigger_summary_record_)
    {
      for (boost::circular_buffer<trigger_record>::iterator it =_gate_circular_buffer_->begin() ; it != _gate_circular_buffer_->end(); it++)
 	{
	  const trigger_record & ctrec = *it; 

 	  my_trigger_summary_record_.clocktick_25ns = ctrec.clocktick_25ns;
 	  if (my_trigger_summary_record_.total_multiplicity.to_ulong() != 0)
 	    {
	      if (my_trigger_summary_record_.total_multiplicity == 3) my_trigger_summary_record_.total_multiplicity = 3;
	      else my_trigger_summary_record_.total_multiplicity = my_trigger_summary_record_.total_multiplicity.to_ulong() + ctrec.total_multiplicity.to_ulong();
 	    }
 	  else
 	    {
 	      my_trigger_summary_record_.total_multiplicity = ctrec.total_multiplicity;
 	    }
	  
	  for (int i = 0; i < mapping::NUMBER_OF_SIDES; i++)
	    {
	      for (int j = 0; j < ZONING_PER_SIDE_BITSET_SIZE; j++)
		{
		  if (ctrec.zoning_word[i].test(j) == true) my_trigger_summary_record_.zoning_word[i].set(j, 1);
		  if (j < INFO_BITSET_SIZE && ctrec.info_bitset.test(j) == true) my_trigger_summary_record_.info_bitset.set(j, 1);   

		  if (j < ZONING_GVETO_BITSET_SIZE && ctrec.gveto_zoning_word.test(j) == true) my_trigger_summary_record_.gveto_zoning_word.set(j, 1);	  
		  
		}
	    }
 	}
    }

    void calo_trigger_algorithm::_compute_back_to_back_decision(trigger_summary_record & my_trigger_summary_record_)
    {
      DT_THROW_IF(!is_activated_back_to_back_coinc(), std::logic_error, "Boolean back to back coincidence is not activated, it can't compute the decision ! ");
      bool zoning_word_0_any = my_trigger_summary_record_.zoning_word[SIDE_0_INDEX].any();
      bool zoning_word_1_any = my_trigger_summary_record_.zoning_word[SIDE_1_INDEX].any();
      unsigned int multiplicity = my_trigger_summary_record_.total_multiplicity.to_ulong();
     
      std::clog << "Back to back decision : " << zoning_word_0_any << ' ' <<  zoning_word_1_any << ' ' << multiplicity << std::endl;      
      
      if (zoning_word_0_any == true && zoning_word_1_any == true)
	{
	  my_trigger_summary_record_.back_to_back_coinc = true;
	}
      return;
    }

    void calo_trigger_algorithm::_compute_single_side_decision(trigger_summary_record & my_trigger_summary_record_)
    {
      DT_THROW_IF(!is_activated_single_side_coinc(), std::logic_error, "Boolean single side coincidence is not activated, it can't compute the decision ! ");
      
      int pm_side_0 = my_trigger_summary_record_.zoning_word[SIDE_0_INDEX].count();
      int pm_side_1 = my_trigger_summary_record_.zoning_word[SIDE_1_INDEX].count();
      unsigned int multiplicity = my_trigger_summary_record_.total_multiplicity.to_ulong();
     
      std::clog << "Single side decision : " << pm_side_0 << ' ' <<  pm_side_1 << ' ' << multiplicity << std::endl;      
      
      if (pm_side_0 > 1 || pm_side_1 > 1)
	{
	  my_trigger_summary_record_.single_side_coinc = true;
	}
      return;
    }

    void calo_trigger_algorithm::_compute_threshold_total_multiplicity_decision(trigger_summary_record & my_trigger_summary_record_)
    {
      DT_THROW_IF(_threshold_total_multiplicity_ == 0, std::logic_error, "Calo threshold is not set, it can't compute the decision ! ");
      
      unsigned int multiplicity = my_trigger_summary_record_.total_multiplicity.to_ulong();
      
      if (multiplicity >= _threshold_total_multiplicity_.to_ulong())
	{
	  my_trigger_summary_record_.threshold_total_multiplicity = true;
	}
      
      return;
    }

    void calo_trigger_algorithm::_compute_trigger_finale_decision(trigger_summary_record & my_trigger_summary_record_)
    {

      if (my_trigger_summary_record_.back_to_back_coinc == true || 
	  my_trigger_summary_record_.single_side_coinc == true || 
	  my_trigger_summary_record_.threshold_total_multiplicity == true )
	{
	  my_trigger_summary_record_.trigger_finale_decision = true;
	}

      return;      
    }

    void calo_trigger_algorithm::process(const calo_ctw_data & calo_ctw_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo trigger algorithm is not initialized, it can't process ! ");
      _process(calo_ctw_data_);
      return;
    }

    void calo_trigger_algorithm::_process(const calo_ctw_data & calo_ctw_data_)
    { 
      reset_trigger_info();
      _gate_circular_buffer_.reset(new buffer_type(_circular_buffer_depth_));

      for(int32_t iclocktick = calo_ctw_data_.get_clocktick_min(); iclocktick <= calo_ctw_data_.get_clocktick_max(); iclocktick++)
	{
	  std::vector<datatools::handle<calo_ctw> > ctw_list_per_clocktick;
	  calo_ctw_data_.get_list_of_calo_ctw_per_clocktick(iclocktick, ctw_list_per_clocktick);
	  
	  for (int isize = 0; isize < ctw_list_per_clocktick.size(); isize++)
	    {
	      build_level_one_bitsets(ctw_list_per_clocktick[isize].get());
	    } // end of isize 
	  std::clog <<"*************************** Clocktick = " << iclocktick << "***************************" << std::endl << std::endl;

	  trigger_record my_trigger_record;
	  trigger_summary_record my_trigger_summary_record;
	  my_trigger_record.clocktick_25ns = iclocktick;

	  _build_trigger_record_structure(my_trigger_record); 
	  _build_trigger_record_summary_structure(my_trigger_summary_record);
	  _display_trigger_summary(my_trigger_summary_record);

	  reset_trigger_info();
	} // end of iclocktick
      return;
    }
  } // end of namespace digitization
} // end of namespace snemo
