// fecom/calo_hit_calibration.cpp

// Ourselves:
#include <fecom/calo_hit_calibration.hpp>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

namespace fecom {

  calo_hit_calibration::calo_hit_calibration()
  {
  }

  calo_hit_calibration::~calo_hit_calibration()
  {
  }

  void calo_hit_calibration::calibrate_hit(const geomtools::geom_id & /*calo_gid_*/,
					   const double amplitude_,
					   double & energy_,
					   double & sigma_energy_)
  {
    energy_ = 1 * CLHEP::MeV * amplitude_ / 0.300 * CLHEP::volt; // 1 MeV <-> 300 mV
    sigma_energy_ = 0;
    return;
  }

} // namespace fecom
