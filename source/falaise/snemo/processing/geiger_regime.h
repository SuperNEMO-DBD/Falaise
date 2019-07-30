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
#include <bayeux/mygsl/tabulated_function.h>
#include <bayeux/mygsl/rng.h>


namespace snemo {

namespace processing {

/// \brief Modelling of the Geiger regime of the SuperNEMO drift cell
class geiger_regime : public datatools::i_tree_dumpable {
 public:
  /// Default constructor
  geiger_regime();

  /// Initialization from parameters
  explicit geiger_regime(const datatools::properties& config_);

  // RoF defaults
  virtual ~geiger_regime() = default;
  geiger_regime(const geiger_regime&) = default;
  geiger_regime& operator=(const geiger_regime&) = default;
  geiger_regime(geiger_regime&&) = default;
  geiger_regime& operator=(geiger_regime&&) = default;

  /// Return the diameter of the cell
  double get_cell_diameter() const;

  /// Return the radius of the cell
  double get_cell_radius() const;

  /// Return the length of the cell
  double get_cell_length() const;

  /// Return the error on anode drift time
  double get_sigma_anode_time(double anode_time_) const;

  /// Return the error on cathode drift time
  double get_sigma_cathode_time() const;

  /// Return the reference time
  double get_t0() const;

  /// Return the maximum drift time to be calibrated
  double get_tcut() const;

  /// Radius of a cell
  double get_r0() const;

  /// Maximum sensitive distance from the anode wire
  double get_rdiag() const;

  /// Return the base anode efficiency
  double get_base_anode_efficiency() const;

  /// Return the base cathode efficiency
  double get_base_cathode_efficiency() const;

  /// Return the anode efficiency
  double get_anode_efficiency(double r_) const;

  /// Return the cathode efficiency
  double get_cathode_efficiency() const;

  /// Return the plasma longitudinal speed
  double get_plasma_longitudinal_speed() const;

  /// Return the error on the plasma longitudinal speed
  double get_sigma_plasma_longitudinal_speed() const;

  /// Randomize the longitudinal position of a Geiger hit
  double randomize_z(mygsl::rng& ran_, double z_, double sigma_z_) const;

  /// Randomize the drift position of a Geiger hit
  double randomize_r(mygsl::rng& ran_, double r_) const;

  /// Randomize the drift time from the drift distance of a Geiger hit
  double randomize_drift_time_from_drift_distance(mygsl::rng& ran_, double drift_distance_) const;

  /// Compute the drift radius from the drift time
  double base_t_2_r(double time_, int mode_ = 0) const;

  /// Return the error on longitudinal position
  double get_sigma_z(double z, size_t missing_cathode = 0) const;

  /// Return the error on the drift distance
  double get_sigma_r(double r_) const;

  /// Calibrate the drift radius from the drift time
  void calibrate_drift_radius_from_drift_time(double drift_time_, double& drift_radius_,
                                              double& sigma_drift_radius_) const;

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool inherit = false) const;

 private:
  double _cell_diameter_;                    //!< Fiducial drift diameter of a cell
  double _cell_length_;                      //!< Fiducial drift length of a cell
  double _sigma_anode_time_;                 //!< Anode TDC resolution
  double _sigma_cathode_time_;               //!< Cathode TDC resolution
  double _sigma_z_;                          //!< Longitudinal resolution
  double _sigma_z_missing_cathode_;          //!< Longitudinal resolution for one missing cathode
  double _sigma_r_a_;                        //!< Parameters 0 for transverse resolution
  double _sigma_r_b_;                        //!< Parameters 1 for transverse resolution
  double _sigma_r_r0_;                       //!< Parameters 2 for transverse resolution
  double _base_anode_efficiency_;            //!< Base anode efficiency
  double _base_cathode_efficiency_;          //!< Base cathode efficiency
  double _plasma_longitudinal_speed_;        //!< Plasma longitudinal speed
  double _sigma_plasma_longitudinal_speed_;  //!< Error on plasma longitudinal speed
  mygsl::tabulated_function _base_rt_;       //!< drift time<->drift radius function
  double _t0_;                               //!< Time (not documented yet)
  double _r0_;                               //!< Radius (not documented yet)
  double _rdiag_;                            //!< Cut on drift radius (not documented yet)
  double _tcut_;                             //!< Cut on drift time (not documented yet)
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
