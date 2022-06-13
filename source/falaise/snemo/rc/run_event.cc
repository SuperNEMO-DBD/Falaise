/// \file falaise/snemo/rc/run_event.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-04-27
/// Last modified: 2022-04-27
///

// Ourselves:
#include <falaise/snemo/rc/run_event.h>

// Bayeux:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace rc {

    std::ostream & operator<<(std::ostream & out_, run_event_category evcat_)
    {
      switch (evcat_) {
      case run_event_category::INDETERMINATE :
        out_ << "indeterminate";
        break;
      case run_event_category::START :
        out_ << "start";
        break;
      case run_event_category::PAUSE :
        out_ << "pause";
        break;
      case run_event_category::RESUME :
        out_ << "resume";
        break;
      case run_event_category::STOP :
        out_ << "stop";
        break;
      case run_event_category::CRASH :
        out_ << "crash";
        break;
      }
      return out_;
    }
    
    run_event::run_event(const time::time_point & timestamp_, run_event_category evcat_, std::int32_t run_id_)
      : base_event(timestamp_)
      , _category_(evcat_)
      , _run_id_(run_id_)
    {
      return;
    }
    
    run_event::run_event(const time::time_point & timestamp_, const time::time_duration & duration_,
                         run_event_category evcat_, std::int32_t run_id_)
      : base_event(timestamp_, duration_)
      , _category_(evcat_)
      , _run_id_(run_id_)
    {
      return;
    }
  
    // virtual
    void run_event::print_tree(std::ostream & out_,
                               const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      this->base_event::print_tree(out_, base_print_options::force_inheritance(options_));
     
      out_ << popts.indent << tag
           << "Category : '"
           << _category_ << "'"
           << std::endl;
     
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Run ID : "
           << _run_id_
           << std::endl;
                                        
      return;
    }
    
  } // end of namespace rc

} // end of namespace snemo
