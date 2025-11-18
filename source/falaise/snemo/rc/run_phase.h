/// \file falaise/snemo/rc/run_phase.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-08-27
 * Last modified: 2025-09-24
 *
 * Description: Description of a run phase
 *
 */

#ifndef FALAISE_SNEMO_RC_RUN_PHASE_H
#define FALAISE_SNEMO_RC_RUN_PHASE_H

// Standard library:
#include <cstdint>
// #include <vector>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/bit_mask.h>

// This project:
#include <falaise/snemo/rc/run_utils.h>
#include <falaise/snemo/rc/run_description.h>
#include <falaise/snemo/time/time_utils.h>

namespace snemo {

  namespace rc {
    
    /// \brief Description of a run phase
    class run_phase
      : public datatools::i_tree_dumpable
    {
    public:

      enum status_bits {
        good = 0x0,
        other_issues = datatools::bit_mask::bit15 ///< Phase has issues of some undocumented type
      };
     
      run_phase() = default;

      virtual ~run_phase() = default;

      void reset();

      void set_id(const std::int32_t id_);

      std::int32_t id() const;

      void set_start_time(const time::time_point & start_time_);

      void set_stop_time(const time::time_point & stop_time_);

      const time::time_point & start_time() const;

      const time::time_point & stop_time() const;

      bool has_valid_period() const;
      
      bool has_closed_period() const;

      time::time_period period() const;
 
      void set_status(const std::uint32_t status_);

      std::uint32_t status() const;
      
      bool status_is_good() const;
     
      bool contains(const time::time_point & timestamp_) const;

      bool has_run(const run_description & rundesc_) const;
      
      /// Smart print
      virtual void print_tree(std::ostream & out_,
                              const boost::property_tree::ptree & options_) const override;
     
    private:
      
      std::int32_t _id_ = -1; ///< Unique ID of the phase
      time::time_point _start_time_ = boost::date_time::not_a_date_time; ///< Starting time of this phase
      time::time_point _stop_time_ = boost::date_time::not_a_date_time; ///< Stopping time of this phase
      std::uint32_t _status_ = good; ///< Status bits (16 bits should be enough)
      
    };
  
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_RUN_PHASE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
