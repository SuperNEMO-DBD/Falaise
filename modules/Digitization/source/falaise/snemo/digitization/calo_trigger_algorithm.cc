// snemo/digitization/calo_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/calo_ctw.h>

namespace snemo {
  
  namespace digitization {

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
      return;
    }

    void calo_trigger_algorithm::build_calo_level_one_bitsets(const calo_ctw & my_calo_ctw_)
    {
      uint32_t crate_index = my_calo_ctw_.get_geom_id().get(mapping::CRATE_INDEX);
      uint32_t side;
      std::bitset<calo_ctw::ZONING_BITSET_SIZE> ctw_zoning_bitset_word;
      my_calo_ctw_.get_zoning_word(ctw_zoning_bitset_word);
      
      if (crate_index == mapping::MAIN_CALO_SIDE_0_RACK)
	{
	  side = 0;
	  for (int izone = 0; izone < mapping::NUMBER_OF_CALO_TRIGGER_ZONES; izone++)
	    {
	      if (ctw_zoning_bitset_word.test(izone) == true)
		{
		  // _level_one_calo_trigger_info_[side][izone].set(izone, 1);
		}
	    }
	  
	  
	}

      else if (crate_index == mapping::MAIN_CALO_SIDE_1_RACK)
	{
	  side = 1;

	}

      else if (crate_index == mapping::XWALL_CALO_RACK)
	{

	}

      else
	{
	  DT_THROW(std::logic_error, "Crate index '"<< crate_index << "' is not defined, check your value ! ");
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
	  for (int isize = 0; isize < calo_ctw_list_per_clocktick.size(); isize ++)
	    {
	      build_calo_level_one_bitsets(calo_ctw_list_per_clocktick[isize].get());	      
 
	    } // end of isize 
	} // end of iclocktick
      return;
    }
  } // end of namespace digitization

} // end of namespace snemo
