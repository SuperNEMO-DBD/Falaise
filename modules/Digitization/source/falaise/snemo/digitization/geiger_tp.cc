// geiger_tp.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

// Ourselves:
#include <snemo/digitization/geiger_tp.h>

// Third party:
// - Bayeux/datatools:
//#include <datatools/utils.h>
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    //DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_tp, "snemo::digitalization::geiger_tp")

    geiger_tp::geiger_tp()
    {
      _locked_gg_tp_ = false;
      _clocktick_800ns_ = -1;
      _gg_tp_ = 0x0;
      grab_geom_id().set(RACK_INDEX, 3);
      return;
    }

    geiger_tp::~geiger_tp()
    {
      reset();
      return;
    }

    int32_t geiger_tp::get_clocktick_800ns() const
    {
      return _clocktick_800ns_;
    }

    void geiger_tp::set_clocktick_800ns( int32_t value_ )
    {
      DT_THROW_IF(is_locked_gg_tp(), std::logic_error, "Clocktick can't be set, geiger TP is locked !) ");
      if(value_ <= -1)
	{
	  reset_clocktick_800ns();
	}
      else
	{
      _clocktick_800ns_ = value_;
      _store |= STORE_CLOCKTICK_800NS;
	}
      return;
    }

    void geiger_tp::reset_clocktick_800ns()
    {
      DT_THROW_IF(is_locked_gg_tp(), std::logic_error, "Clocktick can't be reset, geiger TP is locked !) ");
      _clocktick_800ns_ = -1;
      _store &= ~STORE_CLOCKTICK_800NS;
      return;
    }

    const std::bitset<100> & geiger_tp::get_gg_bitset() const
    {
      return _gg_tp_;
    }
      
    void geiger_tp::reset_gg_bitset()
    {
      DT_THROW_IF(is_locked_gg_tp(), std::logic_error, "TP bitset can't be reset, geiger TP is locked ! ");
      _gg_tp_ = 0x0;
      _store &= ~STORE_GG_TP;
      return;
    }

    void geiger_tp::get_gg_tp_bitset(std::bitset<TP_SIZE> & gg_tp_word_) const
    {
      gg_tp_word_ = 0x0;
      for (int i = TP_BEGIN; i <= TP_END; i++)
	{
	  if(_gg_tp_.test(i) == true)
	    {
	      gg_tp_word_.set(i-TP_BEGIN,1);
	    }
	}
      return;
    }
    
    void geiger_tp::get_thws_bitset(std::bitset<THWS_SIZE> & gg_thws_word_) const
    {
      gg_thws_word_ = 0x0;
      for (int i = THWS_BEGIN; i <= THWS_END; i++)
	{
	  if(_gg_tp_.test(i) == true)
	    {
	      gg_thws_word_.set(i-THWS_BEGIN,1);
	    }
	}
      return;
    }

    void geiger_tp::get_address_bitset(std::bitset<ADDRESS_SIZE> & gg_address_word_) const
    {
      gg_address_word_ = 0x0;
      for (int i = ADDRESS_BEGIN; i <= ADDRESS_END; i++)
	{
	  if(_gg_tp_.test(i) == true)
	    {
	      gg_address_word_.set(i-ADDRESS_BEGIN,1);
	    }
	}
      return;
    }

    void geiger_tp::get_ttid_bitset(std::bitset<TTID_SIZE> & gg_ttid_word_) const
    {
      gg_ttid_word_ = 0x0;
      for (int i = TTID_BEGIN; i <= TTID_END; i++)
	{
	  if(_gg_tp_.test(i) == true)
	    {
	      gg_ttid_word_.set(i-TTID_BEGIN,1);
	    }
	}
      return;
    }

    void geiger_tp::get_control_bitset(std::bitset<CONTROL_SIZE> & gg_control_word_) const
    {
      gg_control_word_ = 0x0;
      for (int i = CONTROL_BEGIN; i <= CONTROL_END; i++)
	{
	  if(_gg_tp_.test(i) == true)
	    {
	      gg_control_word_.set(i-CONTROL_BEGIN,1);
	    }
	}
      return;
    }

    bool geiger_tp::get_tracker_trigger_mode() const
    {
      return _gg_tp_.test(TTM_BIT);
    }

    bool geiger_tp::get_tracker_side_mode() const
    {
      return _gg_tp_.test(TSM_BIT);
    }
    
    unsigned int geiger_tp::get_tracker_row_mode() const
    {
      unsigned int number_of_gg_rows = 0;
      for (int i = TRM_BIT0; i <= TRM_BIT2; i++)
	{
	  if (_gg_tp_.test(i) == 1)
	    {
	      number_of_gg_rows += std::pow(2,i - TRM_BIT0); 
	    }
	}
      return number_of_gg_rows;
    }

    unsigned int geiger_tp::get_board_id() const
    {
      unsigned int board_ID = 0;
      for (int i = BOARD_ID_BIT0; i <= BOARD_ID_BIT4; i++)
	{
	  if (_gg_tp_.test(i) == 1)
	    {
	      board_ID += std::pow(2,i - BOARD_ID_BIT0);
	    }
	}
      return board_ID;
    }

    unsigned int geiger_tp::get_crate_id() const
    {
      unsigned int crate_ID = 0;
      for (int i = CRATE_ID_BIT0; i <= CRATE_ID_BIT1; i++)
	{
	  if (_gg_tp_.test(i) == 1)
	    {
	      crate_ID += std::pow(2,i - CRATE_ID_BIT0);
	    }
	}
      return crate_ID;    
    }

    bool geiger_tp::is_locked_gg_tp() const
    {
      return _locked_gg_tp_;
    }

    void geiger_tp::lock_gg_tp()
    {
      DT_THROW_IF(is_locked_gg_tp(), std::logic_error, "Calorimeter TP is already locked ! ");
      _check();
      _locked_gg_tp_ = true;
      return;
    }
    
    void geiger_tp::unlock_gg_tp()
    {
      DT_THROW_IF(!is_locked_gg_tp(), std::logic_error, "Calorimeter TP is already unlocked ! ");
      _locked_gg_tp_ = false;
      return;
    } 

    bool geiger_tp::is_valid() const
    {
      return _clocktick_800ns_ >= 0;
    }

    void geiger_tp::reset()
    {
      if(is_locked_gg_tp())
	{
	  unlock_gg_tp();
	}
      reset_gg_bitset();
      reset_clocktick_800ns();
      geomtools::base_hit::reset();
      return;
    }

    void geiger_tp::tree_dump (std::ostream & out_,
			     const std::string & title_,
			     const std::string & indent_,
			     bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Clock tick (800 ns)  : " << _clocktick_800ns_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "GG TP (100 bits) : " << _gg_tp_  << std::endl;
      return;
    }
    
    void geiger_tp::_check()
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Clocktick is not valid ! ");
    }

  } // end of namespace digitization

} // end of namespace snemo
