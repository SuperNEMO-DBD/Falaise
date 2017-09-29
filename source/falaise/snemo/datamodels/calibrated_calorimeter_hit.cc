// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodel/calibrated_calorimeter_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>

// Third party
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

namespace datamodel {

// serial tag for datatools::serialization::i_serializable interface :
// DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_calorimeter_hit,
//                                                        "snemo::core::model::calibrated_calorimeter_hit")
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_calorimeter_hit,
                                                  "snemo::datamodel::calibrated_calorimeter_hit")

double calibrated_calorimeter_hit::get_time() const { return _time_; }

void calibrated_calorimeter_hit::set_time(double time_) {
  _time_ = time_;
  return;
}

double calibrated_calorimeter_hit::get_sigma_time() const { return _sigma_time_; }

void calibrated_calorimeter_hit::set_sigma_time(double sigma_time_) {
  _sigma_time_ = sigma_time_;
  return;
}

double calibrated_calorimeter_hit::get_energy() const { return _energy_; }

void calibrated_calorimeter_hit::set_energy(double energy_) {
  _energy_ = energy_;
  return;
}

double calibrated_calorimeter_hit::get_sigma_energy() const { return _sigma_energy_; }

void calibrated_calorimeter_hit::set_sigma_energy(double sigma_energy_) {
  _sigma_energy_ = sigma_energy_;
  return;
}

calibrated_calorimeter_hit::calibrated_calorimeter_hit() : base_hit() {
  datatools::invalidate(_energy_);
  datatools::invalidate(_sigma_energy_);
  datatools::invalidate(_time_);
  datatools::invalidate(_sigma_time_);
  return;
}

calibrated_calorimeter_hit::~calibrated_calorimeter_hit() { return; }

bool calibrated_calorimeter_hit::is_valid() const {
  return this->base_hit::is_valid() && std::isnormal(_energy_);
}

void calibrated_calorimeter_hit::invalidate() {
  this->base_hit::invalidate();
  datatools::invalidate(_energy_);
  datatools::invalidate(_sigma_energy_);
  datatools::invalidate(_time_);
  datatools::invalidate(_sigma_time_);
  return;
}

void calibrated_calorimeter_hit::tree_dump(std::ostream& out_, const std::string& title_,
                                           const std::string& indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) indent = indent_;
  base_hit::tree_dump(out_, title_, indent_, true);

  out_ << indent << datatools::i_tree_dumpable::tag << "Time  : " << _time_ / CLHEP::ns << " ns"
       << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag << "Sigma(time) : " << _sigma_time_ / CLHEP::ns
       << " ns" << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag << "Energy  : " << _energy_ / CLHEP::keV
       << " keV" << std::endl;

  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Sigma(energy) : " << _sigma_energy_ / CLHEP::keV << " keV" << std::endl;

  return;
}

void calibrated_calorimeter_hit::dump() const {
  this->tree_dump(std::clog, "snemo::datamodel::calibrated_calorimeter_hit");
  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo
