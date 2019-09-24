// falaise/snemo/datamodels/calibrated_tracker_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace {
const std::string& anode_time_key() {
  static const std::string key("anode_time");
  return key;
}

// const std::string& bottom_cathode_time_key() {
//  static const std::string key("bottom_cathode_time");
//  return key;
//}

// const std::string& top_cathode_time_key() {
//  static const std::string key("top_cathode_time");
//  return key;
//}
}  // namespace

namespace snemo {

namespace datamodel {

// Serial tag for datatools::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_tracker_hit,
                                                  "snemo::datamodel::calibrated_tracker_hit")

bool calibrated_tracker_hit::get_trait_bit(uint32_t trait) const { return (traits_ & trait) != 0u; }

void calibrated_tracker_hit::set_trait_bit(bool value, uint32_t trait) {
  if (value) {
    traits_ |= trait;
  } else {
    traits_ &= ~trait;
  }
}

bool calibrated_tracker_hit::is_delayed() const { return get_trait_bit(delayed); }

bool calibrated_tracker_hit::is_prompt() const { return !is_delayed(); }

bool calibrated_tracker_hit::is_peripheral() const { return get_trait_bit(peripheral); }

bool calibrated_tracker_hit::is_noisy() const { return get_trait_bit(noisy); }

bool calibrated_tracker_hit::is_sterile() const { return get_trait_bit(sterile); }

bool calibrated_tracker_hit::is_fake() const { return get_trait_bit(fake); }

void calibrated_tracker_hit::set_fake(bool is_fake) { set_trait_bit(is_fake, fake); }

void calibrated_tracker_hit::set_sterile(bool is_sterile) { set_trait_bit(is_sterile, sterile); }

void calibrated_tracker_hit::set_delayed(bool is_delayed) {
  set_trait_bit(is_delayed, delayed);
  if (!is_delayed) {
    datatools::invalidate(delayed_time_);
    datatools::invalidate(delayed_time_error_);
  }
}

void calibrated_tracker_hit::set_delayed_time(double time, double error) {
  if (datatools::is_valid(time)) {
    set_delayed(true);
    delayed_time_ = time;
    delayed_time_error_ = error;
  } else {
    set_delayed(false);
    datatools::invalidate(delayed_time_);
    datatools::invalidate(delayed_time_error_);
  }
}

bool calibrated_tracker_hit::has_delayed_time() const { return datatools::is_valid(delayed_time_); }

/// Return the delayed reference time of the hit
double calibrated_tracker_hit::get_delayed_time() const { return delayed_time_; }

bool calibrated_tracker_hit::has_delayed_time_error() const {
  return datatools::is_valid(delayed_time_error_);
}

/// Return the delayed reference time of the hit
double calibrated_tracker_hit::get_delayed_time_error() const { return delayed_time_error_; }

void calibrated_tracker_hit::set_bottom_cathode_missing(bool is_missing) {
  set_trait_bit(is_missing, missing_bottom_cathode);
}

void calibrated_tracker_hit::set_top_cathode_missing(bool is_missing) {
  set_trait_bit(is_missing, missing_top_cathode);
}

void calibrated_tracker_hit::set_noisy(bool is_noisy) { set_trait_bit(is_noisy, noisy); }

void calibrated_tracker_hit::set_peripheral(bool is_peripheral) {
  set_trait_bit(is_peripheral, peripheral);
}

bool calibrated_tracker_hit::has_anode_time() const {
  if (get_auxiliaries().has_key(anode_time_key())) {
    if (get_auxiliaries().is_real(anode_time_key())) {
      return true;
    }
  }
  return false;
}

void calibrated_tracker_hit::set_anode_time(double anode_time) {
  grab_auxiliaries().update(anode_time_key(), anode_time);
}

double calibrated_tracker_hit::get_anode_time() const {
  if (get_auxiliaries().has_key(anode_time_key())) {
    if (get_auxiliaries().is_real(anode_time_key())) {
      return get_auxiliaries().fetch_real(anode_time_key());
    }
  }
  return datatools::invalid_real();
}

bool calibrated_tracker_hit::has_xy() const { return get_trait_bit(xy); }

void calibrated_tracker_hit::set_xy(double x, double y) {
  set_trait_bit(true, xy);
  x_ = x;
  y_ = y;
}

double calibrated_tracker_hit::get_x() const { return x_; }

double calibrated_tracker_hit::get_y() const { return y_; }

double calibrated_tracker_hit::get_z() const { return z_; }

void calibrated_tracker_hit::set_z(double z) { z_ = z; }

double calibrated_tracker_hit::get_sigma_z() const { return sigma_z_; }

void calibrated_tracker_hit::set_sigma_z(double error) { sigma_z_ = error; }

double calibrated_tracker_hit::get_r() const { return r_; }

void calibrated_tracker_hit::set_r(double radius) { r_ = radius; }

double calibrated_tracker_hit::get_sigma_r() const { return sigma_r_; }

void calibrated_tracker_hit::set_sigma_r(double error) { sigma_r_ = error; }

void calibrated_tracker_hit::invalidate_xy() {
  datatools::invalidate(x_);
  datatools::invalidate(y_);
  set_trait_bit(true, xy);
}

void calibrated_tracker_hit::invalidate_positions() {
  datatools::invalidate(r_);
  datatools::invalidate(sigma_r_);
  datatools::invalidate(z_);
  datatools::invalidate(sigma_z_);
}

bool calibrated_tracker_hit::is_valid() const {
  return this->base_hit::is_valid() && std::isnormal(z_) && std::isnormal(r_);
}

void calibrated_tracker_hit::invalidate() {
  this->base_hit::invalidate();
  invalidate_positions();
  invalidate_xy();
  datatools::invalidate(delayed_time_);
  datatools::invalidate(delayed_time_error_);
  traits_ = 0x0;
}

void calibrated_tracker_hit::clear() { calibrated_tracker_hit::invalidate(); }

bool calibrated_tracker_hit::is_bottom_cathode_missing() const {
  return get_trait_bit(missing_bottom_cathode);
}

bool calibrated_tracker_hit::is_top_cathode_missing() const {
  return get_trait_bit(missing_top_cathode);
}

bool calibrated_tracker_hit::are_both_cathodes_missing() const {
  return is_bottom_cathode_missing() && is_top_cathode_missing();
}

int32_t calibrated_tracker_hit::get_id() const { return get_hit_id(); }

int32_t calibrated_tracker_hit::get_module() const { return get_geom_id().get(0); }

int32_t calibrated_tracker_hit::get_side() const { return get_geom_id().get(1); }

int32_t calibrated_tracker_hit::get_layer() const { return get_geom_id().get(2); }

int32_t calibrated_tracker_hit::get_row() const { return get_geom_id().get(3); }

bool compare_tracker_hit_by_delayed_time::operator()(const calibrated_tracker_hit& lhs,
                                                     const calibrated_tracker_hit& rhs) const {
  double dti = 0.0;
  if (lhs.is_delayed()) {
    dti = lhs.get_delayed_time();
  }
  double dtj = 0.0;
  if (rhs.is_delayed()) {
    dtj = rhs.get_delayed_time();
  }
  return (dti < dtj);
}

void calibrated_tracker_hit::tree_dump(std::ostream& out, const std::string& title,
                                       const std::string& indent, bool is_last) const {
  base_hit::tree_dump(out, title, indent, true);

  std::ostringstream prefix_os;
  prefix_os << indent << datatools::i_tree_dumpable::tag;
  std::string prefix = prefix_os.str();

  out << prefix << "Traits : " << traits_ << std::endl;
  out << prefix << "Delayed : " << is_delayed() << std::endl;
  out << prefix << "Noisy : " << is_noisy() << std::endl;
  out << prefix << "Peripheral : " << is_peripheral() << std::endl;
  out << prefix << "Bottom cathode missing : " << is_bottom_cathode_missing() << std::endl;
  out << prefix << "Top cathode missing : " << is_top_cathode_missing() << std::endl;
  out << prefix << "Sterile : " << is_sterile() << std::endl;
  out << prefix << "Fake : " << is_fake() << std::endl;
  if (has_xy()) {
    out << prefix << "x : " << x_ / CLHEP::mm << " mm" << std::endl;
    out << prefix << "y : " << y_ / CLHEP::mm << " mm" << std::endl;
  }
  if (is_delayed()) {
    out << prefix << "delayed_time : " << delayed_time_ / CLHEP::microsecond << " us" << std::endl;
    out << prefix << "delayed_time_error : " << delayed_time_error_ / CLHEP::microsecond << " us"
        << std::endl;
  }
  out << prefix << "r : " << r_ / CLHEP::mm << " mm" << std::endl;
  out << prefix << "sigma(r) : " << sigma_r_ / CLHEP::mm << " mm" << std::endl;
  out << prefix << "z : " << z_ / CLHEP::cm << " cm" << std::endl;
  out << indent << datatools::i_tree_dumpable::inherit_tag(is_last)
      << "sigma(z) : " << sigma_z_ / CLHEP::cm << " cm" << std::endl;
}

}  // end of namespace datamodel

}  // end of namespace snemo
