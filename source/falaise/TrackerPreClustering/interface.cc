// interface.cc

// Ourselves:
#include "falaise/TrackerPreClustering/interface.h"

// Standard library:
#include <limits>
#include <sstream>

namespace TrackerPreClustering {

  const std::string & setup_data::get_last_error_message() const
  {
    return _last_error_message;
  }

  void setup_data::set_last_error_message(const std::string & message_)
  {
    _last_error_message = message_;
    return;
  }

  setup_data::setup_data()
  {
    reset();
    return;
  }

  void setup_data::reset()
  {
    logging = datatools::logger::PRIO_WARNING;
    cell_size = std::numeric_limits<double>::quiet_NaN();
    delayed_hit_cluster_time = std::numeric_limits<double>::quiet_NaN();
    processing_prompt_hits   = true;
    processing_delayed_hits  = true;
    split_chamber            = false;
    _last_error_message.clear();
    return;
  }

  bool setup_data::check() const
  {
    setup_data * mutable_this = const_cast<setup_data*>(this);
    if (cell_size != cell_size)  {
      std::ostringstream message;
      message << "TrackerPreClustering::setup_data::check: "
              << "Undefined cell size !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
    if (cell_size <= 0.0)  {
      std::ostringstream message;
      message << "TrackerPreClustering::setup_data::check: "
              << "Negative cell size makes no sense !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
    return true;
  }

} // end of namespace TrackerPreClustering
