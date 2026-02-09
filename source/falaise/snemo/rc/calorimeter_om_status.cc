/// \file falaise/snemo/rc/calorimeter_om_status.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-29
/// Last modified: 2025-07-17
///

// Ourselves:
#include <falaise/snemo/rc/calorimeter_om_status.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/utils.h>

namespace snemo {

  namespace rc {

    // static
    bool calorimeter_om_status::is_off(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_OFF;
    }
 
    // static
    bool calorimeter_om_status::is_dead(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_DEAD;
    }
 
    // static
    bool calorimeter_om_status::is_noisy(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_NOISY;
    }
 
    // static
    bool calorimeter_om_status::is_no_bi_monitoring(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_NO_BI_MONITORING;
    }
 
    // static
    bool calorimeter_om_status::is_no_li_monitoring(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_NO_LI_MONITORING;
    }
  
    // static
    bool calorimeter_om_status::is_bi_unstability_long(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_BI_UNSTABILITY_LONG;
    }
   
    // static
    bool calorimeter_om_status::is_bi_jump(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_BI_JUMP;
    }
  
    // static
    bool calorimeter_om_status::is_bi_screening_se_strips(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_BI_SCREENING_SE_STRIPS;
    }
   
    // static
    bool calorimeter_om_status::is_li_unstability_long(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_LI_UNSTABILITY_LONG;
    }
   
    // static
    bool calorimeter_om_status::is_bi_li_unconsistency_short(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_BI_LI_UNCONSISTENCY_SHORT;
    }
    
    // static
    bool calorimeter_om_status::is_bi_li_unconsistency_long(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_BI_LI_UNCONSISTENCY_LONG;
    }
   
    // static
    bool calorimeter_om_status::is_other_issues(const std::uint32_t status_bits_)
    {
      return status_bits_ & OM_OTHER_ISSUES;
    }
 
    // static
    std::string calorimeter_om_status::status_to_string(const std::uint32_t status_bits_)
    {
      std::ostringstream reprss;
      auto count = 0u;
      if (status_bits_ & OM_DEAD) {
        if (count++) reprss << '+';
        reprss << "dead";
      }
      if (status_bits_ & OM_OFF) {
        if (count++) reprss << '+';
        reprss << "off";
      }
      if (status_bits_ & OM_NOISY) {
        if (count++) reprss << '+';
        reprss << "noisy";
      }
      if (status_bits_ & OM_NO_BI_MONITORING) {
        if (count++) reprss << '+';
        reprss << "no_bi_monitoring";
      }
      if (status_bits_ & OM_NO_LI_MONITORING) {
        if (count++) reprss << '+';
        reprss << "no_li_monitoring";
      }
      if (status_bits_ & OM_BI_UNSTABILITY_LONG) {
        if (count++) reprss << '+';
        reprss << "bi_unstability_long";
      }
      if (status_bits_ & OM_BI_JUMP) {
        if (count++) reprss << '+';
        reprss << "bi_jump";
      }
      if (status_bits_ & OM_BI_SCREENING_SE_STRIPS) {
        if (count++) reprss << '+';
        reprss << "bi_screening_se_strips";
      }
      if (status_bits_ & OM_LI_UNSTABILITY_LONG) {
        if (count++) reprss << '+';
        reprss << "li_unstability_long";
      }
      if (status_bits_ & OM_BI_LI_UNCONSISTENCY_SHORT) {
        if (count++) reprss << '+';
        reprss << "bi_li_unconsistency_short";
      }
      if (status_bits_ & OM_BI_LI_UNCONSISTENCY_LONG) {
        if (count++) reprss << '+';
        reprss << "bi_li_unconsistency_long";
      }
      if (status_bits_ & OM_OTHER_ISSUES) {
	if (count++) reprss << '+';
        reprss << "other_issues";
      }
      // if (status_bits_ & OM_FOO) {
      //   if (count++) reprss << '+';
      //   reprss << "foo";
      // }
      if (count == 0u) {
        reprss << "good";
      }
      return reprss.str();
    }
                        
