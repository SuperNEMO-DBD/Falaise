/// \file falaise/snemo/rc/run_status.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-05-06
 * Last modified: 2025-05-06
 *
 * Description: Status of a run
 */

#ifndef FALAISE_SNEMO_RC_RUN_STATUS_H
#define FALAISE_SNEMO_RC_RUN_STATUS_H

// Standard library:
#include <cstdint>
#include <iostream>

// Bayeux:
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/datatools/i_tree_dump.h>

namespace snemo {

  namespace rc {

    typedef std::uint32_t run_status_type;

    /// \brief Run status definitions
    struct run_status
    {
      enum status_bits {
        good = 0x0,
        calo_issues = datatools::bit_mask::bit00,                // 1
        tracker_issues = datatools::bit_mask::bit01,             // 2
        gas_issues = datatools::bit_mask::bit02,                 // 4
        anti_radon_factory_issues = datatools::bit_mask::bit03,  // 8
        sds_issues = datatools::bit_mask::bit04,                 // 16
        truncated_issues =  datatools::bit_mask::bit05,          // 32
        lis_issues =  datatools::bit_mask::bit06,                // 64
        other_issues =  datatools::bit_mask::bit15               // 32768
      };
      
      static run_status_type make_good();
      
      static run_status_type make_from_bitset(const std::uint32_t bits_);
    
      static bool is_good(const run_status_type s_);

      static bool has_issues(const run_status_type s_);
    
      static bool has_calo_issues(const run_status_type s_);
    
      static bool has_tracker_issues(const run_status_type s_);
    
      static bool has_gas_issues(const run_status_type s_);
    
      static bool has_anti_radon_factory_issues(const run_status_type s_);
    
      static bool has_sds_issues(const run_status_type s_);
    
      static bool has_truncated_issues(const run_status_type s_);
    
      static bool has_other_issues(const run_status_type s_);
      
      static bool has_lis_issues(const run_status_type s_);
  
      static std::string to_string(const run_status_type s_);
      
      static void print_tree(const run_status_type s_,
                             std::ostream & out_,
                             const boost::property_tree::ptree & options_);
    };
    
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_RUN_STATUS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
