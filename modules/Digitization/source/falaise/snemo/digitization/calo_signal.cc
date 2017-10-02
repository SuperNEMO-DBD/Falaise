// snemo/digitization/calo_signal.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_signal.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>

namespace snemo {

namespace digitization {

// Serial tag for datatools::serialization::i_serializable interface :
// DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_signal,
// "snemo::digitalization::calo_signal")

constexpr double calo_signal::DELAYED_PM_TIME;

calo_signal::calo_signal() {
  datatools::invalidate(_amplitude_);
  return;
}

calo_signal::~calo_signal() {
  reset();
  return;
}

void calo_signal::set_header(int32_t calo_signal_hit_id_,
                             const geomtools::geom_id& electronic_id_) {
  set_hit_id(calo_signal_hit_id_);
  set_geom_id(electronic_id_);
  return;
}

void calo_signal::set_data(const double& signal_time_, const double& amplitude_) {
  set_signal_time(signal_time_);
  set_amplitude(amplitude_);
  return;
}

double calo_signal::get_signal_time() const { return _signal_time_; }

void calo_signal::set_signal_time(const double& signal_time_) {
  _signal_time_ = signal_time_ + DELAYED_PM_TIME;
  return;
}

double calo_signal::get_amplitude() const { return _amplitude_; }

void calo_signal::set_amplitude(const double& amplitude_) {
  _amplitude_ = amplitude_;
  return;
}

bool calo_signal::is_valid() const { return true; }

void calo_signal::reset() {
  geomtools::base_hit::reset();
  return;
}

void calo_signal::tree_dump(std::ostream& out_, const std::string& title_,
                            const std::string& indent_, bool /*inherit_*/) const {
  base_hit::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Signal time =  : " << _signal_time_
       << " ns " << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag << "Amplitude =  : " << _amplitude_ << " mV "
       << std::endl;

  return;
}

}  // end of namespace digitization

}  // end of namespace snemo
