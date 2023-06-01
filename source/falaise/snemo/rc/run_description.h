/// \file falaise/snemo/rc/run_description.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2022-04-27
 *
 * Description: Description of a run
 *
 * Boost/date_tile API documentation: 
 *   https://www.boost.org/doc/libs/1_79_0/doc/html/date_time/posix_time.html
 *
 */

#ifndef FALAISE_SNEMO_RC_RUN_DESCRIPTION_H
#define FALAISE_SNEMO_RC_RUN_DESCRIPTION_H

// Standard library:
#include <cstdint>
#include <iostream>
#include <string>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/snemo/time/time_utils.h>

namespace snemo {

  namespace rc {

    enum class run_category : std::uint16_t
      {
       INDETERMINATE,
       TEST,
       COMMISSIONING,
       PRODUCTION,
       CALIBRATION_1,
       CALIBRATION_2,
       CALIBRATION_3,
       CALIBRATION_4
      };

    std::ostream & operator<<(std::ostream & out_, run_category run_cat_);

    run_category from_string(const std::string & label_);

    typedef int32_t run_id_type;
    
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

      void set_run_id(const run_id_type id_);
      
      std::int32_t run_id() const;

      static run_description make(const run_id_type run_id_,
                                  const run_category run_cat_,
                                  const time::time_period & run_period_,
                                  std::uint32_t number_of_events_);
 
      inline const time::time_period & period() const
      {
        return _period_;
      }
 
      inline time::time_point begin() const
      {
        return _period_.begin();
      }

      inline time::time_point end() const
      {
        return _period_.end();
      }
  
      time::time_duration duration() const;
 
      std::uint32_t number_of_events() const;

      bool has_breaks() const;
 
      void add_break(const time::time_period & break_);
 
      void add_break(const time::time_duration & break_shift_,
                     const time::time_duration & break_duration_);

      bool has_slices() const;

      const std::vector<time::time_period> & slices() const;
 
      const std::vector<time::time_period> & breaks() const;
      
      bool contains(const time::time_point & timestamp_) const;

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

      void load(const datatools::properties & config_);
      
    private:

      void _sync_();
      
      run_id_type _run_id_ = INVALID_RUN_ID;
      run_category _category_ = run_category::INDETERMINATE;
      time::time_period _period_{time::time_point(time::not_a_date_time),
                                 time::time_point(time::not_a_date_time)};
      std::vector<time::time_period> _breaks_;
      std::vector<time::time_period> _slices_;
      std::uint32_t _number_of_events_ = 0;

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
