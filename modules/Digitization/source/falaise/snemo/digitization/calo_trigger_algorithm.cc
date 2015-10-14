// snemo/digitization/calo_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/calo_ctw.h>
#include <snemo/digitization/calo_ctw_constants.h>

namespace snemo {
  
  namespace digitization {
     
    const int32_t calo_trigger_algorithm::CALO_LEVEL_ONE_MULT_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::CALO_ZONING_PER_SIDE_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::CALO_ZONING_GVETO_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::CALO_INFO_BITSET_SIZE;
    
    calo_trigger_algorithm::calo_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _calo_circular_buffer_depth_ = -1;
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
    
    void calo_trigger_algorithm::set_calo_circular_buffer_depth(unsigned int & calo_circular_buffer_depth_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo circular buffer depth can't be set ! ");
      _calo_circular_buffer_depth_ = calo_circular_buffer_depth_;
      return;
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

      
      DT_THROW_IF(_calo_circular_buffer_depth_ <= 0, std::logic_error, "Calo circular buffer depth value [" << _calo_circular_buffer_depth_ << "] is missing ! ");
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
      _calo_circular_buffer_depth_ = -1;
      reset_trigger_info();
      _calo_gate_circular_buffer_.reset();
      return;
    }
    
    void calo_trigger_algorithm::reset_trigger_info()
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izones = 0; izones < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izones++)
	    {
	      _level_one_calo_trigger_info_[iside][izones] = false;
	    }
	}
      _calo_gveto_info_bitset_.reset();
      _calo_other_info_bitset_.reset();
      _total_calo_multiplicity_for_a_clocktick_ = 0;
      return;
    }

    void calo_trigger_algorithm::display_calo_trigger_info()
    {
      std::clog << "Level One calo trigger info display : " << std::endl;

      std::clog << _calo_other_info_bitset_ << ' ';
      std::clog << _calo_gveto_info_bitset_ << ' ';
      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
	{
	  for (int izone = mapping::NUMBER_OF_CALO_TRIGGER_ZONES-1; izone >= 0 ; izone--)
	    {
	      std::clog << _level_one_calo_trigger_info_[iside][izone];
	    }
	  std::clog << ' ';
	}
      std::clog << _total_calo_multiplicity_for_a_clocktick_ << std::endl << std::endl;
      

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  if (iside == 1)
	    {
	      std::clog << "   |                                                                                                                 |" << std::endl;
	      if (_level_one_calo_trigger_info_[iside][0] == true) std::clog << "[*]|";
	      else std::clog << "[ ]|";
	      std::clog << "                                                                                                                 ";
	      if (_level_one_calo_trigger_info_[iside][9] == true) std::clog << "|[*]" << std::endl;
	      else std::clog << "|[ ]" << std::endl;
	    }
	  std::clog << "   |";
	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
	    {
	      if (izone == 0 || izone == 9) 
		{
		  if (_level_one_calo_trigger_info_[iside][izone] == true) std::clog << "[*******]";
		  else std::clog  << "[       ]";
		}
	      else if (izone == 5) 
		{
		  if (_level_one_calo_trigger_info_[iside][izone] == true) std::clog  << "[*********]";
		  else std::clog  << "[         ]";
		}
	      else 
		{
		  if (_level_one_calo_trigger_info_[iside][izone] == true) std::clog  << "[**********]";
		  else std::clog << "[          ]";
		}
	    } // end of izone
	  std::clog << "|" << std::endl;
	  if (iside == 0)
	    {
	      if (_level_one_calo_trigger_info_[iside][0] == true) std::clog << "[*]|";
	      else std::clog << "[ ]|";
	      std::clog << "                                                                                                                 ";
	      if (_level_one_calo_trigger_info_[iside][9] == true) std::clog << "|[*]" << std::endl;
	      else std::clog << "|[ ]" << std::endl;
	      std::clog << "   |                                                                                                                 |" << std::endl;
	      std::clog << "   |_________________________________________________________________________________________________________________|" << std::endl;
	    }
	  
	} // end of iside
      std::clog << std::endl;
      return;
    }

    void calo_trigger_algorithm::_display_calo_trigger_summary(calo_trigger_record & my_calo_trigger_record_summary_)
    {	 
      std::clog << "Summary : " << my_calo_trigger_record_summary_.clocktick_25ns << ' ';
      std::clog << my_calo_trigger_record_summary_.info_bitset << ' ';
      std::clog << my_calo_trigger_record_summary_.gveto_zoning_word << ' ';      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
      	{
      	  std::clog << my_calo_trigger_record_summary_.calo_zoning_word[iside];
      	  std::clog << ' ';
      	}
      std::clog << my_calo_trigger_record_summary_.total_calo_multiplicity << std::endl << std::endl;	 	
      std::clog << "*************** end of summary bitset ***********" << std::endl << std::endl;
      

      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      	{
      	  if (iside == 1)
      	    {
      	      std::clog << "   |                                                                                                                 |" << std::endl;
      	      if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(ZONE_0_INDEX) == true) std::clog << "[*]|";
      	      else std::clog << "[ ]|";
      	      std::clog << "                                                                                                                 ";
      	      if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(ZONE_9_INDEX) == true) std::clog << "|[*]" << std::endl;
      	      else std::clog << "|[ ]" << std::endl;
      	    }
      	  std::clog << "   |";
      	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9) 
      		{
      		  if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(izone) == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(izone) == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(izone) == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
      	    } // end of izone
      	  std::clog << "|" << std::endl;
      	  if (iside == 0)
      	    {
      	      if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(ZONE_0_INDEX) == true) std::clog << "[*]|";
      	      else std::clog << "[ ]|";
      	      std::clog << "                                                                                                                 ";
      	      if (my_calo_trigger_record_summary_.calo_zoning_word[iside].test(ZONE_9_INDEX) == true) std::clog << "|[*]" << std::endl;
      	      else std::clog << "|[ ]" << std::endl;
      	      std::clog << "   |                                                                                                                 |" << std::endl;
      	      std::clog << "   |_________________________________________________________________________________________________________________|" << std::endl;
      	    }	  
      	} // end of iside
      std::clog << std::endl;

      return;
    }
    
    void calo_trigger_algorithm::build_calo_level_one_bitsets(const calo_ctw & my_calo_ctw_)
    {  
      uint32_t crate_index = my_calo_ctw_.get_geom_id().get(mapping::CRATE_INDEX);  
      DT_THROW_IF(crate_index < 0 || crate_index > 2, std::logic_error, "Crate index '"<< crate_index << "' is not defined, check your value ! ");
      
      std::bitset<calo::ctw::ZONING_BITSET_SIZE> ctw_zoning_bitset_word;
      my_calo_ctw_.get_zoning_word(ctw_zoning_bitset_word);

      unsigned long calo_ctw_multiplicity  = my_calo_ctw_.get_htm_pc_info();
      if (crate_index == mapping::XWALL_CALO_CRATE)
	{
	  for (int izone = calo::ctw::ZONING_XWALL_BIT0; izone < (calo::ctw::ZONING_XWALL_BIT0 + mapping::NUMBER_OF_XWALL_CALO_TRIGGER_ZONES); izone++)
	    {
	      switch (izone)
		{
		case calo::ctw::ZONING_XWALL_BIT0 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {		      
		      _level_one_calo_trigger_info_[0][0] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT1 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      _level_one_calo_trigger_info_[0][9] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT2 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      _level_one_calo_trigger_info_[1][0] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT3 :
		  if (ctw_zoning_bitset_word.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      
		      _level_one_calo_trigger_info_[1][9] = true;
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
	  	  _level_one_calo_trigger_info_[crate_index][izone] = true;
	  	}
	    }  
	}
      
      if (my_calo_ctw_.is_lto()) _calo_other_info_bitset_.set(LT_INFO_BIT, 1);
      if (my_calo_ctw_.is_xt()) _calo_other_info_bitset_.set(XT_INFO_BIT, 1);
      std::bitset<calo::ctw::CONTROL_BITSET_SIZE> calo_control_word;
      my_calo_ctw_.get_control_word(calo_control_word);
      for (int i = 0; i < calo::ctw::CONTROL_BITSET_SIZE; i++)
      	{
      	  if (calo_control_word.test(i) == true)
      	    {
      	      _calo_other_info_bitset_.set(i+CONTROL_INFO_BIT_0);
      	    }
      	}

      if (_total_calo_multiplicity_for_a_clocktick_ != 0)
	{
	  unsigned long new_multiplicity  = _total_calo_multiplicity_for_a_clocktick_.to_ulong() + calo_ctw_multiplicity; 
	  std::bitset<CALO_LEVEL_ONE_MULT_BITSET_SIZE> temporary_calo_ctw_multiplicity_bitset(new_multiplicity);
	  _total_calo_multiplicity_for_a_clocktick_ = temporary_calo_ctw_multiplicity_bitset;
	}

      else
	{
          std::bitset<CALO_LEVEL_ONE_MULT_BITSET_SIZE> temporary_calo_ctw_multiplicity_bitset(calo_ctw_multiplicity);
	  _total_calo_multiplicity_for_a_clocktick_ = temporary_calo_ctw_multiplicity_bitset;
	}	

      return;
    }

    void calo_trigger_algorithm::build_calo_trigger_record_structure(calo_trigger_record & my_calo_trigger_record_)
    {
      my_calo_trigger_record_.total_calo_multiplicity = _total_calo_multiplicity_for_a_clocktick_;
      my_calo_trigger_record_.gveto_zoning_word = _calo_gveto_info_bitset_;
      my_calo_trigger_record_.info_bitset = _calo_other_info_bitset_;
      
      for (int iside = mapping::NUMBER_OF_SIDES-1; iside >= 0; iside--)
	{
	  for (int izone = mapping::NUMBER_OF_CALO_TRIGGER_ZONES-1; izone >= 0 ; izone--)
	    {
	      if (_level_one_calo_trigger_info_[iside][izone] == true)
		{
		  my_calo_trigger_record_.calo_zoning_word[iside].set(izone, 1);
		}
	      else
		{
		  my_calo_trigger_record_.calo_zoning_word[iside].set(izone, 0);
		}
	    }
	}
      _calo_gate_circular_buffer_->push_back(my_calo_trigger_record_); 
    }
    
    void calo_trigger_algorithm::build_calo_trigger_record_summary_structure(calo_trigger_record & my_calo_trigger_record_summary_)
    {
      for (boost::circular_buffer<calo_trigger_record>::iterator it =_calo_gate_circular_buffer_->begin() ; it != _calo_gate_circular_buffer_->end(); it++)
 	{
	  const calo_trigger_record & ctrec = *it; 

 	  my_calo_trigger_record_summary_.clocktick_25ns = ctrec.clocktick_25ns;
 	  if (my_calo_trigger_record_summary_.total_calo_multiplicity.to_ulong() != 0)
 	    {
	      if (my_calo_trigger_record_summary_.total_calo_multiplicity == 3) my_calo_trigger_record_summary_.total_calo_multiplicity = 3;
	      else my_calo_trigger_record_summary_.total_calo_multiplicity = my_calo_trigger_record_summary_.total_calo_multiplicity.to_ulong() + ctrec.total_calo_multiplicity.to_ulong();
 	    }
 	  else
 	    {
 	      my_calo_trigger_record_summary_.total_calo_multiplicity = ctrec.total_calo_multiplicity;
 	    }
	  
	  for (int i = 0; i < mapping::NUMBER_OF_SIDES; i++)
	    {
	      for (int j = 0; j < CALO_ZONING_PER_SIDE_BITSET_SIZE; j++)
		{
		  if (ctrec.calo_zoning_word[i].test(j) == true) my_calo_trigger_record_summary_.calo_zoning_word[i].set(j, 1);
		  if (j < CALO_INFO_BITSET_SIZE && ctrec.info_bitset.test(j) == true) my_calo_trigger_record_summary_.info_bitset.set(j, 1);   

		  if (j < CALO_ZONING_GVETO_BITSET_SIZE && ctrec.gveto_zoning_word.test(j) == true) my_calo_trigger_record_summary_.gveto_zoning_word.set(j, 1);	  
		  
		}
	    }
 	}
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
      _calo_gate_circular_buffer_.reset(new buffer_type(_calo_circular_buffer_depth_));

      for(int32_t iclocktick = calo_ctw_data_.get_clocktick_min(); iclocktick <= calo_ctw_data_.get_clocktick_max(); iclocktick++)
	{
	  std::vector<datatools::handle<calo_ctw> > calo_ctw_list_per_clocktick;
	  calo_ctw_data_.get_list_of_calo_ctw_per_clocktick(iclocktick, calo_ctw_list_per_clocktick);
	  

	  for (int isize = 0; isize < calo_ctw_list_per_clocktick.size(); isize++)
	    {
	      build_calo_level_one_bitsets(calo_ctw_list_per_clocktick[isize].get());
	    } // end of isize 
	  std::clog <<"*************************** Clocktick = " << iclocktick << "***************************" << std::endl << std::endl;

	  calo_trigger_record my_calo_trigger_record;
	  calo_trigger_record my_calo_trigger_record_summary;
	  my_calo_trigger_record.clocktick_25ns = iclocktick;

	  build_calo_trigger_record_structure(my_calo_trigger_record); 
	  build_calo_trigger_record_summary_structure(my_calo_trigger_record_summary);
	  _display_calo_trigger_summary(my_calo_trigger_record_summary);	 

	  reset_trigger_info();
	} // end of iclocktick
      return;
    }
  } // end of namespace digitization

} // end of namespace snemo
