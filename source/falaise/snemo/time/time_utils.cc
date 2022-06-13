#include <falaise/snemo/time/time_utils.h>

// Standard library:
#include <sstream>
#include <memory>

// Boost:
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/algorithm/string.hpp>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace time {

    std::locale & the_date_locale()
    {
      static std::unique_ptr<std::locale> _locale
        = std::make_unique<std::locale>(std::locale::classic(),
                                        new date_facet("%Y-%m-%d"));
      return *_locale;
    }

    std::locale & the_date_locale_input()
    {
      static std::unique_ptr<std::locale> _locale
        = std::make_unique<std::locale>(std::locale::classic(),
                                        new date_input_facet("%Y-%m-%d"));
      return *_locale;
    }

    std::locale & the_time_point_locale()
    {
      static std::unique_ptr<std::locale> _locale
        = std::make_unique<std::locale>(std::locale::classic(),
                                        new time_facet("%Y-%m-%d %H:%M:%S%F"));
      return *_locale;
    }

    std::locale & the_time_point_locale_input()
    {
      static std::unique_ptr<std::locale> _locale
        = std::make_unique<std::locale>(std::locale::classic(),
                                        new time_input_facet("%Y-%m-%d %H:%M:%S%F"));
      return *_locale;
    }

    std::locale & the_time_duration_locale()
    {
      static std::unique_ptr<std::locale> _locale
        = std::make_unique<std::locale>(std::locale::classic(),
                                        new date_facet("%H:%M:%S%F"));
      return *_locale;
    }

    std::locale & the_time_duration_locale_input()
    {
      static std::unique_ptr<std::locale> _locale
        = std::make_unique<std::locale>(std::locale::classic(),
                                        new date_input_facet("%H:%M:%S%F"));
      return *_locale;
    }

    double to_quantity(const time_duration & td_)
    {
      return td_.total_microseconds() * CLHEP::microsecond;
    }

    std::string to_string(const time_point & t_)
    {
      std::ostringstream sout;
      sout.imbue(std::locale(the_time_point_locale()));
      sout << t_;
      return sout.str();
    }

    std::string to_string(const time_duration & td_)
    {
      std::ostringstream sout;
      sout.imbue(std::locale(the_time_duration_locale()));
      sout << td_;
      return sout.str();
    }

    std::string to_string(const time_period & tp_, const time_period_format_type format_)
    {
      std::ostringstream sout;
      sout.imbue(std::locale(the_time_point_locale()));
      if (is_valid(tp_)) {
        if (format_ == TIME_PERIOD_FORMAT_INCLUDE_LAST) {
          sout << tp_;
        } else if (format_ == TIME_PERIOD_FORMAT_BEGIN_DURATION) {
          sout << '{';
          sout << to_string(tp_.begin());
          sout << '+';
          sout << to_string(tp_.length());
          sout << '}';
        } else if (format_ == TIME_PERIOD_FORMAT_EXCLUDE_END) {
          sout << '[';
          sout << to_string(tp_.begin());
          sout << '/';
          sout << to_string(tp_.end());
          sout << ')';
        }
      } else {
        sout << '!';
      }
      return sout.str();
    }

    time_point time_point_from_string(const std::string & repr_)
    {
      time_point tp = invalid_point();
      std::istringstream iss(repr_);
      iss.imbue(std::locale(the_time_point_locale_input()));
      iss >> tp;
      DT_THROW_IF(!iss, std::range_error, "Invalid time point format '" << repr_ << "'!");
      return tp;
    }

    time_duration time_duration_from_string(const std::string & repr_)
    {
      time_duration td = invalid_duration();
      std::istringstream iss(repr_);
      iss.imbue(std::locale(the_time_duration_locale_input()));
      iss >> td;
      DT_THROW_IF(!iss, std::range_error, "Invalid time duration format '" << repr_ << "'!");
      return td;
    }

    time_period time_period_from_string(const std::string & repr_)
    {
      bool devel = false;
      // devel = true;
      time_period tp = invalid_period();
      std::string repr = repr_;
      boost::trim(repr);
      DT_THROW_IF(repr.size() < 2, std::logic_error, "Invalid time period format '" << repr_ << "'!");
      if ((repr[0] == '{') and repr.back() == '}') {
        std::string intRepr = repr.substr(1, repr.size() - 2);
        if (devel) std::cerr << "[devel] intRepr='" << intRepr << "\n";
        size_t plusPos = intRepr.find_first_of("+", 0);
        if (plusPos == intRepr.npos) {
          DT_THROW_IF(repr.size() < 2, std::logic_error, "Invalid time period {point+duration} format as '" << repr_ << "'!");
        }
        std::string begRepr = intRepr.substr(0, plusPos);
        boost::trim(begRepr);
        std::string durationRepr = intRepr.substr(plusPos + 1);
        boost::trim(durationRepr);
        if (devel) std::cerr << "[devel] begRepr='" << begRepr << "'\n";
        if (devel) std::cerr << "[devel] durationRepr='" << durationRepr << "'\n";
        time_point periodBegin = time_point_from_string(begRepr);
        DT_THROW_IF(!is_valid(periodBegin), std::range_error, "Invalid begin time point format '" << begRepr << "'!");
        time_duration periodDuration = time_duration_from_string(durationRepr);
        DT_THROW_IF(!is_valid(periodDuration), std::range_error, "Invalid time duration format '" << durationRepr << "'!");
        tp = time_period(periodBegin, periodDuration);
      } else if ((repr[0] == '[') and repr.back() == ')') {
        std::string intRepr = repr.substr(1, repr.size() - 2);
        if (devel) std::cerr << "[devel] intRepr='" << intRepr << "\n";
        size_t slashPos = intRepr.find_first_of("/", 0);
        if (slashPos == intRepr.npos) {
          DT_THROW_IF(repr.size() < 2, std::logic_error, "Invalid time period [begin/end) format as '" << repr_ << "'!");
        }
        std::string begRepr = intRepr.substr(0, slashPos);
        boost::trim(begRepr);
        std::string endRepr = intRepr.substr(slashPos + 1);
        boost::trim(endRepr);
        if (devel) std::cerr << "[devel] begRepr='" << begRepr << "'\n";
        if (devel) std::cerr << "[devel] endRepr='" << endRepr << "'\n";
        time_point periodBegin = time_point_from_string(begRepr);
        DT_THROW_IF(!is_valid(periodBegin), std::range_error, "Invalid begin time point format '" << begRepr << "'!");
        time_point periodEnd = time_point_from_string(endRepr);
        DT_THROW_IF(!is_valid(periodEnd), std::range_error, "Invalid end time point format '" << endRepr << "'!");
        tp = time_period(periodBegin, periodEnd);
      } else {
        std::istringstream iss(repr_);
        iss.imbue(std::locale(the_time_point_locale_input()));  
        iss >> tp;
        DT_THROW_IF(!iss, std::range_error, "Invalid time period format '" << repr_ << "'!");
      }
      return tp;
    }
 
    time_duration from_quantity(double duration_)
    {
      DT_THROW_IF(duration_ < 0.0, std::range_error, "Invalid negative duration!");
      int nticks = (int) (duration_ / CLHEP::microsecond);
      return time::microseconds(nticks);
    }

    bool is_valid(const time_point & t_)
    {
      return ! t_.is_not_a_date_time();
    }
 
    bool is_valid(const time_duration & td_)
    {
      return ! td_.is_not_a_date_time();      
    }

    bool is_valid(const time_period & tp_)
    {
      if (tp_.is_null()) return false;
      if (! is_valid(tp_.begin())) return false;
      if (! is_valid(tp_.end())) return false;
      return true;
    }

    time_point invalid_point()
    {
      return time_point(not_a_date_time);
    }

    time_duration invalid_duration()
    {
      return time_duration(not_a_date_time);
    }

    time_period invalid_period()
    {
      return time_period(invalid_point(), invalid_point());
    }

    date invalid_date()
    {
      return date(not_a_date_time);
    }
  
    date_period invalid_date_period()
    {
      return date_period(invalid_date(), invalid_date());
    }

  }  // end of namespace time

}  // end of namespace snemo

