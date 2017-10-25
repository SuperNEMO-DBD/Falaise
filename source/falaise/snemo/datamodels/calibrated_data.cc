// falaise/snemo/datamodels/calibrated_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_data,
                                                  "snemo::datamodel::calibrated_data")

bool calibrated_data::has_data() const {
  return has_calibrated_calorimeter_hits() || has_calibrated_tracker_hits();
}

bool calibrated_data::has_calibrated_calorimeter_hits() const {
  return _calibrated_calorimeter_hits_.size() != 0;
}

const calibrated_data::calorimeter_hit_collection_type&
calibrated_data::calibrated_calorimeter_hits() const {
  return _calibrated_calorimeter_hits_;
}

calibrated_data::calorimeter_hit_collection_type& calibrated_data::calibrated_calorimeter_hits() {
  return _calibrated_calorimeter_hits_;
}

bool calibrated_data::has_calibrated_tracker_hits() const {
  return _calibrated_tracker_hits_.size() != 0;
}

const calibrated_data::tracker_hit_collection_type& calibrated_data::calibrated_tracker_hits()
    const {
  return _calibrated_tracker_hits_;
}

calibrated_data::tracker_hit_collection_type& calibrated_data::calibrated_tracker_hits() {
  return _calibrated_tracker_hits_;
}

const datatools::properties& calibrated_data::get_properties() const { return _properties_; }

datatools::properties& calibrated_data::grab_properties() { return _properties_; }

calibrated_data::calibrated_data() { return; }

calibrated_data::~calibrated_data() {
  clear();
  return;
}

void calibrated_data::reset_calibrated_calorimeter_hits() {
  _calibrated_calorimeter_hits_.clear();
  return;
}

void calibrated_data::reset_calibrated_tracker_hits() {
  _calibrated_tracker_hits_.clear();
  return;
}

void calibrated_data::reset() {
  reset_calibrated_calorimeter_hits();
  reset_calibrated_tracker_hits();
  _properties_.clear();
  return;
}

void calibrated_data::clear() {
  reset();
  return;
}

void calibrated_data::tree_dump(std::ostream& out_, const std::string& title_,
                                const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  // Properties:
  {
    out_ << indent << datatools::i_tree_dumpable::tag << "Properties : ";
    if (_properties_.size() == 0) {
      out_ << "<empty>";
    }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::skip_tag;
      _properties_.tree_dump(out_, "", indent_oss.str());
    }
  }

  // Calibrated calorimeter hits:
  out_ << indent << datatools::i_tree_dumpable::tag;
  out_ << "Calibrated calorimeter hits: " << _calibrated_calorimeter_hits_.size() << std::endl;
  for (size_t i = 0; i < _calibrated_calorimeter_hits_.size(); i++) {
    const calibrated_calorimeter_hit& calo_calib_hit = _calibrated_calorimeter_hits_.at(i).get();
    out_ << indent << datatools::i_tree_dumpable::skip_tag;
    if (i + 1 == _calibrated_calorimeter_hits_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Hit #" << i << " : Id=" << calo_calib_hit.get_hit_id()
         << " GID=" << calo_calib_hit.get_geom_id()
         << " E=" << calo_calib_hit.get_energy() / CLHEP::keV << " keV"
         << " t=" << calo_calib_hit.get_time() / CLHEP::ns << " ns" << std::endl;
  }

  // Calibrated tracker hits:
  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_);
  out_ << "Calibrated tracker hits: " << _calibrated_tracker_hits_.size() << std::endl;
  for (size_t i = 0; i < _calibrated_tracker_hits_.size(); i++) {
    const calibrated_tracker_hit& tracker_calib_hit = _calibrated_tracker_hits_.at(i).get();
    out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    if (i + 1 == _calibrated_tracker_hits_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    out_ << "Hit #" << i << " : Id=" << tracker_calib_hit.get_hit_id()
         << " GID=" << tracker_calib_hit.get_geom_id() << ' ';
    if (tracker_calib_hit.is_prompt()) out_ << "[prompt]";
    if (tracker_calib_hit.is_delayed()) out_ << "[delayed]";
    out_ << std::endl;
  }

  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo
