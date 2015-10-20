// snemo/digitization/calo_ctw.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_ctw.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project :
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_ctw, "snemo::digitalization::calo_ctw")

    calo_ctw::calo_ctw()
    {
      _locked_ = false;
      _layout_ = calo::ctw::LAYOUT_UNDEFINED;
      _clocktick_25ns_ = clock_utils::INVALID_CLOCKTICK;
      _ctw_ = 0x0;
      return;
    }

    calo_ctw::~calo_ctw()
    {
      reset();
      return;
    }

    void calo_ctw::set_header(int32_t hit_id_,
			      const geomtools::geom_id & electronic_id_,
			      int32_t clocktick_25ns_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter CTW is locked !) ");
      set_hit_id(hit_id_);
      set_geom_id(electronic_id_);
      set_clocktick_25ns(clocktick_25ns_);

      unsigned int crate_id = electronic_id_.get(mapping::CRATE_DEPTH);
      if (crate_id == mapping::MAIN_CALO_SIDE_0_CRATE || crate_id == mapping::MAIN_CALO_SIDE_1_CRATE) 
	{
	  _layout_ = calo::ctw::LAYOUT_MAIN_WALL;
	}
      else if (crate_id == mapping::XWALL_GVETO_CALO_CRATE)
	{
	  _layout_ = calo::ctw::LAYOUT_XWALL_GVETO; 
	}
      else 
	{
	  _layout_ = calo::ctw::LAYOUT_UNDEFINED;
	}
      return;
    }
    
    bool calo_ctw::is_main_wall() const
    {
      if (_layout_ == calo::ctw::LAYOUT_MAIN_WALL) return true;
      else return false;
    }

    calo::ctw::layout calo_ctw::get_layout() const
    {
      return _layout_;
    }
    
    int32_t calo_ctw::get_clocktick_25ns() const
    {
      return _clocktick_25ns_;
    }

    void calo_ctw::set_clocktick_25ns(int32_t value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be set, calorimeter crate TW is locked !) ");
      if(value_ <= clock_utils::INVALID_CLOCKTICK)
	{
	  reset_clocktick_25ns();
	}
      else
	{
	  _clocktick_25ns_ = value_;
	  _store |= STORE_CLOCKTICK_25NS;
	}
      return;
    }

    void calo_ctw::reset_clocktick_25ns()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be reset, calorimeter crate TW is locked !) ");
      _clocktick_25ns_ = clock_utils::INVALID_CLOCKTICK;
      _store &= ~STORE_CLOCKTICK_25NS;
      return;
    }

    void calo_ctw::set_htm_main_wall(unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "HTM bits can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(multiplicity_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::logic_error, "Multiplicity value ["<< multiplicity_ << "] is not valid ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_XWALL_GVETO, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");

      switch (multiplicity_)
	{
	case 0 :	  
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT1, 0);
	  break;
	  
	case 1 :
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT1, 0);
	  break;
	  
	case 2 :
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT1, 1);
	  break;
	  
	default :
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_MAIN_WALL_BIT1, 1); 
	  break;
	}
      _store |= STORE_CTW;
      return;
    }
    
    unsigned int calo_ctw::get_htm_main_wall_info() const
    {
      if(_ctw_.test(calo::ctw::HTM_MAIN_WALL_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_MAIN_WALL_BIT1) == 0)
	{      
	  return 0;
	}
      else if(_ctw_.test(calo::ctw::HTM_MAIN_WALL_BIT0) == 1 && _ctw_.test(calo::ctw::HTM_MAIN_WALL_BIT1) == 0)
	{
	  return 1;
	}
      else if(_ctw_.test(calo::ctw::HTM_MAIN_WALL_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_MAIN_WALL_BIT1) == 1)
	{
	  return 2;
	}
      return 3;
    }

    bool calo_ctw::is_htm_main_wall() const
    {
      return get_htm_main_wall_info() != 0;
    }

    void calo_ctw::set_htm_gveto(unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "HTM bits can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(multiplicity_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::logic_error, "Multiplicity value ["<< multiplicity_ << "] is not valid ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_MAIN_WALL, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");

      switch (multiplicity_)
	{
	case 0 :	  
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT1, 0);
	  break;
	  
	case 1 :
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT1, 0);
	  break;
	  
	case 2 :
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT1, 1);
	  break;
	  
	default :
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_GVETO_BIT1, 1); 
	  break;
	}
      _store |= STORE_CTW;
      return;
    }

    unsigned int calo_ctw::get_htm_gveto_info() const
    {
      if(_ctw_.test(calo::ctw::HTM_GVETO_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_GVETO_BIT1) == 0)
	{      
	  return 0;
	}
      else if(_ctw_.test(calo::ctw::HTM_GVETO_BIT0) == 1 && _ctw_.test(calo::ctw::HTM_GVETO_BIT1) == 0)
	{
	  return 1;
	}
      else if(_ctw_.test(calo::ctw::HTM_GVETO_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_GVETO_BIT1) == 1)
	{
	  return 2;
	}
      return 3;
    } 

    void calo_ctw::set_htm_xwall_side_0(unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "HTM bits can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(multiplicity_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::logic_error, "Multiplicity value ["<< multiplicity_ << "] is not valid ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_MAIN_WALL, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");

      switch (multiplicity_)
	{
	case 0 :	  
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT1, 0);
	  break;
	  
	case 1 :
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT1, 0);
	  break;
	  
	case 2 :
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT1, 1);
	  break;
	  
	default :
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE0_BIT1, 1); 
	  break;
	}
      _store |= STORE_CTW;
      return;
    }

    unsigned int calo_ctw::get_htm_xwall_side_0_info() const
    {
      if(_ctw_.test(calo::ctw::HTM_XWALL_SIDE0_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_XWALL_SIDE0_BIT1) == 0)
	{      
	  return 0;
	}
      else if(_ctw_.test(calo::ctw::HTM_XWALL_SIDE0_BIT0) == 1 && _ctw_.test(calo::ctw::HTM_XWALL_SIDE0_BIT1) == 0)
	{
	  return 1;
	}
      else if(_ctw_.test(calo::ctw::HTM_XWALL_SIDE0_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_XWALL_SIDE0_BIT1) == 1)
	{
	  return 2;
	}
      return 3;
    }  

    bool calo_ctw::is_htm_xwall_side_0() const
    {
      return get_htm_xwall_side_0_info() != 0;
    }

    void calo_ctw::set_htm_xwall_side_1(unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "HTM bits can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(multiplicity_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::logic_error, "Multiplicity value ["<< multiplicity_ << "] is not valid ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_MAIN_WALL, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");

      switch (multiplicity_)
	{
	case 0 :	  
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT1, 0);
	  break;
	  
	case 1 :
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT1, 0);
	  break;
	  
	case 2 :
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT0, 0);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT1, 1);
	  break;
	  
	default :
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT0, 1);
	  _ctw_.set(calo::ctw::HTM_XWALL_SIDE1_BIT1, 1); 
	  break;
	}
      _store |= STORE_CTW;
      return;
    }

    unsigned int calo_ctw::get_htm_xwall_side_1_info() const
    {
      if(_ctw_.test(calo::ctw::HTM_XWALL_SIDE1_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_XWALL_SIDE1_BIT1) == 0)
	{      
	  return 0;
	}
      else if(_ctw_.test(calo::ctw::HTM_XWALL_SIDE1_BIT0) == 1 && _ctw_.test(calo::ctw::HTM_XWALL_SIDE1_BIT1) == 0)
	{
	  return 1;
	}
      else if(_ctw_.test(calo::ctw::HTM_XWALL_SIDE1_BIT0) == 0 && _ctw_.test(calo::ctw::HTM_XWALL_SIDE1_BIT1) == 1)
	{
	  return 2;
	}
      return 3;
    }  

    bool calo_ctw::is_htm_xwall_side_1() const
    {
      return get_htm_xwall_side_1_info() != 0;
    }
    
    void calo_ctw::get_main_zoning_word(std::bitset<calo::ctw::MAIN_ZONING_BITSET_SIZE> & zoning_word_) const
    {
      zoning_word_ = 0x0;
      for (int i = calo::ctw::W_ZW_BIT0; i <= calo::ctw::W_ZW_BIT9; i++)
	{
	  if(_ctw_.test(i) == true)
	    {
	      zoning_word_.set(i-calo::ctw::W_ZW_BIT0,1);
	    }
	}
      return ;
    }

    void calo_ctw::set_main_zoning_word(std::bitset<calo::ctw::MAIN_ZONING_BITSET_SIZE> & zoning_word_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Zoning word can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_XWALL_GVETO, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");

      for (int i = 0; i < zoning_word_.size(); i++)
	{
	  if (zoning_word_.test(i) == true)
	    {
	      _ctw_.set(i + calo::ctw::W_ZW_BIT0, 1);
	    }
	  else 
	    {
	      _ctw_.set(i + calo::ctw::W_ZW_BIT0, 0);	      
	    }
	}      
      _store |= STORE_CTW;
      return;
    }

    void calo_ctw::set_zoning_bit(int bit_pos_, bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Zoning bit can't be set, calorimeter CTW is locked ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_XWALL_GVETO && (bit_pos_ < calo::ctw::X_ZW_BIT0 && bit_pos_ > calo::ctw::X_ZW_BIT3), std::logic_error, " Bit position for crate number 2 is not defined ! ");

      _ctw_.set(bit_pos_,value_);
      _store |= STORE_CTW;
      return;
    }

    unsigned int calo_ctw::compute_active_zones(std::set<int> & active_zones_ ) const
    {
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_XWALL_GVETO, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");
      unsigned int active_zone_counts = 0;
      for (int i = calo::ctw::W_ZW_BIT0 ; i <= calo::ctw::W_ZW_BIT9 ; i++)
	{
	  if(_ctw_.test(i) == true)
	    {
	      active_zones_.insert(i - calo::ctw::W_ZW_BIT0); 
	      active_zone_counts++;
	    }
	}
      return active_zone_counts;
    }  

    void calo_ctw::set_lto_main_wall_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "LTO bit can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_XWALL_GVETO, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");
      _ctw_.set(calo::ctw::LTO_MAIN_WALL_BIT, value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_lto_main_wall() const
    {
      return _ctw_.test(calo::ctw::LTO_MAIN_WALL_BIT);
    }


    void calo_ctw::set_lto_xwall_side_0_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "LTO bit can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_MAIN_WALL, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");
      _ctw_.set(calo::ctw::LTO_XWALL_SIDE0_BIT, value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_lto_xwall_side_0() const
    {
      return _ctw_.test(calo::ctw::LTO_XWALL_SIDE0_BIT);
    }

    void calo_ctw::set_lto_xwall_side_1_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "LTO bit can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_MAIN_WALL, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");
      _ctw_.set(calo::ctw::LTO_XWALL_SIDE1_BIT, value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_lto_xwall_side_1() const
    {
      return _ctw_.test(calo::ctw::LTO_XWALL_SIDE1_BIT);
    }

    void calo_ctw::set_lto_gveto_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "LTO bit can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(_layout_ == calo::ctw::LAYOUT_UNDEFINED || _layout_ == calo::ctw::LAYOUT_MAIN_WALL, std::logic_error, "Layout value [" << _layout_ << "] is not valid ! ");
      _ctw_.set(calo::ctw::LTO_GVETO_BIT, value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_lto_gveto() const
    {
      return _ctw_.test(calo::ctw::LTO_GVETO_BIT);
    }

    void calo_ctw::set_xt_pc_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, " External bit can't be set, calorimeter crate TW is locked ! ");
      _ctw_.set(calo::ctw::XT_PC_BIT,value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_xt() const
    {
      return _ctw_.test(calo::ctw::XT_PC_BIT);
    }

    void calo_ctw::get_control_word(std::bitset<calo::ctw::CONTROL_BITSET_SIZE> & control_word_) const
    {
      control_word_ = 0x0;
      for (int i = calo::ctw::CONTROL_BIT0; i <= calo::ctw::CONTROL_BIT3; i++)
	{
	  if(_ctw_.test(i))
	    {
	      control_word_.set(i - calo::ctw::CONTROL_BIT0,1);
	    }
	}
      return ;
    }

    void calo_ctw::set_control_word(std::bitset<calo::ctw::CONTROL_BITSET_SIZE> & control_word_)
    {    
      DT_THROW_IF(is_locked(), std::logic_error, "Control word can't be set, calorimeter crate TW is locked ! ");
      for (int i = 0; i < control_word_.size(); i++)
	{
	  if (control_word_.test(i) == true)
	    {
	      _ctw_.set(i + calo::ctw::CONTROL_BIT0,1);
	    }
	  else 
	    {
	      _ctw_.set(i + calo::ctw::CONTROL_BIT0,0);	      
	    }
	}  
      _store |= STORE_CTW;
      return;
    }

    void calo_ctw::get_full_word(std::bitset<calo::ctw::FULL_BITSET_SIZE> & full_word_) const
    {
      full_word_ = 0x0;
      for (int i = calo::ctw::BEGIN_BIT; i <= calo::ctw::END_BIT; i++)
	{
	  if(_ctw_.test(i))
	    {
	      full_word_.set(i - calo::ctw::BEGIN_BIT,1);
	    }
	}
      return ;
    }

    void calo_ctw::set_full_word(std::bitset<calo::ctw::FULL_BITSET_SIZE> & full_word_)
    {    
      DT_THROW_IF(is_locked(), std::logic_error, "Full word can't be set, calorimeter crate TW is locked ! ");
      for (int i = 0; i < full_word_.size(); i++)
	{
	  if (full_word_.test(i) == true)
	    {
	      _ctw_.set(i + calo::ctw::BEGIN_BIT, 1);
	    }
	  else 
	    {
	      _ctw_.set(i + calo::ctw::BEGIN_BIT, 0);	      
	    }
	}  
      _store |= STORE_CTW;
      return;
    }

    bool calo_ctw::is_locked() const
    {
      return _locked_;
    }

    void calo_ctw::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter crate TW is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void calo_ctw::unlock()
    {
      DT_THROW_IF(!is_locked(), std::logic_error, "Calorimeter crate TW is already unlocked ! ");
      _locked_ = false;
      return;
    } 

    void calo_ctw::reset_tw_bitset()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Bitset word can't be reset, calorimeter crate TW is locked ! ");
      _ctw_ = 0x0;
      _store &= ~STORE_CTW;
      return;
    }

    bool calo_ctw::is_valid() const
    {
      return _clocktick_25ns_ >= 0;
    }

    void calo_ctw::reset()
    {
      if(is_locked())
	{
	  unlock();
	}
      reset_tw_bitset();
      reset_clocktick_25ns();
      _layout_ = calo::ctw::LAYOUT_UNDEFINED;
      geomtools::base_hit::reset();
      return;
    }

    void calo_ctw::tree_dump (std::ostream & out_,
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

    void calo_ctw::_check()
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Clocktick is not valid ! ");
    }

  } // end of namespace digitization

} // end of namespace snemo
