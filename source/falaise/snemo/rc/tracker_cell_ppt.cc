/// \file falaise/snemo/rc/tracker_cell_ppt.cc

// Ourselves:
#include <falaise/snemo/rc/tracker_cell_ppt.h>

// Bayeux/datatools:
#include <bayeux/datatools/clhep_units.h>

// Standard library:
#include <sstream>

namespace snemo {

  namespace rc {

    // static
    std::string to_string(const tracker_cell_ppt_type ppt_type_)
    {
      std::string repr;
      if (ppt_type_ == tracker_cell_ppt_type::normal) {
         repr = "normal";
      } else if (ppt_type_ == tracker_cell_ppt_type::missing_bottom) {
         repr = "missing_bottom";
      } else if (ppt_type_ == tracker_cell_ppt_type::missing_top) {
         repr = "missing_top";
      } else if (ppt_type_ == tracker_cell_ppt_type::inoperative) {
         repr = "inoperative";
      }
      return repr;
    }
 
    bool tracker_cell_ppt_info::is_valid() const
    {
      if (type == tracker_cell_ppt_type::inoperative) return false;
      if (ppt != ppt) return false;
      if (ppt_error != ppt_error) return false;
      return true;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const tracker_cell_ppt_info & ppt_info_)
    {
      std::ostringstream sout;
      const tracker_cell_ppt_info & pptInfo = ppt_info_;
      if (pptInfo.is_valid()) {
	sout << "{type='" << to_string(pptInfo.type) << "'"
	     << ";ppt=" << pptInfo.ppt / CLHEP::microsecond << "[us]"
	     << ";ppt_error=" << pptInfo.ppt_error / CLHEP::microsecond << "[us]";
	sout << '}';
      } else {
	sout << "<invalid>";
      }
      out_ << sout.str();
      return out_;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const tracker_cell_ppt_record & record_)
    {
      std::ostringstream sout;
      const tracker_cell_ppt_info & pptInfo = record_.info;
      sout << '@' << snemo::time::to_string(record_.period)
	   << ":ppt_info=" << pptInfo;
      out_ << sout.str();
      return out_;
    }

    tracker_cell_ppt_history::tracker_cell_ppt_history(const std::vector<tracker_cell_ppt_record> & records_)
      : _records_(records_)
    {
      return;
    }

    void tracker_cell_ppt_history::set(const std::vector<tracker_cell_ppt_record> & records_)
    {
      _records_ = records_;
      return;
    }
 
    void tracker_cell_ppt_history::add(const time::time_period & period_, const tracker_cell_ppt_info & ppt_info_)
    {
      if (_records_.size() and period_.begin() < _records_.back().period.end()) {
        DT_THROW(std::domain_error, "New period does not follow last record!");
      }
      _records_.push_back({period_, ppt_info_});
      return;
    }
   
    const std::vector<tracker_cell_ppt_record> &
    tracker_cell_ppt_history::records() const
    {
      return _records_;
    }

    // static
    tracker_cell_ppt_info tracker_cell_ppt_info::make_inoperative()
    {
      return {snemo::rc::tracker_cell_ppt_type::inoperative,
	      std::numeric_limits<double>::quiet_NaN(),
	      std::numeric_limits<double>::quiet_NaN()};
    }
    
    const tracker_cell_ppt_info & tracker_cell_ppt_history::get_ppt_info(const time::time_point & t_) const
    {
      static const tracker_cell_ppt_info _pptInfo = tracker_cell_ppt_info::make_inoperative();
      for (const auto & rec : _records_ ) {
        if (rec.period.contains(t_)) {
          return rec.info;
        }
      }
      return _pptInfo; 
    }

    void tracker_cell_ppt_history::clear()
    {
      _records_.clear();
      return;
    }
  
    void tracker_cell_ppt_history::print(std::ostream & out_, const std::string & indent_) const
    {
      std::string indent = indent_;
      out_ << indent << "History records:\n";
      auto count = 0u;
      for (const auto & rec : _records_) {
	out_ << indent << " - Record #" << count << " : " << rec << '\n';
	count++;
      }
      return;
    }

  } // end of namespace rc

} // end of namespace snemo
