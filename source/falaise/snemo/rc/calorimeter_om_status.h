/// \file falaise/snemo/rc/calorimeter_om_status.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-29
 * Last modified: 2025-07-17
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
         OM_OFF = datatools::bit_mask::bit01, ///< OM is off
         OM_NOISY = datatools::bit_mask::bit02, ///< OM is noisy
         OM_NO_BI_MONITORING = datatools::bit_mask::bit03, ///< OM has no Bi run monitoring (SDS)
         OM_NO_LI_MONITORING = datatools::bit_mask::bit04, ///< OM has no LI run monitoring (LED)
         OM_BI_UNSTABILITY_LONG = datatools::bit_mask::bit05, ///< OM gain has long term unstability after Bi runs
         OM_BI_JUMP = datatools::bit_mask::bit06, ///< OM gain has jumps after Bi runs
         OM_BI_SCREENING_SE_STRIPS = datatools::bit_mask::bit07, ///< OM Bi runs are impacted by screening bent source strip
				 OM_LI_UNSTABILITY_LONG = datatools::bit_mask::bit08, ///< OM gain has long term unstability after LI runs
         OM_BI_LI_UNCONSISTENCY_SHORT = datatools::bit_mask::bit09, ///< OM gain monitoring with respect to Bi and LI runs is inconsistent on a short term
         OM_BI_LI_UNCONSISTENCY_LONG = datatools::bit_mask::bit10, ///< OM gain monitoring with respect to Bi and LI runs is inconsistent on a long term
				 OM_OTHER_ISSUES = datatools::bit_mask::bit15 ///< OM meets other issues			
        };

      static bool is_off(const std::uint32_t status_bits_);

      static bool is_dead(const std::uint32_t status_bits_);

      static bool is_noisy(const std::uint32_t status_bits_);

      static bool is_no_bi_monitoring(const std::uint32_t status_bits_);

      static bool is_no_li_monitoring(const std::uint32_t status_bits_);

      static bool is_bi_unstability_long(const std::uint32_t status_bits_);

      static bool is_bi_jump(const std::uint32_t status_bits_);

      static bool is_bi_screening_se_strips(const std::uint32_t status_bits_);

      static bool is_li_unstability_long(const std::uint32_t status_bits_);

      static bool is_bi_li_unconsistency_short(const std::uint32_t status_bits_);

      static bool is_bi_li_unconsistency_long(const std::uint32_t status_bits_);

      static bool is_other_issues(const std::uint32_t status_bits_);

      static std::string status_to_string(const std::uint32_t status_bits_);

      enum status_decode_flags
        {
         DECODE_TRIM = datatools::bit_mask::bit00
        };
      
      static std::uint32_t status_from_string(const std::string & status_repr_,
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
      void add(const time::time_period & period_, const std::uint32_t status_);
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
