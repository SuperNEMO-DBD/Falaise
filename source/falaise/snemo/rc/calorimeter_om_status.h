/// \file falaise/snemo/rc/calorimeter_om_status.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-29
 * Last modified: 2022-05-29
 *
 * Description: Calorimeter OM status definitions
 */

#ifndef FALAISE_SNEMO_RC_CALORIMETER_OM_STATUS_H
#define FALAISE_SNEMO_RC_CALORIMETER_OM_STATUS_H

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

    /// \brief Calorimeter OM status
    class calorimeter_om_status
    {
    public:

      static const std::uint32_t OM_GOOD = 0; ///< Default status for a working OM with no issue

      /// \brief OM status is implemented as a bitset where each bit has a specific meaning
      enum status_bit
        {
         OM_DEAD = datatools::bit_mask::bit00, ///< OM is dead
         OM_OFF  = datatools::bit_mask::bit01  ///< OM is off
        };

      static bool is_off(std::uint32_t status_bits_);

      static bool is_dead(std::uint32_t status_bits_);

      static std::string status_to_string(std::uint32_t status_bits_);

      enum status_decode_flags
        {
         DECODE_TRIM = datatools::bit_mask::bit00
        };
      
      static std::uint32_t status_from_string(std::string status_repr_,
                                              const std::uint16_t options_ = 0);
      
    };

    struct calorimeter_om_status_record
    {
      time::time_period period;
      std::uint32_t status = calorimeter_om_status::OM_GOOD;
    };

    class calorimeter_om_status_history
    {
    public:
      void add(const time::time_period & period_, std::uint32_t status_);
      void clear();
      const std::vector<calorimeter_om_status_record> & records() const;
      std::uint32_t get_status(const time::time_point & t_) const;
    private:
      std::vector<calorimeter_om_status_record> _records_;
    };
    
  } // end of namespace rc
  
} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_CALORIMETER_OM_STATUS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