    // static
    std::uint32_t calorimeter_om_status::status_from_string(const std::string & status_repr_,
							    const std::uint16_t options_)
    {
      std::uint32_t status = OM_GOOD;
      typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
      boost::char_separator<char> sep{"+"};
      tokenizer tokens{status_repr_, sep};
      bool onlyOneStatus = false;
      if (options_ & ONLY_ONE_BIT) onlyOneStatus = true;
      auto bitCount = 0u;
      for (std::string tk : tokens) {
        if (options_ & DECODE_TRIM) boost::trim(tk);
	
        if (tk == "good") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          return status;

	} else if (tk == "other_issues") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_OTHER_ISSUES;
	  bitCount++;
	} else if (tk == "bi_li_unconsistency_long") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_LI_UNCONSISTENCY_LONG;
	  bitCount++;
	} else if (tk == "bi_li_unconsistency_short") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_LI_UNCONSISTENCY_SHORT;
	  bitCount++;
	} else if (tk == "li_unstability_long") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_LI_UNSTABILITY_LONG;
	  bitCount++;
	} else if (tk == "bi_screening_se_strip") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_SCREENING_SE_STRIPS;
	  bitCount++;
	} else if (tk == "bi_jump") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_JUMP;
	  bitCount++;
	} else if (tk == "bi_unstability_long") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_UNSTABILITY_LONG;
	  bitCount++;
	} else if (tk == "no_li_monitoring") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_NO_LI_MONITORING;
	  bitCount++;
	} else if (tk == "no_bi_monitoring") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_NO_BI_MONITORING;
	  bitCount++;
	} else if (tk == "noisy") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_NOISY;
	  bitCount++;
	} else if (tk == "off") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_OFF;
	  bitCount++;
	} else if (tk == "dead") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_DEAD;
	  bitCount++;
        } else /* if (tk == "foo") status |= OM_FOO;     
		  else if (tk == "bar") status |= OM_BAR;   
		  else */ { 
          DT_THROW(std::logic_error, "Invalid calorimeter OM status label '" << tk << "'!");
        }
      }
      if (onlyOneStatus and bitCount > 1) {
	DT_THROW(std::logic_error, "More than one status bit is set in '" << status_repr_ << "'!");
      }
      return status;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const calorimeter_om_status_record & record_)
    {
      std::ostringstream sout;
      sout << '@' << snemo::time::to_string(record_.period)
	   << " -> status=" << calorimeter_om_status::status_to_string(record_.status);
      out_ << sout.str();
      return out_;
    }
   
    void calorimeter_om_status_history::print(std::ostream & out_, const std::string & indent_) const
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
 
    void calorimeter_om_status_history::add(const time::time_period & period_, const std::uint32_t status_)
    {
      if (_records_.size() and period_.begin() < _records_.back().period.end()) {
        DT_THROW(std::domain_error, "New period does not follow last record!");
      }
      _records_.push_back({period_, status_});
    }

    void calorimeter_om_status_history::clear()
    {
      _records_.clear();
      return;
    }

    std::uint32_t calorimeter_om_status_history::get_status(const time::time_point & t_) const
    {
      std::uint32_t status = calorimeter_om_status::OM_GOOD;
      for (const auto & rec : _records_ ) {
        if (rec.period.contains(t_)) {
          status = rec.status;
          break;
        }
      }
      return status;
    }
    
    const std::vector<calorimeter_om_status_record> &
    calorimeter_om_status_history::records() const
    {
      return _records_;
    }
 
    calorimeter_om_status_change_event::calorimeter_om_status_change_event(const time::time_point & timestamp_,
									   const event_type event_type_,
									   calorimeter_om_status::status_bit bit_)
      : _timestamp_(timestamp_)
      , _event_type_(event_type_)
    {
      if ((_event_type_ != no_change) and (_event_type_ != reset_bits)) {
	_bit_ = bit_;
      }
      return;
    }
    
    calorimeter_om_status::status_bit calorimeter_om_status_change_event::bit() const
    {
      return _bit_;
    }
			
    const time::time_point & calorimeter_om_status_change_event::timestamp() const
    {
      return _timestamp_;
    }

    bool calorimeter_om_status_change_event::is_no_change_event() const
    {
      return _event_type_ == no_change;
    }

    bool calorimeter_om_status_change_event::is_reset_bits_event() const
    {
      return _event_type_ == reset_bits;
    }

    bool calorimeter_om_status_change_event::is_set_bit_event() const
    {
      return _event_type_ == set_bit;
    }

    bool calorimeter_om_status_change_event::is_unset_bit_event() const
    {
      return _event_type_ == unset_bit;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const calorimeter_om_status_change_event & event_)
    {
      out_ << "@" << snemo::time::to_string(event_._timestamp_);
      if (event_.is_no_change_event()) {
	out_ << " -> no_change";
      } else if (event_.is_reset_bits_event()) {
	out_ << " -> reset_bits";
      } else if (event_.is_set_bit_event()) {
	out_ << " -> set_bit[" << calorimeter_om_status::status_to_string(event_.bit()) << "]";
      } else if (event_.is_unset_bit_event()) {
	out_ << " -> unset_bit[" << calorimeter_om_status::status_to_string(event_.bit()) << "]";
      }
      return out_;
    }
			
   
    // static
    calorimeter_om_status_change_event calorimeter_om_status_change_event::make_reset(const time::time_point & timestamp_)
    {
      return {timestamp_, reset_bits, calorimeter_om_status::OM_DEAD};
    }
    
    // static
    calorimeter_om_status_change_event calorimeter_om_status_change_event::make_no_change(const time::time_point & timestamp_)
    {
      return {timestamp_, no_change, calorimeter_om_status::OM_DEAD};
    }

    // static
    calorimeter_om_status_change_event
    calorimeter_om_status_change_event::make_set_bit(const time::time_point & timestamp_, 
						     const calorimeter_om_status::status_bit bit_)
    {
      return {timestamp_, set_bit, bit_};
    }
    
    // static
    calorimeter_om_status_change_event
    calorimeter_om_status_change_event::make_unset_bit(const time::time_point & timestamp_, 
						       const calorimeter_om_status::status_bit bit_)
    {
      return {timestamp_, unset_bit, bit_};
    }
 
    std::size_t calorimeter_om_status_change_event_list::size() const
    {
      return _events_.size();
    }
 
    const calorimeter_om_status_change_event &
    calorimeter_om_status_change_event_list::event(const int i_) const
    {
      DT_THROW_IF(i_< 0 or i_ >= (int) _events_.size(), std::logic_error, "invalid event index");
      return _events_[i_];
    }

    void calorimeter_om_status_change_event_list::add_event(const calorimeter_om_status_change_event & event_)
    {
      if (_events_.size() and event_.timestamp() < _events_.back().timestamp()) {
        DT_THROW(std::domain_error, "the timestamp of the new event is earlier than that of the last event");
      }
      _events_.push_back(event_);
      return;
    }

    void build_calorimeter_om_status_history_from_event_list(const calorimeter_om_status_change_event_list & event_list_,
							     calorimeter_om_status_history & status_history_)
    {
      status_history_.clear();
      std::uint32_t currentStatus = calorimeter_om_status::OM_GOOD;
      snemo::time::time_point currentPeriodStart;
      
      for (auto iEvent = 0u; iEvent < event_list_.size(); iEvent++) {
	const auto & event = event_list_.event(iEvent);
	time::time_point currentEventTimepoint = event.timestamp();
	bool noChange = false;
	if (event.is_no_change_event()) {
	  noChange = true;
	} else if (event.is_set_bit_event()) {
	  std::uint32_t bitMask = 0;
	  bitMask |= event.bit();
	  if (currentStatus & bitMask) {
	    noChange = true;
	  }
	} else if (event.is_unset_bit_event()) {
	  std::uint32_t bitMask = 0;
	  bitMask |= event.bit();
	  if (not (currentStatus & bitMask)) {
	    noChange = true;
	  }
	} else if (event.is_reset_bits_event()) {
	  if (currentStatus == calorimeter_om_status::OM_GOOD) {
	    noChange = true;	    
	  }
	}
	if (noChange) {
	  continue;
	}
	if (currentStatus != calorimeter_om_status::OM_GOOD) {
	  // Close the current period and store the record in the history:
	  status_history_.add(snemo::time::time_period(currentPeriodStart,
						       currentEventTimepoint),
			      currentStatus);
	  // A new period starts:
	  snemo::time::invalidate(currentPeriodStart);
	}
	std::uint32_t newStatus = currentStatus;
	if (event.is_reset_bits_event()) {
	  newStatus = calorimeter_om_status::OM_GOOD;
	} else if (event.is_set_bit_event()) {
	  newStatus |= event.bit();
	} else if (event.is_unset_bit_event()) {
	  newStatus &= ~(event.bit());
	}
	currentStatus = newStatus;
	// if (not snemo::time::is_valid(currentPeriodStart)) {
	//   currentPeriodStart = currentEventTimepoint;
	// }
	if (currentStatus == calorimeter_om_status::OM_GOOD) {
	  snemo::time::invalidate(currentPeriodStart);
	} else {
	  currentPeriodStart = currentEventTimepoint;
	}
      }
      if (snemo::time::is_valid(currentPeriodStart)
	  and currentStatus != calorimeter_om_status::OM_GOOD) {
	status_history_.add(snemo::time::time_period(currentPeriodStart,
						     snemo::time::time_point_pos_infinity()),
			    currentStatus);
      }
     
      return;
    }

  } // end of namespace rc

} // end of namespace snemo
