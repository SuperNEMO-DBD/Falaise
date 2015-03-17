// geiger_tp.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_H

// Standard library :
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <vector>
#include <stdint.h>
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// - Bayeux/datatools :
#include <bayeux/datatools/bit_mask.h>

// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

namespace snemo {
  
  namespace digitization {

    /// \brief The geiger tracker trigger primitive 
    class geiger_tp : public geomtools::base_hit
      {
      public : 

	/// \brief Masks to automatically tag the attributes to be stored
	enum store_mask_type {
	  STORE_CLOCKTICK_800NS   = datatools::bit_mask::bit03, //!< Serialization mask for the clocktick
	  STORE_GG_TP             = datatools::bit_mask::bit04  //!< Serialization mask for the TP
	};

	/// Position of bits in the Geiger tracker TP bitset word (100 bits)
	enum gg_tp_bit_pos {
	  TP_BEGIN          = 0,   //!< Trigger primitive beginning
	  TP_END            = 54,  //!< Trigger primitive ending
	  THWS_BEGIN        = 55,  //!< Tracker hardware status beginning
	  TRM_BIT0          = 55,  //!< Tracker row mode (bit 0)
	  TRM_BIT1          = 56,  //!< Tracker row mode (bit 1)
	  TRM_BIT2          = 57,  //!< Tracker row mode (bit 2)
	  TSM_BIT           = 58,  //!< Tracker side mode bit
	  TTM_BIT           = 59,  //!< Tracker trigger mode bit (0 is 3-wires mode; 1 is 2-wires mode)
	  THWS_END          = 59,  //!< Tracker hardware status ending
	  ADDRESS_BEGIN     = 60,  //!< Board address beginning (board ID [5 bits] , crate ID [2 bits] + Spare [3 bits])
	  BOARD_ID_BIT0     = 60,  //!< Board ID (bit 0) 
	  BOARD_ID_BIT1     = 61,  //!< Board ID (bit 1)
	  BOARD_ID_BIT2     = 62,  //!< Board ID (bit 2)
	  BOARD_ID_BIT3     = 63,  //!< Board ID (bit 3)
	  BOARD_ID_BIT4     = 64,  //!< Board ID (bit 4)
 	  CRATE_ID_BIT0     = 65,  //!< Crate ID (bit 0)
	  CRATE_ID_BIT1     = 66,  //!< Crate ID (bit 1)
	  ADDRESS_END       = 69,  //!< Board address ending
	  TTID_BEGIN        = 70,  //!< Tracker trigger ID beginning
	  TTID_END          = 84,  //!< Tracker trigger ID ending
	  CONTROL_BEGIN     = 85,  //!< Control beginning
	  CONTROL_END       = 99   //!< Control ending
	}; 

	/// Size of bits word in the Geiger tracker TP bitset word (100 bits)
	enum gg_tp_size {
	  TP_SIZE           = 55,
	  THWS_SIZE         = 5,
    	  ADDRESS_SIZE      = 10,
	  TTID_SIZE         = 15,
	  CONTROL_SIZE      = 15
	};

	/// Electronic id index coming from a geom id	
	enum tp_electronic_ID_index {
	  RACK_INDEX  = 0,
	  CRATE_INDEX = 1,
	  BOARD_INDEX = 2
	};
      
	/// Default constructor
	geiger_tp();

	/// Destructor
	virtual ~geiger_tp();
  
	/// Return the timestamp of the geiger trigger primitive
	int32_t get_clocktick_800ns() const;

	/// Set the timestamp of the geiger trigger primitive
	void set_clocktick_800ns(int32_t value_);

	/// Reset the timestamp of the geiger trigger primitive
	void reset_clocktick_800ns();

	/// Return the const reference to the full geiger bitset
	const std::bitset<100> & get_gg_bitset() const;

	/// Reset the full geiger bitset bitset
	void reset_gg_bitset();

	/// Return the const reference to the geiger TP bitset
	void get_gg_tp_bitset(std::bitset<TP_SIZE> & gg_tp_word_) const;

	/// Return the const reference to the geiger Tracker HardWare Status (THWS) bitset
	void get_thws_bitset(std::bitset<THWS_SIZE> & gg_thws_word_) const;
	
	/// Return the const reference to the geiger board address + spare bitset
	void get_address_bitset(std::bitset<ADDRESS_SIZE> & gg_address_word_) const;

	/// Return the const reference to the geiger Tracker Trigger ID (TTID) bitset
	void get_ttid_bitset(std::bitset<TTID_SIZE> & gg_ttid_word_) const;

	/// Return the const reference to the geiger control bitset
	void get_control_bitset(std::bitset<CONTROL_SIZE> & gg_control_word_) const;

	/// Get the tracker trigger mode (0 is 3-wires mode and 1 is 2-wires mode)
	bool get_tracker_trigger_mode() const;
	
	/// Get the tracker side mode (0 is one detector side and 1 is both detector side)
	bool get_tracker_side_mode() const;

	/// Get the tracker row mode (return the number of connected Geiger rows to the FEB (up to 7 rows))
	unsigned int get_tracker_row_mode() const;

	/// Get the board ID 
	unsigned int get_board_id() const;

	/// Get the crate ID 
	unsigned int get_crate_id() const;
	
	/// Check the lock status
	bool is_locked_gg_tp() const;

	/// Lock 
	void lock_gg_tp();

	/// Unlock
	void unlock_gg_tp();

	/// Check if the internal data of the geiger TP is valid
	bool is_valid() const;

	/// Reset the internal data of hit
	void reset();

	/// Smart print
	virtual void tree_dump(std::ostream      & a_out    = std::clog,
			       const std::string & a_title  = "",
			       const std::string & a_indent = "",
			       bool a_inherit               = false) const;

      protected : 

	/// Check if a geiger TP is available
	void _check();

      private : 

	bool _locked_gg_tp_; //!< geiger TP lock flag
	int32_t _clocktick_800ns_; //!< The timestamp of the geiger trigger primitive  
	std::bitset<100> _gg_tp_; //!< The geiger trigger primitive bitset

	//DATATOOLS_SERIALIZATION_DECLARATION();

      };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
