// snemo/digitization/calo_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/calo_ctw.h>
#include <snemo/digitization/calo_ctw_constants.h>

namespace snemo {
  
  namespace digitization {
    
    struct calo_trigger_algorithm::calo_trigger_gate
    {
      std::bitset<CALO_ZONING_PER_SIDE_BITSET_SIZE> calo_zoning_word[mapping::NUMBER_OF_SIDES];
      std::bitset<CALO_LEVEL_ONE_MULT_BITSET_SIZE> total_calo_multiplicity;
    };
    
    const int32_t calo_trigger_algorithm::CALO_LEVEL_ONE_MULT_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::CALO_ZONING_PER_SIDE_BITSET_SIZE;
    
    calo_trigger_algorithm::calo_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
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

    void calo_trigger_algorithm::initialize()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void calo_trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized ! ");
      DT_THROW_IF(_electronic_mapping_ == 0, std::logic_error, "Missing electronic mapping ! " );

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
      reset_trigger_info();
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
      _total_calo_multiplicity_ = 0;
      return;
    }

    void calo_trigger_algorithm::display_calo_trigger_info()
    {
      std::clog << "Level One calo trigger info display : " << std::endl;
      std::clog << "Total multiplicity = " << _total_calo_multiplicity_ << std::endl << std::endl;
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
 
    void calo_trigger_algorithm::build_calo_level_one_bitsets(const calo_ctw & my_calo_ctw_)
    {
      uint32_t crate_index = my_calo_ctw_.get_geom_id().get(mapping::CRATE_INDEX);
      std::bitset<calo::ctw::ZONING_BITSET_SIZE> ctw_zoning_bitset_word;
      my_calo_ctw_.get_zoning_word(ctw_zoning_bitset_word);
      unsigned long calo_ctw_multiplicity  = my_calo_ctw_.get_htm_pc_info();
      
      if (crate_index == mapping::MAIN_CALO_SIDE_0_CRATE || crate_index == mapping::MAIN_CALO_SIDE_1_CRATE)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
	    {
	      if (ctw_zoning_bitset_word.test(izone) == true)
		{
		  _level_one_calo_trigger_info_[crate_index][izone] = true;
		}
	    }
	}

      else if (crate_index == mapping::XWALL_CALO_CRATE)
	{
	  for (int izone = calo::ctw::ZONING_XWALL_BIT0; izone < mapping::NUMBER_OF_XWALL_CALO_TRIGGER_ZONES; izone++)
	    {
	      switch (izone)
		{
		case calo::ctw::ZONING_XWALL_BIT0 :
		  if (ctw_zoning_bitset_word.test(izone) == true)
		    {
		      _level_one_calo_trigger_info_[0][0] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT1 :
		  if (ctw_zoning_bitset_word.test(izone) == true)
		    {
		      _level_one_calo_trigger_info_[0][9] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT2 :
		  if (ctw_zoning_bitset_word.test(izone) == true)
		    {
		      _level_one_calo_trigger_info_[1][9] = true;
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT3 :
		  if (ctw_zoning_bitset_word.test(izone) == true)
		    {
		      _level_one_calo_trigger_info_[1][0] = true;
		    }
		  break;
		default :
		  break;
		}
	    }
	}

      else
	{
	  DT_THROW(std::logic_error, "Crate index '"<< crate_index << "' is not defined, check your value ! ");
	}

      if (_total_calo_multiplicity_ != 0)
	{
	  unsigned long new_multiplicity  = _total_calo_multiplicity_.to_ulong() + calo_ctw_multiplicity; 
	  std::bitset<CALO_LEVEL_ONE_MULT_BITSET_SIZE> temporary_calo_ctw_multiplicity_bitset(new_multiplicity);
	  _total_calo_multiplicity_ = temporary_calo_ctw_multiplicity_bitset;
	}

      else
	{
	  std::bitset<CALO_LEVEL_ONE_MULT_BITSET_SIZE> temporary_calo_ctw_multiplicity_bitset(calo_ctw_multiplicity);
	  _total_calo_multiplicity_ = temporary_calo_ctw_multiplicity_bitset;
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
      for(int32_t iclocktick = calo_ctw_data_.get_clocktick_min(); iclocktick <= calo_ctw_data_.get_clocktick_max(); iclocktick++)
	{
	  std::vector<datatools::handle<calo_ctw> > calo_ctw_list_per_clocktick;
	  calo_ctw_data_.get_list_of_calo_ctw_per_clocktick(iclocktick, calo_ctw_list_per_clocktick);
	  for (int isize = 0; isize < calo_ctw_list_per_clocktick.size(); isize++)
	    {
	      build_calo_level_one_bitsets(calo_ctw_list_per_clocktick[isize].get());	      
	    } // end of isize 
	  std::clog <<"*************************** Clocktick = " << iclocktick << "***************************" << std::endl << std::endl;;
	  display_calo_trigger_info();
	  reset_trigger_info();
	} // end of iclocktick
      return;
    }
  } // end of namespace digitization

} // end of namespace snemo
