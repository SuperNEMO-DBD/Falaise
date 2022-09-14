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

    run_category from_string(const std::string & label_)
    {
      if (label_ == "test") return run_category::TEST;
      if (label_ == "commissioning") return run_category::COMMISSIONING;
      if (label_ == "production") return run_category::PRODUCTION;
      if (label_ == "calibration1") return run_category::CALIBRATION_1;
      if (label_ == "calibration2") return run_category::CALIBRATION_2;
      if (label_ == "calibration3") return run_category::CALIBRATION_3;
      if (label_ == "calibration4") return run_category::CALIBRATION_4;
      return run_category::INDETERMINATE;
    }

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

    void run_description::set_run_id(std::int32_t id_)
    {
      DT_THROW_IF(id_ < INVALID_RUN_ID, std::domain_error,
                  "Invalid run ID '" << id_ << "'!");
      _run_id_ = id_;
      return;
    }
  
    std::int32_t run_description::run_id() const
    {
      return _run_id_;
    }


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
        DT_THROW_IF(break_.begin() <= _breaks_.back().end(), std::domain_error, "Break overlaps/expands last break!");
      }
      _breaks_.push_back(break_);
      _sync_();
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

    void run_description::load(const datatools::properties & config_)
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      if (config_.has_flag("debug")) {
        logging = datatools::logger::PRIO_DEBUG;
      }
      
      if (config_.has_key("category")) {
        std::string catLabel = config_.fetch_string("category");
        run_category rc = from_string(catLabel);
        DT_THROW_IF(rc == run_category::INDETERMINATE, std::logic_error,
                    "Invalid run category label '" << catLabel << "'!");
        _category_ = rc;
      }
      DT_THROW_IF(_category_ == run_category::INDETERMINATE,
                  std::logic_error,
                  "Run category is not set!");
      DT_LOG_DEBUG(logging, "Category = '" << _category_ << "'");
      
      if (config_.has_key("period")) {
        std::string periodRepr = config_.fetch_string("period");
        auto runPeriod = time::time_period_from_string(periodRepr);
        DT_THROW_IF(not time::is_valid(runPeriod), std::logic_error,
                    "Invalid run period '" << periodRepr << "'!");
        _period_ = runPeriod;
      }
      DT_THROW_IF(not time::is_valid(_period_), std::logic_error,
                  "Run period is not set!");
      DT_LOG_DEBUG(logging, "Run period = '"
                   << time::to_string(_period_, time::TIME_PERIOD_FORMAT_EXCLUDE_END) << "'");

      if (config_.has_key("number_of_events")) {
        std::uint32_t number_of_events = config_.fetch_positive_integer("number_of_events");
        _number_of_events_ = number_of_events;
      }
      DT_LOG_DEBUG(logging, "Number of events = " << _number_of_events_);

      if (config_.has_key("number_of_breaks")) {
        std::uint32_t number_of_breaks = config_.fetch_positive_integer("number_of_breaks");
        for (std::uint32_t iBreak = 0; iBreak < number_of_breaks; iBreak++) {
          std::string breakPeriodKey = "break_" + std::to_string(iBreak) + ".period";
          DT_THROW_IF(not config_.has_key(breakPeriodKey), std::logic_error,
                      "Missing run break period '" << breakPeriodKey << "'!");
          std::string breakPeriodRepr = config_.fetch_string(breakPeriodKey);
          auto breakPeriod = time::time_period_from_string(breakPeriodRepr);
          DT_THROW_IF(not time::is_valid(breakPeriod), std::logic_error,
                      "Invalid run break period '" << breakPeriodRepr << "'!");
          add_break(breakPeriod);
        }
      } else if (config_.has_key("breaks")) {
        std::vector<std::string> breakPeriodReprs;
        config_.fetch("breaks", breakPeriodReprs);
        for (unsigned int iBreak = 0; iBreak < breakPeriodReprs.size(); iBreak++) {
          const std::string & breakPeriodRepr = breakPeriodReprs[iBreak];
          auto breakPeriod = time::time_period_from_string(breakPeriodRepr);
          DT_THROW_IF(not time::is_valid(breakPeriod), std::logic_error,
                      "Invalid run break period '" << breakPeriodRepr << "'!");
          add_break(breakPeriod);         
        }
      }
      
      return;
    }

    void run_description::_sync_()
    {
      /*
       *
       *     b                                             e       run
       *  ---[---[====[--------------[==============[------[-------->
       *         b0   e0             b1             e1             pauses
       *    Slices:
       *     [.............................................[    step 0
       *     [...[    [....................................[    step 1
       *     [...[    [..............[              [......[    step 2
       *   
       */
      _slices_.clear();
      _slices_.push_back(time::time_period(this->begin(), this->end()));
      for (const auto & pause : this->breaks()) {
        time::time_period & lastSlice = _slices_.back();
        if (pause.begin() == lastSlice.begin() and pause.end() < lastSlice.end()) {
          time::time_period updatedSlice(pause.end(), lastSlice.end());
          lastSlice = updatedSlice;
        } else if (pause.begin() > lastSlice.begin()) {
          if (pause.end() < lastSlice.end()) {
            time::time_period updatedSlice(lastSlice.begin(), pause.begin());
            time::time_period nextSlice(pause.end(), lastSlice.end());
            lastSlice = updatedSlice;
            _slices_.push_back(nextSlice);
          } else {
            time::time_period updatedSlice(lastSlice.begin(), pause.begin());
            lastSlice = updatedSlice;
          }
        }
      }
      return;
    }
 
    time::time_duration run_description::duration() const
    {
      time::time_duration rd = _period_.length();
      for (const auto & aBreak : _breaks_) {
        rd -= aBreak.length();
      }
      return rd;
    }
  
    std::uint32_t run_description::number_of_events() const
    {
      return _number_of_events_;
    }

    bool run_description::has_slices() const
    {
      // return duration().is_positive() and not duration().is_zero();
      return duration().ticks() > 0;
    }
    
    const std::vector<time::time_period> & run_description::slices() const
    {
      if (has_slices() and _slices_.size() == 0) {
        // Slices have not been computed yet:
        run_description * mutableThis = const_cast<run_description*>(this);
        mutableThis->_sync_();
      }
      return _slices_;
    }
 
    bool run_description::has_breaks() const
    {
      return _breaks_.size() > 0;
    }
    
    const std::vector<time::time_period> & run_description::breaks() const
    {
      return _breaks_;
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
           << time::to_string(_period_, time::TIME_PERIOD_FORMAT_EXCLUDE_END)
           << " (duration : " << time::to_string(_period_.length()) << " , "
           << std::setprecision(15) << time::to_quantity(_period_.length()) / CLHEP::second << " s)"
           << std::endl;
    
      if (has_breaks()) {
        out_ << popts.indent << tag
             << "Breaks : " << std::endl;
        unsigned int brkCount = 0;
        for (const auto & aBreak : _breaks_) {
          out_ << popts.indent << skip_tag;
          if (brkCount + 1 < _breaks_.size()) {
            out_ << tag;
          } else { 
            out_ << last_tag;
          }
          out_ << "Period #" << brkCount << " : ";
          out_ << time::to_string(aBreak, time::TIME_PERIOD_FORMAT_EXCLUDE_END)
               << " (duration : " << time::to_string(aBreak.length()) << ")"
               << std::endl;
          brkCount++;
        }
      }
    
      if (has_slices()) {
        out_ << popts.indent << tag
             << "Slices : " << std::endl;
        unsigned int sliceCount = 0;
        for (const auto & aSlice : _slices_) {
          out_ << popts.indent << skip_tag;
          if (sliceCount + 1 < _slices_.size()) {
            out_ << tag;
          } else { 
            out_ << last_tag;
          }
          out_ << "Period #" << sliceCount << " : ";
          out_ << time::to_string(aSlice, time::TIME_PERIOD_FORMAT_EXCLUDE_END)
               << " (duration : " << time::to_string(aSlice.length()) << ")"
               << std::endl;
          sliceCount++;
        }
      }
        
      out_ << popts.indent << tag
           << "Duration : "
           << time::to_string(duration())
           << " (=" << time::to_quantity(duration()) / CLHEP::second << " s)"
           << std::endl;
  
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Number of events : "
           << _number_of_events_
           << std::endl;

      return;
    } 
    
  }  // end of namespace rc

}  // end of namespace snemo
