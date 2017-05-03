// fecom/tracker_channel_hit.cpp

// Ourselves:
#include <fecom/tracker_channel_hit.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_channel_hit, "fecom::tracker_channel_hit")

  tracker_channel_hit::tracker_channel_hit()
    : base_hit(SIG_TRACKER)
  {
    reset();
  }

  tracker_channel_hit::~tracker_channel_hit()
  {
  }

  bool tracker_channel_hit::is_valid() const
  {
    if (! this->base_hit::is_valid()) return false;
    if (!(electronic_id.get_type() == tracker_constants::ANODIC_CHANNEL_TYPE
	  || electronic_id.get_type() == tracker_constants::CATHODIC_CHANNEL_TYPE)
	|| electronic_id.get(tracker_constants::SLOT_INDEX) >= tracker_constants::INVALID_BOARD
	|| electronic_id.get(tracker_constants::FEAST_INDEX) >= tracker_constants::INVALID_FEAST
	|| electronic_id.get(tracker_constants::CHANNEL_INDEX) >=  tracker_constants::INVALID_CHANNEL
	|| timestamp_type == "INVALID") return false;
    return true;
  }

  double tracker_channel_hit::get_timestamp() const
  {
    return timestamp_time_ns;
  }

  void tracker_channel_hit::reset()
  {
    this->base_hit::reset();
    _reset_();

    return;
  }

  void tracker_channel_hit::_reset_()
  {
    timestamp_type = "INVALID";
    timestamp_time_ns = 0;
    reset_timestamp();
    associated = false;
    return;
  }

  void tracker_channel_hit::reset_timestamp()
  {
    timestamp_value = tracker_constants::INVALID_TIME;
    timestamp_time_ns = 0;
    return;
  }

  void tracker_channel_hit::tree_dump(std::ostream & out_,
			      const std::string & title_,
			      const std::string & indent_,
			      bool inherit_) const
  {
    this->base_hit::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << io::tag()
         << "Timestamp type            : " << (std::string) timestamp_type << std::endl;

    out_ << indent_ << io::tag()
         << "Timestamp value           : " << (uint64_t) timestamp_value << std::endl;

    out_ << indent_ << io::tag()
         << "Timestamp time (in ns)    : " << timestamp_time_ns << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Already associated        : " << std::boolalpha << associated << std::endl;

    return;
  }


} // namespace fecom
