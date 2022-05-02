/// \file falaise/snemo/rc/run_list.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2022-04-28
 *
 * Description: Description of a list of successive runs
 */

#ifndef FALAISE_SNEMO_RC_RUN_LIST_H
#define FALAISE_SNEMO_RC_RUN_LIST_H

// Standard library:
#include <cstdint>
#include <iostream>
#include <map>
#include <set>

// Boost
// #include <boost/date_time/posix_time/posix_time.hpp>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/rc/run_description.h>
#include <falaise/snemo/time/time_utils.h>

namespace snemo {

  namespace rc {

    /// \brief List of runs ordered by run IDs and time periods
    class run_list
      : public datatools::i_tree_dumpable
    {
    public:
     
      run_list() = default;

      virtual ~run_list() = default;

      inline bool is_empty() const
      {
        return _runs_.size() == 0;
      }

      inline bool size() const
      {
        return _runs_.size();
      }

      void reset();

      bool has_run(std::int32_t run_id_) const;
      
      const run_description & get_run(std::int32_t run_id_) const;

      void add_run(const run_description & rd_);

      time::time_duration duration() const;
  
      time::time_period span() const;

      inline std::uint32_t number_of_events() const
      {
        return _number_of_events_;
      }
   
      inline std::int32_t last_run_id() const
      {
        return _last_run_id_;
      }
      
      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

      void build_run_ids(std::set<std::int32_t> & run_ids_) const;
     
      bool contains(const time::time_point & timestamp_) const;
     
      std::int32_t locate_run(const time::time_point & timestamp_) const;
      
    private:

      void _compute_duration_();

      void _compute_span_();
 
      void _compute_number_of_events_();
 
      void _sync_();
      
    private:
      
      std::map<std::int32_t, run_description> _runs_;
      time::time_period _span_{time::time_point(time::not_a_date_time), time::time_point(time::not_a_date_time)};
      time::time_duration _duration_{time::not_a_date_time};
      std::uint32_t _number_of_events_ = 0;
      std::int32_t _last_run_id_ = -1;

    };
   
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_RUN_LIST_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

