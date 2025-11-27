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
        reprss << "no_bi_monitoring";
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
      if (count == 0) {
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
      for (std::string tk : tokens) {
        if (options_ & DECODE_TRIM) boost::trim(tk);
	
        if (tk == "good") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          return status;

	} else if (tk == "other_issues") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_OTHER_ISSUES;

	} else if (tk == "bi_li_unconsistency_long") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_LI_UNCONSISTENCY_LONG;

	} else if (tk == "bi_li_unconsistency_short") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_LI_UNCONSISTENCY_SHORT;

	} else if (tk == "li_unstability_long") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_LI_UNSTABILITY_LONG;

	} else if (tk == "bi_screening_se_strip") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_SCREENING_SE_STRIPS;

	} else if (tk == "bi_jump") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_JUMP;

	} else if (tk == "bi_unstability_long") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_BI_UNSTABILITY_LONG;

	} else if (tk == "no_li_monitoring") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_NO_LI_MONITORING;

	} else if (tk == "no_bi_monitoring") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_NO_BI_MONITORING;

	} else if (tk == "noisy") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_NOISY;

	} else if (tk == "off") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_OFF;

	} else if (tk == "dead") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_DEAD;
        } else /* if (tk == "foo") status |= OM_FOO;     
		  else if (tk == "bar") status |= OM_BAR;   
		  else */ { 
          DT_THROW(std::logic_error, "Invalid calorimeter OM status label '" << tk << "'!");
        }
      }      
      return status;
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
     
  } // end of namespace rc

} // end of namespace snemo
