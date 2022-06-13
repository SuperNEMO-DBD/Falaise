/// \file falaise/snemo/rc/tracker_cell_status.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-23
 * Last modified: 2022-05-23
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
      enum status_bit
      {
       CELL_DEAD              = datatools::bit_mask::bit00, ///< Cell is dead
       CELL_OFF               = datatools::bit_mask::bit01, ///< Cell is off
       CELL_NO_ANODE          = datatools::bit_mask::bit02, ///< Cell anode signal is not collected
       CELL_NO_BOTTOM_CATHODE = datatools::bit_mask::bit03, ///< Cell bottom cathode signal is not collected
       CELL_NO_TOP_CATHODE    = datatools::bit_mask::bit04  ///< Cell top cathode signal is not collected
      };

      static bool is_off(std::uint32_t status_bits_);

      static bool is_dead(std::uint32_t status_bits_);

      static bool is_no_anode(std::uint32_t status_bits_);

      static bool is_no_bottom_cathode(std::uint32_t status_bits_);

      static bool is_no_top_cathode(std::uint32_t status_bits_);
 
      static std::string status_to_string(std::uint32_t status_bits_);

      enum status_decode_flags
        {
         DECODE_TRIM = datatools::bit_mask::bit00
        };
      
      static std::uint32_t status_from_string(std::string status_repr_, const std::uint16_t options_ = 0);
      
    };

    struct tracker_cell_status_record
    {
      time::time_period period;
      std::uint32_t status = tracker_cell_status::CELL_GOOD;
    };

    class tracker_cell_status_history
    {
    public:
      void add(const time::time_period & period_, std::uint32_t status_);
      void clear();
      const std::vector<tracker_cell_status_record> & records() const;
      std::uint32_t get_status(const time::time_point & t_) const;
    private:
      std::vector<tracker_cell_status_record> _records_;
    };
    
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
