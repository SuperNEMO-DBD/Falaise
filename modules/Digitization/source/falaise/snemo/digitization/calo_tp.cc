// snemo/digitization/calo_tp.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_tp.h>

// Third party:
// - Bayeux/datatools: 
#include <datatools/exception.h>

// This project :
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_tp, "snemo::digitalization::calo_tp")

    const unsigned int calo_tp::MAX_NUMBER_OF_CHANNELS;
    const unsigned int calo_tp::FULL_SIZE;
    const unsigned int calo_tp::HTM_SIZE;
    constexpr double calo_tp::LOW_THRESHOLD;
    constexpr double calo_tp::HIGH_THRESHOLD;
    calo_tp::calo_tp()
    {
      _locked_ = false;
      _clocktick_25ns_ = clock_utils::INVALID_CLOCKTICK;
      _tp_ = 0x0;
      return;
    }

    calo_tp::~calo_tp()
    {
      reset();
      return;
    }

    void calo_tp::set_header(int32_t hit_id_,
			     const geomtools::geom_id & electronic_id_,
			     int32_t clocktick_25ns_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter TP is locked !) ");
      set_hit_id(hit_id_);    
      set_geom_id(electronic_id_);
      set_clocktick_25ns(clocktick_25ns_);
      return;
    }
    
    void calo_tp::set_data(const double amplitude_,
			   const bool xt_bit_,
			   const bool spare_bit_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter TP is locked !) ");
      // To check :
      if (amplitude_ > HIGH_THRESHOLD)
	{
	  unsigned int multiplicity = 1;
	  set_htm(multiplicity);
	  set_xt_bit(xt_bit_);
	  set_spare_bit(spare_bit_);
	}
      else if (amplitude_ >= LOW_THRESHOLD && amplitude_ <= HIGH_THRESHOLD)
	{
	  set_lto_bit(true);
	  set_xt_bit(xt_bit_);
	  set_spare_bit(spare_bit_);
	}
      return;
    }

    void calo_tp::update_data(const double amplitude_,	
			      const bool xt_bit_,
			      const bool spare_bit_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter TP is locked !) ");
      unsigned int existing_multiplicity = get_htm();
      if (amplitude_ > HIGH_THRESHOLD)
	{
	  existing_multiplicity += 1;
	  set_htm(existing_multiplicity);
	}
      else if (amplitude_ >= LOW_THRESHOLD && amplitude_ <= HIGH_THRESHOLD)
	{
	  set_lto_bit(1);
	}
      set_xt_bit(xt_bit_);
      set_spare_bit(spare_bit_);
      return;
    }
    
    int32_t calo_tp::get_clocktick_25ns() const
    {
      return _clocktick_25ns_;
    }

    void calo_tp::set_clocktick_25ns(const int32_t clocktick_25_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be set, calorimeter TP is locked !) ");
      if(clocktick_25_ <= clock_utils::INVALID_CLOCKTICK)
	{
	  reset_clocktick_25ns();
	}
      else
	{
	  _clocktick_25ns_ = clocktick_25_;
	  _store |= STORE_CLOCKTICK_25NS;
	}
      return;
    }

    bool calo_tp::has_clocktick_25ns() const
    {
      return _clocktick_25ns_ != clock_utils::INVALID_CLOCKTICK;
    } 

    void calo_tp::reset_clocktick_25ns()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be reset, calorimeter TP is locked !) ");
      _clocktick_25ns_ = clock_utils::INVALID_CLOCKTICK;
      _store &= ~STORE_CLOCKTICK_25NS;
      return;
    }

    const std::bitset<calo_tp::FULL_SIZE> & calo_tp::get_tp_bitset() const
    {
      return _tp_;
    }
    
    void calo_tp::reset_tp_bitset()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "TP bitset can't be reset, calorimeter TP is locked ! ");
      _tp_ = 0x0;
      _store &= ~STORE_TP;
      return;
    }

    void calo_tp::set_htm(const unsigned int multiplicity_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "HTM bits can't be set, calorimeter TP is locked ! ");
      DT_THROW_IF(multiplicity_ > MAX_NUMBER_OF_CHANNELS, std::logic_error, "Multiplicity value ["<< multiplicity_ << "] is not valid ! ");

      switch (multiplicity_)
	{
	case 0 :	  
	  _tp_.set(HTM_BIT0, 0);
	  _tp_.set(HTM_BIT1, 0);
	  break;
	  
	case 1 :
	  _tp_.set(HTM_BIT0, 1);
	  _tp_.set(HTM_BIT1, 0);
	  break;
	  
	case 2 :
	  _tp_.set(HTM_BIT0, 0);
	  _tp_.set(HTM_BIT1, 1);
	  break;
	  
	default :
	  _tp_.set(HTM_BIT0, 1);
	  _tp_.set(HTM_BIT1, 1); 
	}
      _store |= STORE_TP;
      return;
    }
    
    unsigned int calo_tp::get_htm() const
    {
      if(_tp_.test(HTM_BIT0) == 0 && _tp_.test(HTM_BIT1) == 0)
	{           
	  return 0;
	}
      else if(_tp_.test(HTM_BIT0) == 1 && _tp_.test(HTM_BIT1) == 0)
	{
	  return 1;
	}
      else if(_tp_.test(HTM_BIT0) == 0 && _tp_.test(HTM_BIT1) == 1)
	{
	  return 2;
	}
      return 3;
    }

    std::bitset<calo_tp::HTM_SIZE> calo_tp::get_htm_bits() const
    {
      std::bitset<HTM_SIZE> htm_word = 0x0;
      if(_tp_.test(HTM_BIT0) == true)
	{
	  htm_word.set(HTM_BIT0, 1);
	}
      if(_tp_.test(HTM_BIT1) == true)
	{
	  htm_word.set(HTM_BIT1, 1);
	}
      return htm_word;
    }

    bool calo_tp::is_htm() const
    {
      return get_htm() != 0;
    }

    void calo_tp::set_lto_bit(const bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "LTO bit can't be set, calorimeter TP is locked ! ");
      _tp_.set(LTO_BIT,value_);
      _store |= STORE_TP;
      return;
    }
    
    bool calo_tp::is_lto() const
    {
      return _tp_.test(LTO_BIT);
    }

    void calo_tp::set_xt_bit(const bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, " External bit can't be set, calorimeter TP is locked ! ");
      _tp_.set(XT_BIT,value_);
      _store |= STORE_TP;
      return;
    }
    
    bool calo_tp::is_xt() const
    {
      return _tp_.test(XT_BIT);
    }

    void calo_tp::set_spare_bit(const bool value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Spare bit can't be set, calorimeter TP is locked ! ");
      _tp_.set(SPARE_BIT,value_);
      _store |= STORE_TP;
      return;
    }
    
    bool calo_tp::is_spare() const
    {
      return _tp_.test(SPARE_BIT);
    }

    bool calo_tp::is_locked() const
    {
      return _locked_;
    }

    void calo_tp::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Calorimeter TP is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void calo_tp::unlock()
    {
      DT_THROW_IF(!is_locked(), std::logic_error, "Calorimeter TP is already unlocked ! ");
      _locked_ = false;
      return;
    } 

    bool calo_tp::is_valid() const
    {
      return has_geom_id() && has_hit_id() && has_clocktick_25ns();
    }

    void calo_tp::reset()
    {
      if(is_locked())
	{
	  unlock();
	}
      reset_tp_bitset();
      reset_clocktick_25ns();
      geomtools::base_hit::reset();
      return;
    }

    void calo_tp::tree_dump (std::ostream & out_,
			     const std::string & title_,
			     const std::string & indent_,
			     bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Clock tick (25 ns)  : " << _clocktick_25ns_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "TP (5 bits) : " << _tp_  << std::endl;
      return;
    }
    
    void calo_tp::_check()
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Clocktick is not valid ! ");
    }

  } // end of namespace digitization

} // end of namespace snemo
