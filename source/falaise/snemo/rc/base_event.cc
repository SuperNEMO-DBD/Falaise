/// \file falaise/snemo/rc/base_event.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-27
/// Last modified: 2022-04-27
///

// Ourselves:
#include <falaise/snemo/rc/base_event.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace rc {
     
    base_event::base_event(const time::time_point & timestamp_)
    {
      set_timestamp(timestamp_);
      return;
    }
      
    base_event::base_event(const time::time_point & timestamp_, const time::time_duration & duration_)
    {
      set_timestamp(timestamp_);
      set_duration(duration_);
      return;
    }

    void base_event::set_timestamp(const time::time_point & timestamp_)
    {
      DT_THROW_IF(timestamp_.is_not_a_date_time(),
                  std::logic_error, "Invalid event timestamp!");
      _timestamp_ = timestamp_;
      return;
    }

    void base_event::set_duration(const time::time_duration & duration_)
    {
      DT_THROW_IF(duration_.is_not_a_date_time() or duration_.is_negative(),
                  std::logic_error, "Invalid event duration!");
      _duration_ = duration_;
      return;
    }
    
    void base_event::reset()
    {
      time::invalidate(_timestamp_);
      time::invalidate(_duration_);
      return;
    }
    
    // virtual
    void base_event::print_tree(std::ostream & out_,
                                const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      
      out_ << popts.indent << tag
           << "Timestamp : "
           << time::to_string(_timestamp_)
           << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Duration : ";
      if (not is_instantaneous()) {
        out_ << time::to_string(_duration_);
      } else {
        out_ << "'instantaneous'";      
      }
      out_ << std::endl;
   
      return;
    }
  
  } // end of namespace rc

} // end of namespace snemo
