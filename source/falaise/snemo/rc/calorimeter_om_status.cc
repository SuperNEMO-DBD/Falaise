/// \file falaise/snemo/rc/calorimeter_om_status.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-29
/// Last modified: 2022-05-29
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
    bool calorimeter_om_status::is_off(std::uint32_t status_bits_)
    {
      return status_bits_ & OM_OFF;
    }
 
    // static
    bool calorimeter_om_status::is_dead(std::uint32_t status_bits_)
    {
      return status_bits_ & OM_DEAD;
    }
 
    // static
    std::string calorimeter_om_status::status_to_string(std::uint32_t status_bits_)
    {
      std::ostringstream reprss;
      int count=0;
      if (status_bits_ & OM_DEAD) {
        if (count++) reprss << '+';
        reprss << "dead";
      }
      if (status_bits_ & OM_OFF) {
        if (count++) reprss << '+';
        reprss << "off";
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
    std::uint32_t calorimeter_om_status::status_from_string(std::string status_repr_, const std::uint16_t options_)
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
        } else if (tk == "dead") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_DEAD;
        } else if (tk == "off") {
          DT_THROW_IF(status != OM_GOOD, std::logic_error, "Found incompatible calorimeter OM status labels in '" << status_repr_ << "'!");
          status |= OM_OFF;
        } else /* if (tk == "foo") status |= OM_FOO;     
        else if (tk == "bar") status |= OM_BAR;   
        else */ { 
          DT_THROW(std::logic_error, "Invalid calorimeter OM status label '" << tk << "'!");
        }
      }      
      return status;
    }
 
    void calorimeter_om_status_history::add(const time::time_period & period_, std::uint32_t status_)
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
     
  }  // end of namespace rc

}  // end of namespace snemo
