// snemo/digitization/geiger_ctw.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_ctw.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project : 
#include <snemo/digitization/geiger_tp.h>
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_ctw, "snemo::digitalization::geiger_ctw")

    geiger_ctw::geiger_ctw()
    {
      _locked_ = false;
      _clocktick_800ns_ = clock_utils::INVALID_CLOCKTICK;
      _gg_ctw_ = 0x0;
      return;
    }

    geiger_ctw::~geiger_ctw()
    {
      reset();
      return;
    }

    void geiger_ctw::set_header(int32_t hit_id_,
				const geomtools::geom_id & electronic_id_,
				int32_t clocktick_800ns_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger CTW is locked !) ");
      set_hit_id(hit_id_);
      set_geom_id(electronic_id_);
      set_clocktick_800ns(clocktick_800ns_);
      _set_full_board_id();
      _store |= STORE_CLOCKTICK_800NS;
      return;
    }

    int32_t geiger_ctw::get_clocktick_800ns() const
    {
      return _clocktick_800ns_;
    }

    void geiger_ctw::set_clocktick_800ns(int32_t value_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be set, Geiger CTW is locked !) ");
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

    bool geiger_ctw::has_clocktick_800ns() const
    {
      return _clocktick_800ns_ != clock_utils::INVALID_CLOCKTICK;
    }

    void geiger_ctw::reset_clocktick_800ns()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Clocktick can't be reset, Geiger CTW is locked !) ");
      _clocktick_800ns_ = clock_utils::INVALID_CLOCKTICK;
      _store &= ~STORE_CLOCKTICK_800NS;
      return;
    }

    void geiger_ctw::get_100_bits_in_ctw_word(unsigned int block_index_, std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const
    {
      DT_THROW_IF(block_index_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::logic_error, "Block index out of range (should be [0;19])  ! ");
      for (int i = 0; i < my_bitset_.size(); i++)
	{
	  if (_gg_ctw_.test(i + block_index_ * my_bitset_.size()) == true)
	    {
	      my_bitset_.set(i, 1);
	    }
	  else
	    {
	      my_bitset_.set(i, 0);
	    }	  
	}     
      return;
    }

    void geiger_ctw::set_100_bits_in_ctw_word(unsigned int block_index_, const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_)
    {
      DT_THROW_IF(block_index_ > mapping::NUMBER_OF_FEBS_BY_CRATE, std::logic_error, "Block index out of range (should be [0;19])  ! ");
      for (int i = 0; i < my_bitset_.size(); i++)
	{
	  if (my_bitset_.test(i) == true)
	    {
	      _gg_ctw_.set(i + block_index_ * my_bitset_.size(),1);
	    }
	  else 
	    {
	      _gg_ctw_.set(i + block_index_ * my_bitset_.size(),0);
	    }	  
	}
      _store |= STORE_GG_CTW;
      return;
    }

    void geiger_ctw::set_full_hardware_status(const std::bitset<geiger::tp::THWS_SIZE> & gg_tp_hardware_status_)
    {
      for (int i = geiger::tp::THWS_BEGIN; i <= CTW_BITSET_FULL_SIZE; i += geiger::tp::FULL_SIZE)
	{
	  for (int j = 0; j < gg_tp_hardware_status_.size(); j ++)
	    {
	      if (gg_tp_hardware_status_.test(j) == true)
		{
		  _gg_ctw_.set(i + j, 1);
		}
	    }
	}
      return;
    }

   void geiger_ctw::_set_full_board_id()
    {
      unsigned long board_id = 0;
      for (int i = geiger::tp::BOARD_ID_BIT0; i <= CTW_BITSET_FULL_SIZE; i += geiger::tp::FULL_SIZE)
	{
	  if (board_id == 10) board_id += 1;
	  std::bitset<geiger::tp::BOARD_ID_WORD_SIZE> board_id_bitset(board_id);
	  for (int j = 0; j < board_id_bitset.size(); j++)
	    {
	      if (board_id_bitset.test(j) == true)
		{
		  _gg_ctw_.set(i + j, 1);
		}
	    }
	  board_id ++;
	}
      
      return;
    }

    void geiger_ctw::set_full_crate_id(const std::bitset<geiger::tp::CRATE_ID_WORD_SIZE> & gg_tp_crate_id_)
    {
      for (int i = geiger::tp::CRATE_ID_BIT0; i <= CTW_BITSET_FULL_SIZE; i += geiger::tp::FULL_SIZE)
	{
	  for (int j = 0; j < gg_tp_crate_id_.size(); j ++)
	    {
	      if (gg_tp_crate_id_.test(j) == true)
		{
		  _gg_ctw_.set(i + j, 1);
		}
	    }
	}
      return;
    }

    bool geiger_ctw::is_locked() const
    {
      return _locked_;
    }

    void geiger_ctw::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Geiger CTW is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void geiger_ctw::unlock()
    {
      DT_THROW_IF(!is_locked(), std::logic_error, "Geiger CTW is already unlocked ! ");
      _locked_ = false;
      return;
    } 

    void geiger_ctw::reset_tw_bitset()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Bitset word can't be reset, Geiger CTW is locked ! ");
      _gg_ctw_ = 0x0;
      _store &= ~STORE_GG_CTW;
      return;
    }

    bool geiger_ctw::is_valid() const
    {
      return _clocktick_800ns_ != clock_utils::INVALID_CLOCKTICK;
    }

    void geiger_ctw::reset()
    {
      if(is_locked())
	{
	  unlock();
	}
      reset_tw_bitset();
      reset_clocktick_800ns();
      geomtools::base_hit::reset();
      return;
    }

    void geiger_ctw::tree_dump (std::ostream & out_,
			      const std::string & title_,
			      const std::string & indent_,
			      bool inherit_) const
    {
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Clock tick (800 ns)  : " << _clocktick_800ns_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "CTW ("<< _gg_ctw_.size() << " bits) : " << std::endl;

      std::bitset<100> my_bitset;
      for (int i = 0; i < mapping::NUMBER_OF_FEBS_BY_CRATE; i++)
	 {
	   get_100_bits_in_ctw_word(i, my_bitset);
	   out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
		<< my_bitset << std::endl;
	 }

      return;
    }

    void geiger_ctw::_check()
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Clocktick is not valid ! ");
    }

  } // end of namespace digitization

} // end of namespace snemo
