// fecom/commissioning_event_data.cpp

// Ourselves:
#include <fecom/commissioning_event_data.hpp>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(commissioning_event_data, "fecom::commissioning_event_data")

  commissioning_event_data::commissioning_event_data()
  {
    reset();
  }

  commissioning_event_data::~commissioning_event_data()
  {
  }

  bool commissioning_event_data::is_valid() const
  {
    if (_commissioning_event_collection_.size() != 0) return true;
    else return false;
  }

  void commissioning_event_data::add_commissioning_event(commissioning_event & a_commissioning_event_)
  {
    _commissioning_event_collection_.push_back(a_commissioning_event_);
    return;
  }

  const commissioning_event_data::commissioning_event_collection & commissioning_event_data::get_commissioning_event_collection() const
  {
    return _commissioning_event_collection_;
  }

  commissioning_event_data::commissioning_event_collection & commissioning_event_data::grab_commissioning_event_collection()
  {
    return _commissioning_event_collection_;
  }

  void commissioning_event_data::reset()
  {
    _reset_();
    return;
  }

  void commissioning_event_data::_reset_()
  {
    _commissioning_event_collection_.clear();
    return;
  }
  void commissioning_event_data::tree_dump(std::ostream & out_,
					   const std::string & title_,
					   const std::string & indent_,
					   bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Commissioning event collection size : " << _commissioning_event_collection_.size() << std::endl;

    return;
  }

} // namespace fecom
