/// \file falaise/snemo/time/time_utils.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2022-04-28
 *
 * Description: Time utilities
 */

#ifndef FALAISE_SNEMO_TIME_TIME_UTILS_H
#define FALAISE_SNEMO_TIME_TIME_UTILS_H

// Boost:
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace time {

    using boost::posix_time::time_period   ;
    using boost::posix_time::time_duration ;
    using time_point = boost::posix_time::ptime;
    using boost::posix_time::hours         ;
    using boost::posix_time::minutes       ;
    using boost::posix_time::seconds       ;
    using boost::posix_time::milliseconds  ;
    using boost::posix_time::microseconds  ;
    using boost::gregorian::date           ;
    using boost::gregorian::date_duration  ;
    using boost::gregorian::date_period    ;
    using boost::gregorian::days           ;
    using boost::date_time::not_a_date_time;
    using boost::gregorian::date_facet;
    using boost::posix_time::time_facet;
    using boost::gregorian::date_input_facet;
    using boost::posix_time::time_input_facet;

    std::locale & the_date_locale();

    std::locale & the_date_locale_input();

    std::locale & the_time_point_locale();

    std::locale & the_time_point_locale_input();

    std::locale & the_time_duration_locale();

    std::locale & the_time_duration_locale_input();
 
    double to_quantity(const time_duration & td_);

    time_duration from_quantity(double duration_);
     
    std::string to_string(const time_point & t_);

    std::string to_string(const time_duration & td_);

    std::string to_string(const time_period & tp_);
 
    time_point time_point_from_string(const std::string & repr_);
 
    time_duration time_duration_from_string(const std::string & repr_);
  
    time_period time_period_from_string(const std::string & repr_);

    bool is_valid(const time_point & t_);
 
    bool is_valid(const time_duration & td_);

    bool is_valid(const time_period & tp_);
  
    inline void invalidate(time_point & t_)
    {
      t_ = time_point(not_a_date_time);
    }
    
    inline void invalidate(time_duration & td_)
    {
      td_ = time_duration(not_a_date_time);
    }
    
    inline void invalidate(time_period & tp_)
    {
      tp_ = time_period(time_point(not_a_date_time), time_point(not_a_date_time));
    }
    
    inline void invalidate(date & d_)
    {
      d_ = date(not_a_date_time);
    }
   
    inline void invalidate(date_period & dp_)
    {
      dp_ = date_period(date(not_a_date_time), date(not_a_date_time));
    }

    time_point invalid_point();

    time_duration invalid_duration();

    time_period invalid_period();

    date invalid_date();
  
    date_period invalid_date_period();
  
  }  // end of namespace time

}  // end of namespace snemo

#endif // FALAISE_SNEMO_TIME_TIME_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/


