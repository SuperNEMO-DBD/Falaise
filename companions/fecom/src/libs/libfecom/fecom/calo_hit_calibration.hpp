//! \file fecom/calo_hit_calibration.hpp
//
// Copyright (c) 2017 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_HIT_CALIBRATION_HPP
#define FECOM_CALO_HIT_CALIBRATION_HPP

// Standard library:
#include <iostream>
#include <string>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/utils.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>

namespace fecom {

//! \brief Commissioning calorimeter hit calibration
struct calo_hit_calibration {
  /// Constructor
  calo_hit_calibration();

  /// Destructor
  virtual ~calo_hit_calibration();

  /// Calibrate hit, 1 MeV <-> 300 mV
  void calibrate_hit(const geomtools::geom_id& calo_gid_, const double amplitude_, double& energy_,
                     double& sigma_energy_);
};

}  // namespace fecom

#endif  // FECOM_CALO_HIT_CALIBRATION_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
