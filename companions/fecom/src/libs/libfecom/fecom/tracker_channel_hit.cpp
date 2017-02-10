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
    if (channel >= tracker_constants::INVALID_CHANNEL_INDEX
	|| feast_id == tracker_constants::INVALID_FEAST_INDEX
	|| channel_type == INVALID_CHANNEL
	|| timestamp_type == "INVALID") return false;
    return true;
  }

  void tracker_channel_hit::reset()
  {
    this->base_hit::reset();
    _reset_();

    return;
  }

  void tracker_channel_hit::_reset_()
  {
    feast_id = tracker_constants::INVALID_FEAST_INDEX;
    channel = tracker_constants::INVALID_CHANNEL_INDEX;
    event_id = 0xFFFF;
    channel_type = INVALID_CHANNEL;
    timestamp_type = "INVALID";
    reset_timestamp();
    associated = false;
    return;
  }

  void tracker_channel_hit::reset_timestamp()
  {
    timestamp_value = tracker_constants::INVALID_TIME;
    return;
  }

  void tracker_channel_hit::tree_dump(std::ostream & out_,
			      const std::string & title_,
			      const std::string & indent_,
			      bool inherit_) const
  {
    this->base_hit::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << io::tag()
         << "Feast                     : " << (int) feast_id << std::endl;

    out_ << indent_ << io::tag()
         << "Channel                   : " << (int) channel << std::endl;

    out_ << indent_ << io::tag()
         << "Event ID                  : " << (int) event_id << std::endl;

    out_ << indent_ << io::tag()
         << "Channel type              : " << (int) channel_type << std::endl;

    out_ << indent_ << io::tag()
         << "Timestamp type            : " << (std::string) timestamp_type << std::endl;

    out_ << indent_ << io::tag()
         << "Timestamp value           : " << (int) timestamp_value << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity                  : " << std::boolalpha << is_valid() << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Already associated        : " << std::boolalpha << associated << std::endl;

    return;
  }


} // namespace fecom
