/// \file falaise/snemo/rc/run_description.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2025-05-07
 *
 * Description: Description of a run
 *
 * Boost/date_time API documentation: 
 *   https://www.boost.org/doc/libs/1_79_0/doc/html/date_time/posix_time.html
 *
 */

#ifndef FALAISE_SNEMO_RC_RUN_DESCRIPTION_H
#define FALAISE_SNEMO_RC_RUN_DESCRIPTION_H

// Standard library:
#include <cstdint>
#include <iostream>
#include <string>
#include <optional>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/version_id.h>

// This project:
#include <falaise/snemo/time/time_utils.h>
#include <falaise/snemo/rc/run_utils.h>
#include <falaise/snemo/rc/run_status.h>

namespace snemo {

  namespace rc {

    /// \brief Categories of run
    enum class run_category : std::uint16_t
      {
        INDETERMINATE = 0, ///< Undefined/invalid run category
        TEST          = 1, ///< Test run
        COMMISSIONING = 2, ///< Commissioning run
        PRODUCTION    = 3, ///< Production run (for betabeta or background data collection) [standard run]
        CALIBRATION_1 = 4, ///< Calibration run type 1 (Bi-207 calibration run) [standard run]
        CALIBRATION_BI = CALIBRATION_1, 
        CALIBRATION_2  = 5, ///< Calibration run type 2 (LI calibration run) [standard run]
        CALIBRATION_LI = CALIBRATION_2, 
        CALIBRATION_3  = 6, ///< Calibration run type 3 (reference OM calibration run) [standard run]
        CALIBRATION_REFOM = CALIBRATION_3, 
        CALIBRATION_4     = 7, ///< Calibration run type 4 (OM pedestal calibration run) [standard run]
        CALIBRATION_OMPED = CALIBRATION_4 
      };

    std::ostream & operator<<(std::ostream & out_, const run_category run_cat_);

    std::string to_string(const run_category run_cat_);

    run_category run_category_from_string(const std::string & label_);

    run_category run_category_from_uint(const std::uint32_t value_);
    
    /// \brief Run description
    class run_description
      : public datatools::i_tree_dumpable
    {
    public:

      static const run_id_type INVALID_RUN_ID = -1;
      static const run_id_type ALL_RUN_IDS = -2;
      
      run_description() = default;

      virtual ~run_description() = default;

      bool is_valid() const;

      void reset();

      /// Set the run number (run ID)
      void set_run_id(const run_id_type id_);

      /// Return the run number (run ID)
      std::int32_t run_id() const;
     
      /// Return the run number (run ID)
      std::int32_t run_number() const;

      run_category category() const;

      // bool has_daq_config_id() const;
 
      // const datatools::version_id & daq_config_id() const;
      
      // /// Set the DAQ configuration ID
      // void set_daq_config_id(const datatools::version_id & id_);

      void set_status(const rc::run_status_type s_);
      
      rc::run_status_type status() const;

      // Create a run decription object with a single time slice and no breaks
      static run_description make_unique_slice(const run_id_type run_id_,
                                               const run_category run_cat_,
                                               // const datatools::version_id & daq_config_id_,
                                               const time::time_period & run_period_,
                                               const std::uint32_t number_of_entries_,
                                               const std::optional<time::time_duration> & run_deadtime_ = std::nullopt,
                                               const std::optional<rc::run_status_type> & run_status_ = std::nullopt);
      
      // Create a run decription object with some breaks
      static run_description make_with_breaks(const run_id_type run_id_,
                                              const run_category run_cat_,
                                              //  const datatools::version_id & daq_config_id_,
                                              const time::time_period & run_period_,
                                              const std::uint32_t number_of_entries_,
                                              const std::vector<time::time_period> & breaks_,
                                              const std::optional<std::vector<time::time_duration>> & run_deadtimes_ = std::nullopt,
                                              const std::optional<rc::run_status_type> & run_status_ = std::nullopt);
 
      const time::time_period & period() const;
 
      time::time_point begin() const;

      time::time_point end() const;
  
      time::time_duration duration() const;

      bool has_number_of_entries() const;
 
      std::uint32_t number_of_entries() const;

      std::uint32_t total_number_of_entries() const;

      bool has_breaks() const;
 
      void add_break(const time::time_period & break_);
 
      void add_break(const time::time_duration & break_shift_,
                     const time::time_duration & break_duration_);

      bool has_deadtime() const;

      void set_deadtime(const std::size_t slice_index_,
                        const time::time_duration & deadtime_);
      
      bool has_slices() const;

      bool is_unique_slice() const;
      
      const std::vector<time::time_period> & slices() const;
 
      const std::vector<time::time_duration> & deadtimes() const;
 
      const std::vector<time::time_period> & breaks() const;
      
      bool contains(const time::time_point & timestamp_) const;

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

      void load(const datatools::properties & config_);

      time::time_duration total_deadtime() const;
      
      time::time_duration effective_duration() const;
      
      void lock();
      
      bool is_locked() const;

      const boost::property_tree::ptree & details() const;

      boost::property_tree::ptree & details();
      
      
    private:

      void _sync_();

      bool _locked_ = false; ///< Lock flag
      run_id_type _run_id_ = INVALID_RUN_ID; ///< Unique run number
      run_category _category_ = run_category::INDETERMINATE; ///< Run category
      // datatools::version_id _daq_config_id_; ///< DAQ configuration ID 
          
      /// Run span period
      time::time_period _period_{time::time_point(time::not_a_date_time), time::time_point(time::not_a_date_time)}; 
      std::vector<time::time_period> _breaks_; ///< Array of breaks (paused/resume data acquisition time intervals)
      std::vector<time::time_period> _slices_; ///< Array of active data acquisition time slices
      std::vector<time::time_duration> _deadtimes_; ///< Array of estimated deadtimes associated to active time slices
      // The concept of number of entries in a run depends on the run type:
      // run type: 3/4 -> number of triggers
      // run type: 5/6 -> number of calo hits
      std::uint32_t _number_of_entries_ = 0; ///< Number of entries within the run
      rc::run_status_type _status_ = rc::run_status::good; ///< Status bits

      /// \brief Additional details about the run (dictionary = Boost ptree)
      /// Details may contains:
      ///
      /// - "nb_calo_hits" : uint32
      /// - "nb_tracker_hits" : uint32
      /// - "nb_triggers" : uint32
      /// - "crates" : bitset<7>
      /// - "phase_id" : int
      /// - "daq_config" : string (instance ID see class "snemo::datasets::instance_id")
      /// 
      boost::property_tree::ptree _details_; 
      
      // 2025-10-03 FM:
      // The _daq_config_id_ attribute maybe ignored at this stage (to be discussed)
      // The _deadtimes_ attribute maybe also not meaningfull (to be discussed)
      // The _number_of_entries__ attribute maybe also not used because what could be interesting for
      // data analysis is, dependeing on the context :
      // - the number of raw hits,
      // - the number of raw triggers,
      // - the number of reconstructed events (RTD 2 RED algo)
      // To be discussed.
      
    };
    
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_RUN_DESCRIPTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
