// fecom/commissioning_event.cpp

// Ourselves:
#include <fecom/commissioning_event.hpp>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(commissioning_event, "fecom::commissioning_event")

  commissioning_event::commissioning_event()
  {
    reset();
  }

  commissioning_event::~commissioning_event()
  {
  }

  bool commissioning_event::is_valid() const
  {
    return _trigger_id_ != 0xFFFFFFFF;
  }

  void commissioning_event::add_calo_hit(fecom::calo_hit & a_calo_hit_)
  {
    _calo_hit_collection_.insert(a_calo_hit_);
    return;
  }

  void commissioning_event::add_tracker_channel_hit(fecom::tracker_channel_hit & a_tracker_channel_hit_)
  {
    _tracker_channel_hit_collection_.insert(a_tracker_channel_hit_);
    return;
  }

  uint32_t commissioning_event::get_trigger_id() const
  {
    return _trigger_id_;
  }

  void commissioning_event::set_trigger_id(const uint32_t trigger_id_)
  {
    _trigger_id_ = trigger_id_;
    return;
  }

  const commissioning_event::calo_hit_collection & commissioning_event::get_calo_hit_collection() const
  {
    return _calo_hit_collection_;
  }

  const commissioning_event::tracker_channel_hit_collection & commissioning_event::get_tracker_channel_hit_collection() const
  {
    return _tracker_channel_hit_collection_;
  }

  void commissioning_event::reset()
  {
    _reset_();
    return;
  }

  void commissioning_event::_reset_()
  {
    _trigger_id_ = 0xFFFFFFFF;
    _calo_hit_collection_.clear();
    _tracker_channel_hit_collection_.clear();
    _tracker_hit_collection_.clear();
    return;
  }
  void commissioning_event::tree_dump(std::ostream & out_,
				      const std::string & title_,
				      const std::string & indent_,
				      bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
         << "Trigger ID : " << _trigger_id_ << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Calo hit collection size : " << _calo_hit_collection_.size() << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Tracker channel hit collection size : " << _tracker_channel_hit_collection_.size() << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Tracker hit collection size : " << _tracker_hit_collection_.size() << std::endl;

    return;
  }

  bool commissioning_event::compare::operator()(const commissioning_event & a,
						const commissioning_event & b)
  {
    if (a._trigger_id_ < b._trigger_id_) {
      return true;
    }
    else if ((a._trigger_id_ == b._trigger_id_)){
      DT_THROW(std::logic_error, "Two commissioning events have the same trigger_id, trigger_id_a = '" << a._trigger_id_ << "', trigger_id_b = '" << b._trigger_id_ << "' !");
      return true;
    }

    else {
      return false;
    }
  }

} // namespace fecom
