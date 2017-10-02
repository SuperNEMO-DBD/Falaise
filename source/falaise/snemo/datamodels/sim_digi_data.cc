/// \file falaise/snemo/datamodels/sim_digi_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_digi_data.h>

namespace snemo {

namespace datamodel {

// Serial tag :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_digi_data, "snemo::datamodel::sim_digi_data")

sim_digi_data::sim_digi_data() {
  _digi_events_.clear();
  return;
}

sim_digi_data::~sim_digi_data() { return; }

bool sim_digi_data::is_valid() const { return true; }

void sim_digi_data::reset() {
  _digi_events_.clear();
  _auxiliaries_.clear();
  return;
}

const sim_digi_data::sim_digi_event_collection_type& sim_digi_data::get_digi_events() const {
  return _digi_events_;
}

sim_digi_data::sim_digi_event_collection_type& sim_digi_data::grab_digi_events() {
  return _digi_events_;
}

const datatools::properties& sim_digi_data::get_auxiliaries() const { return _auxiliaries_; }

datatools::properties& sim_digi_data::grab_auxiliaries() { return _auxiliaries_; }

void sim_digi_data::tree_dump(std::ostream& out_, const std::string& title_,
                              const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  // Auxiliary properties:
  {
    out_ << indent_ << datatools::i_tree_dumpable::tag << "Auxiliary properties : ";
    if (_auxiliaries_.size() == 0) {
      out_ << "<empty>";
    }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent_;
      indent_oss << datatools::i_tree_dumpable::skip_tag;
      _auxiliaries_.tree_dump(out_, "", indent_oss.str());
    }
  }

  // Digitized events:
  {
    out_ << indent_ << datatools::i_tree_dumpable::tag << "Digitized events : " << std::endl;
    out_ << _digi_events_.size();
    out_ << std::endl;
  }

  // Validity:
  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
  out_ << "Valid: " << is_valid() << std::endl;

  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo
