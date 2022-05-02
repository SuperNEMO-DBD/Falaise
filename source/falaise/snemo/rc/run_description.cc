/// \file falaise/snemo/rc/run_description.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-27
/// Last modified: 2022-04-27
///

// Ourselves:
#include <falaise/snemo/rc/run_description.h>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace rc {

    std::ostream & operator<<(std::ostream & out_, run_category run_cat_)
    {
      switch (run_cat_) {
      case run_category::INDETERMINATE :
        out_ << "indeterminate";
        break;
      case run_category::TEST :
        out_ << "test";
        break;
      case run_category::COMMISSIONING :
        out_ << "commissioning";
        break;
      case run_category::PRODUCTION :
        out_ << "production";
        break;
      case run_category::CALIBRATION_1 :
        out_ << "calibration1";
        break;
      case run_category::CALIBRATION_2 :
        out_ << "calibration2";
        break;
      case run_category::CALIBRATION_3 :
        out_ << "calibration3";
        break;
      case run_category::CALIBRATION_4 :
        out_ << "calibration4";
        break;
      }
      return out_;
    }

    // run_description::run_description()
    // {
    //   _run_id_ = INVALID_RUN_ID;
    //   _category_ = run_category::INDETERMINATE;
    //   _number_of_events_ = 0;
    //   return;
    // }

    // static
    run_description run_description::make(std::int32_t run_id_,
                                          run_category run_cat_,
                                          const time::time_period & run_period_,
                                          std::uint32_t number_of_events_)
    {
      run_description rd;
      rd._run_id_ = run_id_;
      rd._category_ = run_cat_;
      rd._period_ = run_period_;
      rd._number_of_events_ = number_of_events_;
      return rd;
    }

    bool run_description::is_valid() const
    {
      if (_run_id_ == INVALID_RUN_ID) return false;
      if (_category_ == run_category::INDETERMINATE) return false;
      if (_period_.is_null()) return false;
      return true;
    }

    void run_description::reset()
    {
      _run_id_ = INVALID_RUN_ID;
      _category_ = run_category::INDETERMINATE;
      _period_ = time::time_period{time::time_point(time::not_a_date_time),
                                   time::time_point(time::not_a_date_time)};
      _number_of_events_ = 0;
      _breaks_.clear();
      return;
    }

    void run_description::add_break(const time::time_period & break_)
    {
      DT_THROW_IF(break_.is_null(), std::logic_error, "Malformed break period!");
      DT_THROW_IF(break_.begin() < _period_.begin(), std::domain_error, "Break start out of run period!");
      DT_THROW_IF(break_.end() > _period_.end(), std::domain_error, "Break stop out of run period!");
      if (_breaks_.size()) {
        DT_THROW_IF(break_.begin() < _breaks_.back().end(), std::domain_error, "Break overlap with last break!");
      }
      _breaks_.push_back(break_);
      return;
    }
 
    void run_description::add_break(const time::time_duration & break_shift_,
                                    const time::time_duration & break_duration_)
    {
      auto ref = _period_.begin();
      if (has_breaks()) {
        ref = _breaks_.back().end();
      }
      auto breakStart = ref + break_shift_;
      auto breakStop = breakStart + break_duration_;
      add_break(time::time_period(breakStart, breakStop));
      return;
    }

 
    bool run_description::contains(const time::time_point & timestamp_) const
    {
      if (not _period_.contains(timestamp_)) return false;
      for (const auto & pause : _breaks_) {
        if (pause.contains(timestamp_)) return false;
      }
      return true;
    }
     
    // virtual
    void run_description::print_tree(std::ostream & out_,
                                     const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      
      out_ << popts.indent << tag
           << "Run ID : "
           << _run_id_
           << std::endl;
    
      out_ << popts.indent << tag
           << "Run category : '"
           << _category_ << "'"
           << std::endl;
    
      out_ << popts.indent << tag
           << "Period : "
           << time::to_string(_period_)
           << " (duration : " << time::to_string(_period_.length()) << ")"
           << std::endl;

      if (has_breaks()) {
        out_ << popts.indent << tag
             << "Breaks : " << std::endl;
        unsigned int brkCount = 0;
        for (const auto & aBreak : _breaks_) {
          out_ << popts.indent << skip_tag;
          if (brkCount + 1 < _breaks_.size()) {
            out_ << popts.indent << tag;
          } else { 
            out_ << popts.indent << last_tag;
          }
          out_ << time::to_string(aBreak)
               << " (duration : " << time::to_string(aBreak.length()) << ")"
               << std::endl;
          brkCount++;
        }
      }
        
      out_ << popts.indent << tag
           << "Duration : "
           << time::to_string(duration()) << " (=" << time::to_quantity(duration()) / CLHEP::second << " s)"
           << std::endl;
  
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Number of events : "
           << _number_of_events_
           << std::endl;

      return;
    } 
    
  }  // end of namespace rc

}  // end of namespace snemo
