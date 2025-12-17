/// \file falaise/snemo/rc/run_phase.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2025-08-27
/// Last modified: 2025-08-27
///

// Ourselves:
#include <falaise/snemo/rc/run_phase.h>

// Standard library:
#include <bitset>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace rc {
    
    void run_phase::reset()
    {
      _id_ = INVALID_PHASE_ID;
      _start_time_ = time::invalid_point();
      _stop_time_ = time::invalid_point();
      _status_ = good;
      return;
    }

    void run_phase::set_id(const run_phase_id_type id_)
    {
      _id_ = id_;
      return;
    }

    run_phase_id_type run_phase::id() const
    {
      return _id_;
    }

    void run_phase::set_start_time(const time::time_point & start_time_)
    {
      _start_time_ = start_time_;
      if (not time::is_valid(_stop_time_)) {
        _stop_time_ = boost::date_time::pos_infin;
      }
      return;
    }

    void run_phase::set_stop_time(const time::time_point & stop_time_)
    {
      if (time::is_valid(_start_time_)) {
        if (stop_time_ <= _start_time_) {
          DT_THROW(std::range_error, "Invalid stop time <= start time")
	    }
      } else {
        _start_time_ = boost::date_time::neg_infin;
      }
      _stop_time_ = stop_time_;
      return;
    }

    const time::time_point & run_phase::start_time() const
    {
      return _start_time_;
    }

    const time::time_point & run_phase::stop_time() const
    {
      return _stop_time_;
    }

    bool run_phase::has_valid_period() const
    {
      if (not time::is_valid(_start_time_)) return false;
      if (not time::is_valid(_stop_time_)) return false;
      return true;
    }
                                
    bool run_phase::has_closed_period() const
    {
      return has_valid_period() and _start_time_ != time::neg_infin and _stop_time_ != time::pos_infin;
    }
 
    time::time_period run_phase::period() const
    {
      DT_THROW_IF(not has_valid_period(), std::logic_error,
                  "Invalid phase period");
      return time::time_period(_start_time_, _stop_time_);
    }

    /*
      void run_phase::add_run_id(const run_id_type run_id_)
      {
      DT_THROW_IF(run_id_ < 0, std::logic_error, "Invalid run ID");
      if (_run_ids_.size()) {
      DT_THROW_IF(run_id_ <= _run_ids_.back(),
      std::logic_error, "Invalid backward run ID");
      }
      _run_ids_.push_back(run_id_);
      return;
      }

      const std::vector<run_id_type> & run_phase::run_ids() const
      {
      return _run_ids_;
      }
   
      std::size_t run_phase::number_of_runs() const
      {
      return _run_ids_.size();
      }
 
      run_id_type run_phase::first_run_id() const
      {
      DT_THROW_IF(_run_ids_.size() == 0, std::logic_error, "Empty run phase");
      return _run_ids_.front();
      }

      run_id_type run_phase::last_run_id() const
      {
      DT_THROW_IF(_run_ids_.size() == 0, std::logic_error, "Empty run phase");
      return _run_ids_.back();
      }
    */
    
    void run_phase::set_status(const std::uint32_t status_)
    {
      _status_ = status_;
      return;
    }

    std::uint32_t run_phase::status() const
    {
      return _status_;
    }
                
    bool run_phase::status_is_good() const
    {
      return (_status_ & 0xFFFF) == 0u;
    }
    
    bool run_phase::contains(const time::time_point & timestamp_) const
    {
      if (not has_valid_period()) return false;
      return this->period().contains(timestamp_);
    }
    
    bool run_phase::has_run(const run_description & rundesc_) const
    {
      if (not has_valid_period()) return false;
      return this->period().contains(rundesc_.period());
    }
                
    void run_phase::print_tree(std::ostream & out_,
                               const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      
      out_ << popts.indent << tag
           << "Phase ID : "
           << _id_
           << std::endl;
     
      out_ << popts.indent << tag
           << "Start time : ";
      if (time::is_valid(_start_time_)) {
        out_ << time::to_string(_start_time_);
      } else {
        out_ << "unset";
      }
      out_ << std::endl;
    
      out_ << popts.indent << tag
           << "Stop time : ";
      if (time::is_valid(_stop_time_)) {
        out_ << time::to_string(_stop_time_);
      } else {
        out_ << "unset";
      }
      out_ << std::endl;

      out_ << popts.indent << tag
           << "Valid period: " << std::boolalpha << has_valid_period()
           << std::endl;

      out_ << popts.indent << tag
           << "Closed period: " << std::boolalpha << has_closed_period()
           << std::endl;

      // out_ << popts.indent << tag
      //           << "Number of runs : "
      //           << number_of_runs()
      //           << std::endl;
      
      // if (_run_ids_.size()) {        
      //        out_ << popts.indent << tag
      //             << "First run ID : "
      //             << _run_ids_.front()
      //             << std::endl;
        
      //        out_ << popts.indent << tag
      //             << "Last run ID : "
      //             << _run_ids_.back()
      //             << std::endl;
      // } else {
      //        out_ << popts.indent << tag
      //             << "No runs."
      //             << std::endl;
      // }

      out_ << popts.indent << tag
           << "Good : " << std::boolalpha << status_is_good()
           << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Status : " << std::bitset<32>(_status_)
           << std::endl;
     
      return;
    }

  } // end of namespace rc

} // end of namespace snemo
