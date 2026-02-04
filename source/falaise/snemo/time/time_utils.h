/// \file falaise/snemo/time/time_utils.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2026-02-02
 *
 * Description: Time utilities
 */

#ifndef FALAISE_SNEMO_TIME_TIME_UTILS_H
#define FALAISE_SNEMO_TIME_TIME_UTILS_H

// Boost:
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

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
    using boost::date_time::neg_infin;
    using boost::date_time::pos_infin;
    using boost::date_time::max_date_time;
    using boost::date_time::min_date_time;
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

		time_point epoch();

		time_duration elapsed_since_epoch(const time_point & t_);

		time_point sn_epoch();

		time_duration elapsed_since_sn_epoch(const time_point & t_);
  
    double to_quantity(const time_duration & td_);
  
    double precision_quantity();

    time_duration from_quantity(const double duration_);
     
    std::string to_string(const time_point & t_);

    std::string to_string(const time_duration & td_);

    enum time_period_format_type
      {
       TIME_PERIOD_FORMAT_INCLUDE_LAST   = 0, ///< [begin/last]
       TIME_PERIOD_FORMAT_EXCLUDE_END    = 1, ///< [begin/end)
       TIME_PERIOD_FORMAT_BEGIN_DURATION = 2, ///< {begin+duration}
       TIME_PERIOD_FORMAT_DEFAULT = TIME_PERIOD_FORMAT_EXCLUDE_END //TIME_PERIOD_FORMAT_INCLUDE_LAST
      };
    
    std::string to_string(const time_period & tp_,
													const time_period_format_type format_ = TIME_PERIOD_FORMAT_DEFAULT);
 
    time_point time_point_from_string(const std::string & repr_);
 
    time_duration time_duration_from_string(const std::string & repr_);
  
    time_period time_period_from_string(const std::string & repr_);

    bool is_valid(const time_point & t_);
 
    bool is_valid(const time_duration & td_);

    bool is_valid(const time_period & tp_);
 
    time_point time_point_pos_infinity();

    time_point time_point_neg_infinity();
  
    void invalidate(time_point & t_);
    
    void invalidate(time_duration & td_);
    
    void invalidate(time_period & tp_);
   
    void invalidate(date & d_);
   
		void invalidate(date_period & dp_);

    time_point invalid_point();

    time_duration invalid_duration();

    time_period invalid_period();

    date invalid_date();
  
    date_period invalid_date_period();

		time_point time_point_from_epoch_sec(const double elapsed_from_epoch_);

		time_duration time_duration_from_sec(const double elapsed_);
		
  } // end of namespace time

} // end of namespace snemo

#endif // FALAISE_SNEMO_TIME_TIME_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/


