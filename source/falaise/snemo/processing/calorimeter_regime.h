// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/calorimeter_regime.h
 * Author (s) :     Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *                  Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2011-01-12
 * Last modified: 2014-02-24
 *
 * License:
 *
 * Description:
 *
 *   Tools to model the calorimeter regime in SuperNEMO.
 *
 * History:
 *
 */
#ifndef FALAISE_SNEMO_PROCESSING_CALORIMETER_REGIME_H
#define FALAISE_SNEMO_PROCESSING_CALORIMETER_REGIME_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party
// - Bayeux/datatools
#include <bayeux/mygsl/rng.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "falaise/config/property_set.h"


namespace snemo {

namespace processing {

/// \brief Simple modelling of the energy and time measurement with the SuperNEMO calorimeter
/// optical lines
class CalorimeterModel {
 public:
  CalorimeterModel() = default;
  explicit CalorimeterModel(falaise::config::property_set const& ps);

  /// Randomize the measured energy value given the true energy
  double randomize_energy(mygsl::rng& rng, const double energy) const;

  /// Return the error on energy
  double get_sigma_energy(const double energy) const;

  /// Compute the effective quenched energy for alpha particle
  double quench_alpha_energy(const double energy) const;

  /// Randomize the measured time value given the true time and energy
  double randomize_time(mygsl::rng& rng, const double time, const double energy) const;

  /// Return the error on time
  double get_sigma_time(const double energy) const;

  /// Check if a given energy passes the high threshold
  bool is_high_threshold(const double energy) const;

  /// Check if a given energy passes the low threshold
  bool is_low_threshold(const double energy) const;

 private:
  double highEnergyThreshold {150.*CLHEP::keV}; //!< High energy threshold
  double lowEnergyThreshold {50.*CLHEP::keV};   //!< Low energy threshold

  double energyResolution {8.*CLHEP::perCent};  //!< Energy resolution for electrons at 1 MeV
  double alphaQuenching_0 {77.4};               //!< Parameter 0 for alpha quenching
  double alphaQuenching_1 {0.639};              //!< Parameter 1 for alpha quenching
  double alphaQuenching_2 {2.34};               //!< Parameter 2 for alpha quenching
  double relaxationTime {6.*CLHEP::ns};         //!< Scintillator relaxation time
};

}  // end of namespace processing

}  // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::calorimeter_regime the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::CalorimeterModel)

#endif  // FALAISE_SNEMO_PROCESSING_CALORIMETER_REGIME_H

// end of falaise/snemo/processing/calorimeter_regime.h
