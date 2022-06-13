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

void calibrated_data::tree_dump(std::ostream& out, const std::string& title,
                                const std::string& indent, bool is_last) const {
  auto printCaloHit = [&](const CalorimeterHitHdl& x) {
    out << "(Id : " << x->get_hit_id() << ", GID : " << x->get_geom_id()
        << ", Energy : " << x->get_energy() / CLHEP::keV << " keV"
        << ", Time : " << x->get_time() / CLHEP::ns << " ns)" << std::endl;
  };

  auto printTrackerHit = [&](const TrackerHitHdl& x) {
    out << "(Id : " << x->get_hit_id() << ", GID : " << x->get_geom_id()
        << ", Type : " << (x->is_prompt() ? "prompt" : "delayed [time=" + std::to_string(x->get_delayed_time() / CLHEP::microsecond) + " us]" ) << ")" << std::endl;
  };

  if (!title.empty()) {
    out << indent << title << std::endl;
  }

  // Calibrated calorimeter hits:
  out << indent << datatools::i_tree_dumpable::tag << "CalorimeterHits[" << calorimeter_hits_.size()
      << "]:" << std::endl;
  if (!calorimeter_hits_.empty()) {
    std::for_each(
        calorimeter_hits_.begin(), --calorimeter_hits_.end(), [&](const CalorimeterHitHdl& x) {
          out << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::tag;
          printCaloHit(x);
        });
    out << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::last_tag;
    printCaloHit(calorimeter_hits_.back());
  }

  // Calibrated tracker hits:
  out << indent << datatools::i_tree_dumpable::inherit_tag(is_last) << "TrackerHits["
      << tracker_hits_.size() << "]:" << std::endl;
  if (!tracker_hits_.empty()) {
    std::for_each(tracker_hits_.begin(), --tracker_hits_.end(), [&](const TrackerHitHdl& x) {
      out << indent << datatools::i_tree_dumpable::inherit_skip_tag(is_last)
          << datatools::i_tree_dumpable::tag;
      printTrackerHit(x);
    });
    out << indent << datatools::i_tree_dumpable::inherit_skip_tag(is_last)
        << datatools::i_tree_dumpable::last_tag;
    printTrackerHit(tracker_hits_.back());
  }
}

}  // end of namespace datamodel

}  // end of namespace snemo
