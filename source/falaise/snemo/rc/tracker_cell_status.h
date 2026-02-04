/// \file falaise/snemo/rc/tracker_cell_status.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-23
 * Last modified: 2025-07-17
 *
 * Description: Tracker cell status definitions
 */

#ifndef FALAISE_SNEMO_RC_TRACKER_CELL_STATUS_H
#define FALAISE_SNEMO_RC_TRACKER_CELL_STATUS_H

// Standard library:
#include <cstdint>
#include <map>

// Bayeux/datatools:
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/geomtools/geom_id.h>

// This project:
#include "falaise/snemo/time/time_utils.h"

namespace snemo {
  
  namespace rc {

    /// \brief Tracker cell status
    class tracker_cell_status
    {
    public:

      static const std::uint32_t CELL_GOOD = 0; ///< Default status for a working cell with no issue

      /// \brief Cell status is implemented as a bitset whre each bit has a specific meaning
      enum status_bit {
        CELL_DEAD              = datatools::bit_mask::bit00, ///< Cell is dead
        CELL_OFF               = datatools::bit_mask::bit01, ///< Cell is off
        CELL_NO_ANODE          = datatools::bit_mask::bit02, ///< Cell anode signal is not collected
        CELL_NO_BOTTOM_CATHODE = datatools::bit_mask::bit03, ///< Cell bottom cathode signal is not collected
        CELL_NO_TOP_CATHODE    = datatools::bit_mask::bit04, ///< Cell top cathode signal is not collected
        CELL_NOISY             = datatools::bit_mask::bit05, ///< Cell is noisy
        CELL_ON_TRIP           = datatools::bit_mask::bit06, ///< Cell is trippy
        CELL_OTHER_ISSUES      = datatools::bit_mask::bit15  ///< Cell has other issues			
      };

      static bool is_off(const std::uint32_t status_bits_);

      static bool is_dead(const std::uint32_t status_bits_);

      static bool is_no_anode(const std::uint32_t status_bits_);

      static bool is_no_bottom_cathode(const std::uint32_t status_bits_);

      static bool is_no_top_cathode(const std::uint32_t status_bits_);
 
      static bool is_noisy(const std::uint32_t status_bits_);
 
      static bool is_on_trip(const std::uint32_t status_bits_);
 
      static std::string status_to_string(const std::uint32_t status_bits_);

      enum status_decode_flags {
        DECODE_TRIM  = datatools::bit_mask::bit00,
				ONLY_ONE_BIT = datatools::bit_mask::bit01
      };
      
      static std::uint32_t status_from_string(const std::string & status_repr_,
																							const std::uint16_t options_ = 0);
      
    };

    struct tracker_cell_status_record
    {
      time::time_period period;
      std::uint32_t status = tracker_cell_status::CELL_GOOD;
 
			friend std::ostream & operator<<(std::ostream & out_, const tracker_cell_status_record & record_);
		};
		
    class tracker_cell_status_history
    {
    public:
      void add(const time::time_period & period_, const std::uint32_t status_);
      void clear();
      const std::vector<tracker_cell_status_record> & records() const;
      std::uint32_t get_status(const time::time_point & t_) const;
			void print(std::ostream & out_, const std::string & indent_ = "") const;

    private:
      std::vector<tracker_cell_status_record> _records_;
    };
 
    class tracker_cell_status_change_event
    {
		public:
			
			enum event_type
				{
					no_change,
					reset_bits,
					set_bit,
					unset_bit
				};			
			
			tracker_cell_status_change_event() = default;
			
		private:

			tracker_cell_status_change_event(const time::time_point & timestamp_,
																			 const event_type event_type_,
																			 tracker_cell_status::status_bit bit_);
		public:

			tracker_cell_status::status_bit bit() const;
			
			const time::time_point & timestamp() const;

			bool is_no_change_event() const;

			bool is_reset_bits_event() const;

			bool is_set_bit_event() const;

			bool is_unset_bit_event() const;

			friend std::ostream & operator<<(std::ostream & out_, const tracker_cell_status_change_event & event_);

			static tracker_cell_status_change_event make_reset(const time::time_point & timestamp_);

			static tracker_cell_status_change_event make_no_change(const time::time_point & timestamp_);
			
			static tracker_cell_status_change_event make_set_bit(const time::time_point & timestamp_, 
																													 const tracker_cell_status::status_bit bit_);
			
			static tracker_cell_status_change_event make_unset_bit(const time::time_point & timestamp_, 
																														 const tracker_cell_status::status_bit bit_);
			
		private:
			
			time::time_point _timestamp_;
			event_type _event_type_ = no_change;
			tracker_cell_status::status_bit _bit_ = tracker_cell_status::CELL_OFF;
			
		};

		class tracker_cell_status_change_event_list
    {
		public:
			
			tracker_cell_status_change_event_list() = default;

			std::size_t size() const;

			const tracker_cell_status_change_event & event(const int i_) const;

			void add_event(const tracker_cell_status_change_event & event_);
			
		private:

			std::vector<tracker_cell_status_change_event> _events_;
			
		};

		void build_tracker_cell_status_history_from_event_list(const tracker_cell_status_change_event_list & event_list_,
																													 tracker_cell_status_history & status_history_);
    
  } // end of namespace rc
  
} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_TRACKER_CELL_STATUS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
