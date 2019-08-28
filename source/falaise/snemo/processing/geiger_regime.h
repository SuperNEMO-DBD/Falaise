// -*- mode: c++ ; -*-
/* * \file falaise/snemo/processing/geiger_regime.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2014-02-24
 *
 * License:
 *
 * Description:
 *
 *   Tools to model the Geiger regime in SuperNEMO drift cells.
 *   It combines both digitization (radius to time) *and* calibration
 *   (time to radius) - to be refactored!
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_GEIGER_REGIME_H
#define FALAISE_SNEMO_PROCESSING_GEIGER_REGIME_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
// - Bayeux/mygsl:
#include <bayeux/mygsl/rng.h>
#include <bayeux/mygsl/tabulated_function.h>

namespace snemo {

namespace processing {

/// \brief Modelling of the Geiger regime of the SuperNEMO drift cell
class geiger_regime : public datatools::i_tree_dumpable {
 public:
  /// Default constructor
  geiger_regime();

  /// Initialization from parameters
  explicit geiger_regime(const datatools::properties& dps);

  // RoF defaults
  virtual ~geiger_regime() = default;
  geiger_regime(const geiger_regime&) = default;
  geiger_regime& operator=(const geiger_regime&) = default;
  geiger_regime(geiger_regime&&) = default;
  geiger_regime& operator=(geiger_regime&&) = default;

  /// Return the diameter of the cell
  double getCellDiameter() const;

  /// Return the radius of the circle fitting into the cell
  double getCellRadius() const;

  /// Return distance from center to corner of cell
  double getMaximumRadius() const;

  /// Return the length of the cell
  double getCellLength() const;

  /// Return the error on anode drift time
  double getAnodeTimeResolution(double anode_time_) const;

  /// Return the error on cathode drift time
  double getCathodeTimeResolution() const;

  /// Return the time to drift a distance equal to the cell's radius
  double getDriftTimeForCellRadius() const;

  /// Return the maximum drift time to be calibrated
  double getMaximumDriftTime() const;

  /// Return the anode efficiency
  double getAnodeEfficiency(double radius) const;

  /// Return the cathode efficiency
  double getCathodeEfficiency() const;

  /// Return the plasma longitudinal speed
  double getPlasmaSpeed() const;

  /// Return the error on the plasma longitudinal speed
  double getPlasmaSpeedError() const;

  /// Randomize the longitudinal position of a Geiger hit
  double smearZ(mygsl::rng& ran_, double z_, double sigma_z_) const;

  /// Randomize the drift position of a Geiger hit
  double smearRadius(mygsl::rng& ran_, double r_) const;

  /// Randomize the drift time from the drift distance of a Geiger hit
  double getRandomTimeGivenRadius(mygsl::rng& ran_, double drift_distance_) const;

  /// Return the error on longitudinal position
  double getLongitudinalResolution(double z, size_t missing_cathode = 0) const;

  /// Return the error on the drift distance at a given radius
  double getRadialResolution(double r) const;

  /// Calibrate the drift radius and error from the drift time
  void calibrateRadiusFromTime(double drift_time_, double& drift_radius_,
                               double& sigma_drift_radius_) const;

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool inherit = false) const;

 private:
  /// Compute the drift radius from the drift time
  double base_t_2_r(double time_, int mode_ = 0) const;

  double cellRadius_;                         //!< Fiducial drift radius of a cell
  double cellDiagonal_;                       //!< Radius of circle containing corners of cell
  double cellLength_;                         //!< Fiducial drift length of a cell
  double anodeTimeResolution_;                //!< Anode TDC resolution
  double cathodeTimeResolution_;              //!< Cathode TDC resolution
  double zResolution_;                        //!< Longitudinal resolution
  double zResolutionSingleCathode_;           //!< Longitudinal resolution with single cathode
  double rResolution_a_;                      //!< Parameters 0 for transverse resolution
  double rResolution_b_;                      //!< Parameters 1 for transverse resolution
  double rResolution_r0_;                     //!< Parameters 2 for transverse resolution
  double coreAnodeEfficiency_;                //!< Anode efficiency inside _r0_
  double coreCathodeEfficiency_;              //!< Cathode efficiency inside _r0_
  double plasmaSpeed_;                        //!< Plasma longitudinal speed
  double plasmaSpeedError_;                   //!< Error on plasma longitudinal speed
  mygsl::tabulated_function timeFromRadius_;  //!< drift radius->time function
  double timeToDriftCellRadius_;              //!< Drift time equivalent to cell radius
  double tCut_;  //!< Cut on drift time (related, maybe identical, to threshold for delayed hits)
};

}  // end of namespace processing
}  // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::geiger_regime the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::geiger_regime)

#endif  // FALAISE_SNEMO_PROCESSING_GEIGER_REGIME_H

// end of falaise/snemo/processing/geiger_regime.h
