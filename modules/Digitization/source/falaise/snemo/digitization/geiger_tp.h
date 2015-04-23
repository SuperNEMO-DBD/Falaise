// snemo/digitization/geiger_tp.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_H

// Standard library :
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/bit_mask.h>
// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

// This project : 
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/geiger_tp_constants.h>

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
      
			/// Default constructor
			geiger_tp();

			/// Destructor
			virtual ~geiger_tp();

			/// Set the header with valid values
			void set_header(int32_t hit_id_,
											const geomtools::geom_id & electronic_id_,
											int32_t clocktick_800ns_,
											bool trigger_mode_ ,
											bool trigger_side_, 
											unsigned int number_of_rows_);

			/// Set the data with values
			void set_data(std::bitset<geiger::tp::TP_SIZE> & gg_tp_word_);

			/// Return the const timestamp of the geiger trigger primitive
			int32_t get_clocktick_800ns() const;

			/// Set the timestamp of the geiger trigger primitive
			void set_clocktick_800ns(int32_t value_);

			/// Reset the timestamp of the geiger trigger primitive
			void reset_clocktick_800ns();

			/// Return the const reference to the full geiger bitset
			const std::bitset<geiger::tp::FULL_SIZE> & get_gg_bitset() const;

			/// Reset the full geiger bitset bitset
			void reset_gg_bitset();

			/// Return the const reference to the geiger TP bitset
			void get_gg_tp_bitset(boost::dynamic_bitset<> & gg_tp_word_) const;

			/// Set the geiger trigger primitive bitset (55 bits)
			void set_gg_tp_bitset(std::bitset<geiger::tp::TP_SIZE> & gg_tp_word_);
			
			/// Set the active bit when a geiger cell is fired
			void set_gg_tp_active_bit(const int & bit_index_);

			/// Return the const reference to the geiger Tracker HardWare Status (THWS) bitset
			// void get_thws_bitset(boost::dynamic_bitset<> & gg_thws_word_) const;
			
			/// Return the const reference to the geiger Tracker HardWare Status (THWS) bitset
			void get_hardware_status_bitset(std::bitset<geiger::tp::THWS_SIZE> & gg_hardware_status_) const;
			
			/// Get the tracker row mode (return the number of connected Geiger rows to the FEB (up to 7 rows))
			unsigned long get_tracker_row_mode() const;
			                   
			/// Set the tracker row mode (number of connected Geiger rows to the FEB (up to 7 rows))
			void set_tracker_row_mode(unsigned int number_of_rows_);

			/// Get the tracker side mode (0 is one detector side and 1 is both detector side)
			bool get_tracker_side_mode() const;

			/// Set the tracker side mode (0 is one detector side and 1 is both detector side)
			void set_tracker_side_mode(bool trigger_side_);

			/// Get the tracker trigger mode (0 is 3-wires mode and 1 is 2-wires mode)
			bool get_tracker_trigger_mode() const;
	
			/// Set the tracker trigger mode (0 is 3-wires mode and 1 is 2-wires mode)
			void set_tracker_trigger_mode(bool trigger_mode_);

			/// Return the const reference to the geiger board address + spare bitset
			void get_address_bitset(boost::dynamic_bitset<> & gg_address_word_) const;

			/// Get the board ID (ID = 0-19)
			unsigned long get_board_id() const;

			/// Set the board ID (ID = 0-19)
			void set_board_id(unsigned long board_id_);

			/// Get the crate ID (ID = 0-2)
			void get_crate_id(std::bitset<geiger::tp::CRATE_ID_WORD_SIZE> & crate_id_) const;

			/// Set the crate ID (ID = 0-2)
			void set_crate_id(unsigned long crate_id_);

			/// Return the const reference to the geiger Tracker Trigger ID (TTID) bitset
			void get_ttid_bitset(boost::dynamic_bitset<> & gg_ttid_word_) const;

			/// Return the const reference to the geiger control bitset
			void get_control_bitset(boost::dynamic_bitset<> & gg_control_word_) const;
	
			/// Extract dynamic bitset from bit begin to bit end
			void extract_bitset_from_to(unsigned int BIT_BEGIN_, 
																	unsigned int BIT_END_, 
																	boost::dynamic_bitset<> & my_bitset_word_) const;

			/// Extract dynamic bitset from bit begin with a given length
			void extract_bitset_from_is_length(unsigned int BIT_BEGIN_, 
																				 unsigned int word_size_, 
																				 boost::dynamic_bitset<> & my_bitset_word_) const;
			
			/// Check the lock status
			bool is_locked() const;

			/// Lock 
			void lock();

			/// Unlock
			void unlock();

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

			/// Set the address id for a geiger tp 
			void _set_address(const geomtools::geom_id & electronic_id_);


		private : 

			bool _locked_; //!< geiger TP lock flag
			int32_t _clocktick_800ns_; //!< The timestamp of the geiger trigger primitive  
			std::bitset<geiger::tp::FULL_SIZE> _gg_tp_; //!< The geiger trigger primitive bitset

			DATATOOLS_SERIALIZATION_DECLARATION();

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
