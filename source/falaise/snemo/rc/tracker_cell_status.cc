/// \file falaise/snemo/rc/tracker_cell_status.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2022-05-23
/// Last modified: 2022-05-23
///

// Ourselves:
#include <falaise/snemo/rc/tracker_cell_status.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>

namespace snemo {

  namespace rc {

    // static
    bool tracker_cell_status::is_off(std::uint32_t status_bits_)
    {
      return status_bits_ & CELL_OFF;
    }
 
    // static
    bool tracker_cell_status::is_dead(std::uint32_t status_bits_)
    {
      return status_bits_ & CELL_DEAD;
    }
 
    // static
    bool tracker_cell_status::is_no_anode(std::uint32_t status_bits_)
    {
      return status_bits_ & CELL_NO_ANODE;
    }
 
    // static
    bool tracker_cell_status::is_no_bottom_cathode(std::uint32_t status_bits_)
    {
      return status_bits_ & CELL_NO_BOTTOM_CATHODE;
    }
 
    // static
    bool tracker_cell_status::is_no_top_cathode(std::uint32_t status_bits_)
    {
      return status_bits_ & CELL_NO_TOP_CATHODE;
    }

    // static
    std::string tracker_cell_status::status_to_string(std::uint32_t status_bits_)
    {
      std::ostringstream reprss;
      int count=0;
      if (status_bits_ & CELL_DEAD) {
        if (count++) reprss << '+';
        reprss << "dead";
      }
      if (status_bits_ & CELL_OFF) {
        if (count++) reprss << '+';
        reprss << "off";
      }
      if (status_bits_ & CELL_NO_ANODE) {
        if (count++) reprss << '+';
        reprss << "no_anode";
      }
      if (status_bits_ & CELL_NO_BOTTOM_CATHODE) {
        if (count++) reprss << '+';
        reprss << "no_bottom_cathode";
      }
      if (status_bits_ & CELL_NO_TOP_CATHODE) {
        if (count++) reprss << '+';
        reprss << "no_top_cathode";
      }
      if (count == 0) {
        reprss << "good";
      }
      return reprss.str();
    }
                        
    // static
    std::uint32_t tracker_cell_status::status_from_string(std::string status_repr_, const std::uint16_t options_)
    {
      std::uint32_t status = CELL_GOOD;
      typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
      boost::char_separator<char> sep{"+"};
      tokenizer tokens{status_repr_, sep};
      for (std::string tk : tokens) {
        if (options_ & DECODE_TRIM) boost::trim(tk);
        if (tk == "good") {
          DT_THROW_IF(status != CELL_GOOD, std::logic_error, "Found incompatible tracker cell status labels in '" << status_repr_ << "'!");
          return status;
        } else if (tk == "dead") {
          DT_THROW_IF(status != CELL_GOOD, std::logic_error, "Found incompatible tracker cell status labels in '" << status_repr_ << "'!");
          status |= CELL_DEAD;
        } else if (tk == "off") {
          DT_THROW_IF(status != CELL_GOOD, std::logic_error, "Found incompatible tracker cell status labels in '" << status_repr_ << "'!");
          status |= CELL_OFF;
        } else if (tk == "no_anode") status |= CELL_NO_ANODE;     
        else if (tk == "no_bottom_cathode") status |= CELL_NO_BOTTOM_CATHODE;   
        else if (tk == "no_top_cathode") status |= CELL_NO_TOP_CATHODE;
        else {
          DT_THROW(std::logic_error, "Invalid tracker cell status label '" << tk << "'!");
        }
      }      
      return status;
    }
 
    void tracker_cell_status_history::add(const time::time_period & period_, std::uint32_t status_)
    {
      if (_records_.size() and period_.begin() < _records_.back().period.end()) {
        DT_THROW(std::domain_error, "New period does not follow last record!");
      }
      _records_.push_back({period_, status_});
    }

    void tracker_cell_status_history::clear()
    {
      _records_.clear();
      return;
    }

    std::uint32_t tracker_cell_status_history::get_status(const time::time_point & t_) const
    {
      std::uint32_t status = tracker_cell_status::CELL_GOOD;
      for (const auto & rec : _records_ ) {
        if (rec.period.contains(t_)) {
          status = rec.status;
          break;
        }
      }
      return status;
    }
    
    const std::vector<tracker_cell_status_record> &
    tracker_cell_status_history::records() const
    {
      return _records_;
    }
     
  }  // end of namespace rc

}  // end of namespace snemo
