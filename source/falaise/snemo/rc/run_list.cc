/// \file falaise/snemo/rc/run_list.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-27
/// Last modified: 2022-04-27
///

// Ourselves:
#include <falaise/snemo/rc/run_list.h>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace rc {

    void run_list::reset()
    {
      _runs_.clear();
      _span_ = time::time_period{time::time_point(time::not_a_date_time),
                                 time::time_point(time::not_a_date_time)};
      _duration_ = time::time_duration{time::not_a_date_time};
      _number_of_events_ = 0;
      _last_run_id_ = -1;
      return;
    }

    bool run_list::has_run(std::int32_t run_id_) const
    {
      return _runs_.count(run_id_) == 1;
    }
       
    const run_description & run_list::get_run(std::int32_t run_id_) const
    {
      const auto & found = _runs_.find(run_id_);
      DT_THROW_IF(found == _runs_.end(), std::logic_error, "No run with ID=" << run_id_ << "!");
      return found->second;
    }

    void run_list::add_run(const run_description & rd_)
    {
      DT_THROW_IF(! rd_.is_valid(), std::logic_error, "Invalid run description!");
      bool can_add = true;
      if (_last_run_id_ >= 0) {
        DT_THROW_IF(rd_.run_id() <= _last_run_id_, std::logic_error, "Run ID=" << rd_.run_id() << " does not follow last run ID=" << _last_run_id_ << "!");
        const run_description & lastRD = get_run(_last_run_id_);
        DT_THROW_IF(rd_.begin() < lastRD.end(), std::logic_error, "Period for run " << rd_.run_id() << " does not follow period of last run " << _last_run_id_ << "!");
      } 
      DT_THROW_IF(! can_add, std::logic_error, "Cannot add run description [run_id=" << rd_.run_id() << "]!");
      _runs_.emplace(rd_.run_id(), rd_);
      const_cast<run_list*>(this)->_sync_();
      _last_run_id_ = rd_.run_id();
      return;
    }
    
    time::time_duration run_list::duration() const
    {
      return _duration_;
    }
    
    time::time_period run_list::span() const
    {
      return _span_;
    }
    
    bool run_list::contains(const time::time_point & timestamp_) const
    {
      if (not _span_.contains(timestamp_)) return false;
      for (const auto & rd : _runs_) {
        if (rd.second.contains(timestamp_)) return true;
      } 
      return false;
    }
   
    std::int32_t run_list::locate_run(const time::time_point & timestamp_) const
    {
      for (const auto & rd : _runs_) {
        if (rd.second.contains(timestamp_)) {
          return rd.first;
        }
      } 
      return -1;
    }
    
    void run_list::build_run_ids(std::set<std::int32_t> & run_ids_) const
    {
      run_ids_.clear();
      for (const auto & rd : _runs_) {
        run_ids_.insert(rd.first);
      }
      return;
    }
    
    void run_list::_compute_duration_()
    {
      time::time_duration rld(time::not_a_date_time); 
      for (const auto & rd : _runs_) {
        if (rld.is_not_a_date_time()) {
          rld = boost::posix_time::seconds(0);
        }
        rld += rd.second.duration();
      }
      _duration_ = rld;
      return;
    }

    void run_list::_compute_span_()
    {
      time::time_period rlSpan{time::time_point(time::not_a_date_time),
                               time::time_point(time::not_a_date_time)};
      for (const auto & rd : _runs_) {
        if (rlSpan.is_null()) {
          rlSpan = rd.second.period();
        } else {
          rlSpan = rlSpan.span(rd.second.period());
        }
      }
      _span_ = rlSpan;
      return;
    }

    void run_list::_compute_number_of_events_()
    {
      std::uint32_t noe = 0;
      for (const auto & rd : _runs_) {
        noe +=  rd.second.number_of_events();
      }
      _number_of_events_ = noe;
      return;
    }
  
    void run_list::_sync_()
    {
      _compute_span_();
      _compute_duration_();
      _compute_number_of_events_();
      return;
    }
            
    // virtual
    void run_list::print_tree(std::ostream & out_,
                              const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      
      out_ << popts.indent << tag
           << "Runs : " << std::endl;
      unsigned int rdCount = 0;
      for (const auto & rd : _runs_) {
        out_ << popts.indent << skip_tag;
        if (rdCount + 1 < _runs_.size()) {
          out_ << popts.indent << tag;
        } else { 
          out_ << popts.indent << last_tag;
        }
        out_ << "Run " << rd.first
             << std::endl;
        {
          boost::property_tree::ptree runOpts;
          std::ostringstream indentItem;
          indentItem << popts.indent << skip_tag;
          if (rdCount + 1 < _runs_.size()) {
            indentItem << skip_tag;
          } else {
            indentItem << last_skip_tag;
          }
          runOpts.put("indent", indentItem.str());
          rd.second.print_tree(out_, runOpts);
        }
        rdCount++;
      }

      out_ << popts.indent << tag
           << "Span : "
           << boost::posix_time::to_simple_string(span())
           << std::endl;

      out_ << popts.indent << tag
           << "Duration : "
           << boost::posix_time::to_simple_string(duration()) << " (=" << time::to_quantity(duration()) / CLHEP::second << " s)"
           << std::endl;
 
      out_ << popts.indent << tag
           << "Number of events : "
           << number_of_events()
           << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Last run ID : "
           << _last_run_id_
           << std::endl;
    
      return;
    }
    
  }  // end of namespace rc

}  // end of namespace snemo
