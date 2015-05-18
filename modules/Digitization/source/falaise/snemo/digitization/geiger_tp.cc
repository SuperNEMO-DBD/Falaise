// snemo/digitization/geiger_tp.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_tp.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project : 
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_tp, "snemo::digitalization::geiger_tp")

    geiger_tp::geiger_tp()
    {
      _locked_ = false;
      _clocktick_800ns_ = clock_utils::INVALID_CLOCKTICK;
      _gg_tp_ = 0x0;
      return;
    }

    geiger_tp::~geiger_tp()
    {
      reset();
      return;
    }

    void geiger_tp::set_header(int32_t hit_id_,
			       const geomtools::geom_id & electronic_id_,
			       int32_t clocktick_800ns_,
			       bool trigger_mode_,
			       bool trigger_side_, 
			       unsigned int number_of_rows_)
    {    
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger TP is locked !) ");
      DT_THROW_IF((electronic_id_.get(mapping::BOARD_INDEX) == 10), std::logic_error, "FEB ID 10 is dedicated to control board ! ");
      set_hit_id(hit_id_);
      set_geom_id(electronic_id_);
      set_clocktick_800ns(clocktick_800ns_);
      set_tracker_trigger_mode(trigger_mode_);
      set_tracker_row_mode(number_of_rows_);
      set_tracker_side_mode(trigger_side_);
      _set_address(electronic_id_);

      _store |= STORE_CLOCKTICK_800NS;
      _store |= STORE_GG_TP;
      return ;
    }

    void geiger_tp::set_data(std::bitset<geiger::tp::TP_SIZE> & gg_tp_word_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger TP is locked !) ");
      set_gg_tp_bitset(gg_tp_word_);
      return;
    }
			     
    int32_t geiger_tp::get_clocktick_800ns() const
    {
      return _clocktick_800ns_;
    }

    void geiger_tp::set_clocktick_800ns(int32_t value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be set, geiger TP is locked !) ");
      if(value_ <= clock_utils::INVALID_CLOCKTICK)
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
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be reset, geiger TP is locked !) ");
      _clocktick_800ns_ = clock_utils::INVALID_CLOCKTICK;
      _store &= ~STORE_CLOCKTICK_800NS;
      return;
    }

    const std::bitset<geiger::tp::FULL_SIZE> & geiger_tp::get_gg_bitset() const
    {
      return _gg_tp_;
    }
      
    void geiger_tp::reset_gg_bitset()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger bitset can't be reset, geiger TP is locked ! ");
      _gg_tp_ = 0x0;
      _store &= ~STORE_GG_TP;
      return;
    }

    void geiger_tp::get_gg_tp_bitset(boost::dynamic_bitset<> & gg_tp_word_) const
    {
      extract_bitset_from_is_length(geiger::tp::TP_BEGIN, geiger::tp::TP_SIZE, gg_tp_word_);
      return;
    }
    
    void geiger_tp::set_gg_tp_bitset(std::bitset<geiger::tp::TP_SIZE> & gg_tp_word_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "TP bitset can't be set, geiger TP is locked ! ");
	    
      for (int i = 0; i < geiger::tp::TP_SIZE; i++)
	{
	  if (gg_tp_word_.test(i) == true)
	    {
	      _gg_tp_.set(i + geiger::tp::TP_BEGIN, 1);
	    }
	  else 
	    {
	      _gg_tp_.set(i + geiger::tp::TP_BEGIN, 0);
	    }
	} 
      _store |= STORE_GG_TP;
      return;
    }

    void geiger_tp::set_gg_tp_active_bit(const int & bit_index_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Tp active bit can't be set, geiger TP is locked ! ");
      _gg_tp_.set(bit_index_, 1);
      
      return;
    }
    
    // void geiger_tp::get_thws_bitset(boost::dynamic_bitset<> & gg_thws_word_) const
    // {
    //   extract_bitset_from_is_length(THWS_BEGIN, THWS_SIZE, gg_thws_word_);
    //   return;
    // }

    void geiger_tp::get_hardware_status_bitset(std::bitset<geiger::tp::THWS_SIZE> & gg_hardware_status_) const
    {   
	for (int i = 0; i < geiger::tp::THWS_SIZE; i++)
	  {
	    if (_gg_tp_.test(i + geiger::tp::THWS_BEGIN) == 1)
	      {
		gg_hardware_status_.set(i, 1);
	      }
	  }
      return;
    }

    unsigned long geiger_tp::get_tracker_row_mode() const
    {
      boost::dynamic_bitset<> TRM_word;
      extract_bitset_from_is_length(geiger::tp::TRM_BIT0, geiger::tp::TRM_WORD_SIZE, TRM_word);
      return TRM_word.to_ulong();
    }

    void geiger_tp::set_tracker_row_mode(unsigned int number_of_rows_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Tracker row mode (TRM) can't be set, geiger TP is locked ! ");  
      DT_THROW_IF(number_of_rows_ > mapping::NUMBER_OF_CONNECTED_ROWS, std::range_error, "Unsupported number of rows ["<< number_of_rows_ <<"] ! ");
      std::bitset<geiger::tp::TRM_WORD_SIZE> row_tp_bitset(number_of_rows_);
      
      for (int i = 0; i < row_tp_bitset.size(); i++)
	{
	  _gg_tp_.set(i + geiger::tp::TRM_BIT0, row_tp_bitset[i]);
	}
      _store |= STORE_GG_TP;
      return;
    }

    bool geiger_tp::get_tracker_side_mode() const
    {
      return _gg_tp_.test(geiger::tp::TSM_BIT);
    }

    void geiger_tp::set_tracker_side_mode(bool trigger_side_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Tracker side mode (TSM) can't be set, geiger TP is locked ! ");  
 
      if (trigger_side_ == true)
	{
	  _gg_tp_.set(geiger::tp::TSM_BIT, 1);
	}
      else
	{
	  _gg_tp_.set(geiger::tp::TSM_BIT, 0);
	}
      _store |= STORE_GG_TP;
      return;
    }

    bool geiger_tp::get_tracker_trigger_mode() const
    {
      return _gg_tp_.test(geiger::tp::TTM_BIT);
    }
    
    void geiger_tp::set_tracker_trigger_mode(bool trigger_mode_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Tracker trigger mode (TTM) can't be set, geiger TP is locked ! ");  
      if (trigger_mode_ == true)
	{
	  _gg_tp_.set(geiger::tp::TTM_BIT, 1);
	}
      else
	{
	  _gg_tp_.set(geiger::tp::TTM_BIT, 0);
	}
      _store |= STORE_GG_TP;
      return;
    }

    void geiger_tp::get_address_bitset(boost::dynamic_bitset<> & gg_address_word_) const
    {
      extract_bitset_from_is_length(geiger::tp::ADDRESS_BEGIN, geiger::tp::ADDRESS_SIZE, gg_address_word_); 
      return;
    }

    void geiger_tp::_set_address(const geomtools::geom_id & electronic_id_)
    {
      set_crate_id(electronic_id_.get(mapping::CRATE_INDEX));
      set_board_id(electronic_id_.get(mapping::BOARD_INDEX));
      _store |= STORE_GG_TP;
    }

    unsigned long geiger_tp::get_board_id() const
    {
      boost::dynamic_bitset<> board_id_word;
      extract_bitset_from_is_length(geiger::tp::BOARD_ID_BIT0, geiger::tp::BOARD_ID_WORD_SIZE, board_id_word);
      return board_id_word.to_ulong();
    }

    void geiger_tp::set_board_id(unsigned long board_id_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Board ID can't be set, geiger TP is locked ! ");  
      DT_THROW_IF(board_id_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::range_error, "Unsupported board ID ["<< board_id_ <<"] ! ");
      std::bitset<geiger::tp::BOARD_ID_WORD_SIZE> board_id_bitset(board_id_);
      
      for (int i = 0; i < board_id_bitset.size(); i++)
	{
	  _gg_tp_.set(i + geiger::tp::BOARD_ID_BIT0, board_id_bitset[i]);
	}
      _store |= STORE_GG_TP;
      return;
    }

    void geiger_tp::get_crate_id(std::bitset<geiger::tp::CRATE_ID_WORD_SIZE> & crate_id_) const
    {
      for (int i = geiger::tp::CRATE_ID_BIT0; i <= geiger::tp::CRATE_ID_BIT1; i++)
	{
	  if(_gg_tp_.test(i) == true)
	    {
	      crate_id_.set(i - geiger::tp::CRATE_ID_BIT0, 1);
	    }
	}
      return;
    }

    void geiger_tp::set_crate_id(unsigned long crate_id_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Crate ID can't be set, geiger TP is locked ! ");  
      DT_THROW_IF(crate_id_ > mapping::NUMBER_OF_CRATES, std::range_error, "Unsupported crate ID ["<< crate_id_ <<"] ! ");
      std::bitset<geiger::tp::CRATE_ID_WORD_SIZE> crate_id_bitset(crate_id_);
      
      for (int i = 0; i < crate_id_bitset.size(); i++)
	{
	  _gg_tp_.set(i + geiger::tp::CRATE_ID_BIT0, crate_id_bitset[i]);
	}
      _store |= STORE_GG_TP;
      return;
    }

    void geiger_tp::get_ttid_bitset(boost::dynamic_bitset<> & gg_ttid_word_) const
    {
      extract_bitset_from_is_length(geiger::tp::TTID_BEGIN, geiger::tp::TTID_SIZE, gg_ttid_word_);
      return;
    }

    void geiger_tp::get_control_bitset(boost::dynamic_bitset<> & gg_control_word_) const
    {
      extract_bitset_from_is_length(geiger::tp::CONTROL_BEGIN, geiger::tp::CONTROL_SIZE, gg_control_word_);
      return;
    }

    void geiger_tp::extract_bitset_from_to(unsigned int BIT_BEGIN_, 
					   unsigned int BIT_END_, 
					   boost::dynamic_bitset<> & my_bitset_word_) const
    {
      unsigned int word_size = BIT_END_ - BIT_BEGIN_;     
      for (int i = 0; i < word_size; i++)
	{
	  if (_gg_tp_.test(i + BIT_BEGIN_) == 1)
	    {
	      my_bitset_word_.set(i, 1);
	    }
	}
      return;
    }
    
    void geiger_tp::extract_bitset_from_is_length(unsigned int BIT_BEGIN_,  
						  unsigned int word_size_, 
						  boost::dynamic_bitset<> & my_bitset_word_) const
    {
      for (int i = 0; i < word_size_; i++)
	{
	  if (_gg_tp_.test(i + BIT_BEGIN_) == 1)
	    {
	      my_bitset_word_.set(i, 1);
	    }
	}
      return;
    }

    bool geiger_tp::is_locked() const
    {
      return _locked_;
    }

    void geiger_tp::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger TP is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void geiger_tp::unlock()
    {
      DT_THROW_IF(!is_locked(), std::logic_error, "Geiger TP is already unlocked ! ");
      _locked_ = false;
      return;
    } 

    bool geiger_tp::is_valid() const
    {
      return _clocktick_800ns_ >= 0;
    }

    void geiger_tp::reset()
    {
      if(is_locked())
	{
	  unlock();
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
