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

const CalorimeterHitHdlCollection& calibrated_data::calorimeter_hits() const {
  return calorimeter_hits_;
}

CalorimeterHitHdlCollection& calibrated_data::calorimeter_hits() { return calorimeter_hits_; }

const TrackerHitHdlCollection& calibrated_data::tracker_hits() const { return tracker_hits_; }

TrackerHitHdlCollection& calibrated_data::tracker_hits() { return tracker_hits_; }

void calibrated_data::clear() {
  calorimeter_hits_.clear();
  tracker_hits_.clear();
  _properties_.clear();
}

void calibrated_data::tree_dump(std::ostream& out_, const std::string& title_,
                                const std::string& indent_, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  // Calibrated calorimeter hits:
  out_ << indent_ << datatools::i_tree_dumpable::tag;
  out_ << "Calibrated calorimeter hits: " << calorimeter_hits_.size() << std::endl;
  for (size_t i = 0; i < calorimeter_hits_.size(); i++) {
    out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
    if (i + 1 == calorimeter_hits_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    const calibrated_calorimeter_hit& calo_calib_hit = calorimeter_hits_.at(i).get();
    out_ << "Hit #" << i << " : Id=" << calo_calib_hit.get_hit_id()
         << " GID=" << calo_calib_hit.get_geom_id()
         << " E=" << calo_calib_hit.get_energy() / CLHEP::keV << " keV"
         << " t=" << calo_calib_hit.get_time() / CLHEP::ns << " ns" << std::endl;
  }

  // Calibrated tracker hits:
  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
  out_ << "Calibrated tracker hits: " << tracker_hits_.size() << std::endl;
  for (size_t i = 0; i < tracker_hits_.size(); i++) {
    out_ << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    if (i + 1 == tracker_hits_.size()) {
      out_ << datatools::i_tree_dumpable::last_tag;
    } else {
      out_ << datatools::i_tree_dumpable::tag;
    }
    const calibrated_tracker_hit& tracker_calib_hit = tracker_hits_.at(i).get();
    out_ << "Hit #" << i << " : Id=" << tracker_calib_hit.get_hit_id()
         << " GID=" << tracker_calib_hit.get_geom_id() << ' ';
    if (tracker_calib_hit.is_prompt()) {
      out_ << "[prompt]";
    }
    if (tracker_calib_hit.is_delayed()) {
      out_ << "[delayed]";
    }
    out_ << std::endl;
  }
}

}  // end of namespace datamodel

}  // end of namespace snemo
