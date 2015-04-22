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
      _clocktick_25ns_ = clock_utils::INVALID_CLOCKTICK;
      _ctw_ = 0x0;
      _wall_ = INVALID_WALL;
      return;
    }

    calo_ctw::~calo_ctw()
    {
      reset();
      return;
    }

    void calo_ctw::set_header(int32_t hit_id_,
			      const geomtools::geom_id & electronic_id_,
			      int32_t clocktick_25ns_,
			      wall_type wall_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter CTW is locked !) ");
      set_hit_id(hit_id_);
      set_geom_id(electronic_id_);
      set_clocktick_25ns(clocktick_25ns_);
      set_wall(wall_);
      return;
    }	

    void calo_ctw::set_wall(wall_type type_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Wall can't be set, calorimeter crate TW is locked !) ");
      _wall_ = type_;
      _store |= STORE_WALL;
    }

    calo_ctw::wall_type calo_ctw::get_wall() const
    {
      return _wall_;
    }

    bool calo_ctw::is_main_wall() const
    {
      if (get_wall() == MAIN_WALL )
	{
	  return true; 
	}
      return false;
    }

    bool calo_ctw::is_xg_wall() const
    {
      if (get_wall() == XG_WALL) 
	{
	  return true; 
	}
      return false;
    }

    void calo_ctw::set_main_wall()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Wall can't be set, calorimeter crate TW is locked !) ");
      set_wall(MAIN_WALL);
      _store |= STORE_WALL;
      return;
    }

    void calo_ctw::set_xg_wall()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Wall can't be set, calorimeter crate TW is locked !) ");
      set_wall(XG_WALL);
      _store |= STORE_WALL;
      return;
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

    void calo_ctw::set_htm_pc(unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "HTM bits can't be set, calorimeter crate TW is locked ! ");
      DT_THROW_IF(multiplicity_ > MAX_NUMBER_OF_FEB, std::logic_error, "Multiplicity value ["<< multiplicity_ << "] is not valid ! ");

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
	  break;
	}
      _store |= STORE_CTW;
      return;
    }
    
    unsigned int calo_ctw::get_htm_pc_info() const
    {
      if(_ctw_.test(HTM_PC_BIT0) == 0 && _ctw_.test(HTM_PC_BIT1) == 0)
	{      
	  return 0;
	}
      else if(_ctw_.test(HTM_PC_BIT0) == 1 && _ctw_.test(HTM_PC_BIT1) == 0)
	{
	  return 1;
	}
      else if(_ctw_.test(HTM_PC_BIT0) == 0 && _ctw_.test(HTM_PC_BIT1) == 1)
	{
	  return 2;
	}
      return 3;
    }

    bool calo_ctw::is_htm() const
    {
      return get_htm_pc_info() != 0;
    }
    
    void calo_ctw::get_zoning_word(std::bitset<10> & zoning_word_) const
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

    void calo_ctw::set_zoning_word(std::bitset<10> & zoning_word_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Zoning word can't be set, calorimeter crate TW is locked ! ");
      for (int i = 0; i < zoning_word_.size(); i++)
	{
	  if (zoning_word_.test(i) == true)
	    {
	      _ctw_.set(i + ZONING_BIT0, 1);
	    }
	  else 
	    {
	      _ctw_.set(i + ZONING_BIT0, 0);	      
	    }
	}      
      _store |= STORE_CTW;
      return;
    }

    void calo_ctw::set_zoning_bit(int BIT_POS_, bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Zoning bit can't be set, calorimeter CTW is locked ! ");
      _ctw_.set(BIT_POS_,value_);
      _store |= STORE_CTW;
      return;
    }

    unsigned int calo_ctw::compute_active_zones(std::set<int> & active_zones_ ) const
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

    void calo_ctw::set_lto_pc_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "LTO bit can't be set, calorimeter crate TW is locked ! ");
      _ctw_.set(LTO_PC_BIT,value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_lto() const
    {
      return _ctw_.test(LTO_PC_BIT);
    }

    void calo_ctw::set_xt_pc_bit(bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, " External bit can't be set, calorimeter crate TW is locked ! ");
      _ctw_.set(XT_PC_BIT,value_);
      _store |= STORE_CTW;
      return;
    }
    
    bool calo_ctw::is_xt() const
    {
      return _ctw_.test(XT_PC_BIT);
    }

    void calo_ctw::get_control_word(std::bitset<4> & control_word_) const
    {
      control_word_ = 0x0;
      for (int i = CONTROL_BIT0; i <= CONTROL_BIT3; i++)
	{
	  if(_ctw_.test(i))
	    {
	      control_word_.set(i-CONTROL_BIT0,1);
	    }
	}
      return ;
    }

    void calo_ctw::set_control_word(std::bitset<4> & control_word_)
    {    
      DT_THROW_IF(is_locked(), std::logic_error, "Control word can't be set, calorimeter crate TW is locked ! ");
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
      geomtools::base_hit::reset();
      _wall_ = INVALID_WALL;
      return;
    }

    void calo_ctw::tree_dump (std::ostream & out_,
			      const std::string & title_,
			      const std::string & indent_,
			      bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
	   << "Wall type  : " << _wall_ << std::endl;

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
