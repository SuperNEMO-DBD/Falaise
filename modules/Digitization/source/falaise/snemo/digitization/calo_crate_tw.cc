// calo_crate_tw.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

// Ourselves:
#include <snemo/digitization/calo_crate_tw.h>

// Third party:
// - Bayeux/datatools:
//#include <datatools/utils.h>
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    //DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_crate_tw, "snemo::digitalization::calo_crate_tw")

    calo_crate_tw::calo_crate_tw()
    {
      _locked_ctw_ = false;
      _clocktick_25ns_ = -1;
      _ctw_ = 0x0;
      return;
    }

    calo_crate_tw::~calo_crate_tw()
    {
      reset();
      return;
    }

    int32_t calo_crate_tw::get_clocktick_25ns() const
    {
      return _clocktick_25ns_;
    }

    void calo_crate_tw::set_clocktick_25ns( int32_t value_ )
    {
      DT_THROW_IF(is_locked_ctw(), std::logic_error, "Clocktick can't be set, calorimeter crate TW is locked !) ");
      if(value_ <= -1)
	{
	  reset_clocktick_25ns();
	}
      else
	{
	  _clocktick_25ns_ = value_;
	  //_store |= STORE_CLOCKTICK_25NS;
	}
      return;
    }

    void calo_crate_tw::reset_clocktick_25ns()
    {
      _clocktick_25ns_ = -1;
      //_store &= ~STORE_CLOCKTICK_25NS;
      return;
    }

    void calo_crate_tw::set_htm_pc_info(unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked_ctw(), std::logic_error, "HTM bits can't be set, calorimeter crate TW is locked ! ");
      switch (multiplicity_)
	{
	case 0 :	  
	  _ctw_.set(HTM_PC_BIT0, 0);
	  _ctw_.set(HTM_PC_BIT1, 0);
	  break;
	  
	case 1 :
	  _ctw_.set(HTM_PC_BIT0, 1);
	  _ctw_.set(HTM_PC_BIT1, 0);
	  break;
	  
	case 2 :
	  _ctw_.set(HTM_PC_BIT0, 0);
	  _ctw_.set(HTM_PC_BIT1, 1);
	  break;
	  
	default :
	  _ctw_.set(HTM_PC_BIT0, 1);
	  _ctw_.set(HTM_PC_BIT1, 1); 
	}
      //_store |= STORE_CTW;
      return;
    }
    
    unsigned int calo_crate_tw::get_htm_pc_info() const
    {
      if(_ctw_.test(HTM_PC_BIT0 == 0 && HTM_PC_BIT1 == 0))
	{      
	  return 0;
	}
      else if(_ctw_.test(HTM_PC_BIT0 == 1 && HTM_PC_BIT1 == 0))
	{
	  return 1;
	}
      else if(_ctw_.test(HTM_PC_BIT0 == 0 && HTM_PC_BIT1 == 1))
	{
	  return 2;
	}
      return 3;
    }

    bool calo_crate_tw::is_htm() const
    {
      return get_htm_pc_info() != 0;
    }
    
    void calo_crate_tw::get_zoning_word(std::bitset<10> & zoning_word_) const
    {
      zoning_word_ = 0x0;
      for (int i = ZONING_BIT0; i <= ZONING_BIT9; i++)
	{
	  if(_ctw_.test(i) == true)
	    {
	      zoning_word_.set(i-ZONING_BIT0,1);
	    }
	}
      return ;
    }

    void calo_crate_tw::set_zoning_word(std::bitset<10> & zoning_word_)
    {
      DT_THROW_IF(is_locked_ctw(), std::logic_error, "Zoning word can't be set, calorimeter crate TW is locked ! ");

      for (int i = 0; i < zoning_word_.size(); i++)
	{
	  if (zoning_word_.test(i) == true)
	    {
	      _ctw_.set(i + ZONING_BIT0,1);
	    }
	  else 
	    {
	      _ctw_.set(i + ZONING_BIT0,0);	      
	    }
	}      
      return;
    }
    
    unsigned int calo_crate_tw::compute_active_zones(std::set<int> & active_zones_ ) const
    {
      unsigned int active_zone_counts = 0;

      for (int i = ZONING_BIT0 ; i <= ZONING_BIT9 ; i++)
	{
	  if(_ctw_.test(i) == true)
	    {
	      active_zones_.insert(i-ZONING_BIT0); 
	      active_zone_counts++;
	    }
	}
      return active_zone_counts;
    }  

    void calo_crate_tw::set_lto_pc_bit(bool value_)
    {
      DT_THROW_IF(is_locked_ctw(), std::logic_error, "LTO bit can't be set, calorimeter crate TW is locked ! ");
      _ctw_.set(LTO_PC_BIT,value_);
      //_store |= STORE_CTW;
      return;
    }
    
    bool calo_crate_tw::is_lto() const
    {
      return _ctw_.test(LTO_PC_BIT);
    }

    void calo_crate_tw::set_xt_pc_bit(bool value_)
    {
      DT_THROW_IF(is_locked_ctw(), std::logic_error, " External bit can't be set, calorimeter crate TW is locked ! ");
      _ctw_.set(XT_PC_BIT,value_);
      //_store |= STORE_CTW;
      return;
    }
    
    bool calo_crate_tw::is_xt() const
    {
      return _ctw_.test(XT_PC_BIT);
    }

    void calo_crate_tw::get_control_word(std::bitset<4> & control_word_) const
    {
      control_word_ = 0x0;
      for (int i = CONTROL_BIT0; i <= CONTROL_BIT3; i++)
	{
	  if(_ctw_.test(i == true))
	    {
	      control_word_.set(i-CONTROL_BIT0,1);
	    }
	}
      return ;
    }

    void calo_crate_tw::set_control_word(std::bitset<4> & control_word_)
    {    
      DT_THROW_IF(is_locked_ctw(), std::logic_error, "Control word can't be set, calorimeter crate TW is locked ! ");
      for (int i = 0; i < control_word_.size(); i++)
	{
	  if (control_word_.test(i) == true)
	    {
	      _ctw_.set(i + CONTROL_BIT0,1);
	    }
	  else 
	    {
	      _ctw_.set(i + CONTROL_BIT0,0);	      
	    }
	}      
      return;
    }

    bool calo_crate_tw::is_locked_ctw() const
    {
      return _locked_ctw_;
    }

    void calo_crate_tw::lock_ctw()
    {
      DT_THROW_IF(is_locked_ctw(), std::logic_error, "Calorimeter crate TW is already locked ! ");
      _check();
      _locked_ctw_ = true;
      return;
    }
    
    void calo_crate_tw::unlock_ctw()
    {
      DT_THROW_IF(!is_locked_ctw(), std::logic_error, "Calorimeter crate TW is already unlocked ! ");
      _locked_ctw_ = false;
      return;
    } 

    void calo_crate_tw::reset_tw_bitset()
    {
      _ctw_ = 0x0;
      //_store &= ~STORE_CTW;
      return;
    }

    bool calo_crate_tw::is_valid() const
    {
      return _clocktick_25ns_ >= 0;
    }

    void calo_crate_tw::reset()
    {
      reset_tw_bitset();
      reset_clocktick_25ns();
      geomtools::base_hit::reset();
      return;
    }

    void calo_crate_tw::tree_dump (std::ostream & out_,
				   const std::string & title_,
				   const std::string & indent_,
				   bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Clock tick (25 ns)  : " << _clocktick_25ns_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "CTW (18 bits) : " << _ctw_  << std::endl;
      return;
    }

    void calo_crate_tw::_check()
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Clocktick is not valid ! ");
    }

  } // end of namespace digitization

} // end of namespace snemo
