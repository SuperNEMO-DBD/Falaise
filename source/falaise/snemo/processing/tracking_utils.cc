/// \file falaise/snemo/processing/tracking_utils.cc

// Ourselves:
#include <falaise/snemo/processing/tracking_utils.h>

namespace snemo {

  namespace processing {

    
    tracking_precluster::tracking_precluster(const int side_,
					     const bool delayed_)
    {
      _side_ = side_;
      _delayed_ = delayed_;
    }

    tracking_precluster::tracking_precluster(const int side_,
					     const bool delayed_,
					     const snemo::datamodel::TrackerHitHdlCollection & hits_)
    {
      _side_ = side_;
      _delayed_ = delayed_;
      _hits_ = hits_;
    }

    int tracking_precluster::side() const
    {
      return _side_;
    }

    bool tracking_precluster::is_delayed() const
    {
      return _delayed_;
    }

    const snemo::datamodel::TrackerHitHdlCollection &
    tracking_precluster::hits() const
    {
      return _hits_;
    }

    snemo::datamodel::TrackerHitHdlCollection &
    tracking_precluster::hits()
    {
      return _hits_;
    }

    void tracking_precluster::print(std::ostream & out_, 
				    const std::string & indent_,
				    const uint32_t /*flags_*/) const
    {
      // out_ << indent_ << "Input precluster:\n";
      out_ << indent_ << "|-- " << "Side = " << _side_ << '\n';
      out_ << indent_ << "|-- " << "Delayed = " << std::boolalpha << _delayed_ << '\n';
      out_ << indent_ << "`-- " << "Hits = " << _hits_.size() << '\n';
      for (auto i = 0u; i < _hits_.size(); i++) {
	out_ << indent_ << "    ";
	if (i + 1 == _hits_.size()) out_ << "`-- ";
	else out_ << "|-- ";
	out_ << "Hit #" << i << " : " << _hits_[i]->get_hit_id() << '\n';
      }
      return;
    }

  } // end of namespace processing

} // end of namespace snemo
