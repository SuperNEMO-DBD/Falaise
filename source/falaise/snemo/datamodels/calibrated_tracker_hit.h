/// \file falaise/snemo/datamodels/calibrated_tracker_hit.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-15
 * Last modified: 2014-01-27
 *
 * Description:
 *
 *   Calibrated tracker hit
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_H
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
#include <datatools/handle.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace snemo {

namespace datamodel {

/// \brief Model of a calibrated tracker hit (Geiger regime)
class calibrated_tracker_hit : public geomtools::base_hit {
 public:
  /// \brief Special traits
  enum traits_type {
    none = 0x0,
    delayed = datatools::bit_mask::bit00,
    noisy = datatools::bit_mask::bit01,
    missing_bottom_cathode = datatools::bit_mask::bit02,
    missing_top_cathode = datatools::bit_mask::bit03,
    peripheral = datatools::bit_mask::bit04,
    xy = datatools::bit_mask::bit05,
    sterile = datatools::bit_mask::bit06,
    fake = datatools::bit_mask::bit07
  };

  /// Alias for a handle on a calibrated tracker hit
  typedef datatools::handle<calibrated_tracker_hit> handle_type;

  /// Alias for a collection of handles on calibrated tracker hits
  typedef std::vector<handle_type> collection_type;

  /// Name of the property to store optional anode time
  static const std::string& anode_time_key();

  /// Name of the property to store optional bottom cathode time
  static const std::string& bottom_cathode_time_key();

  /// Name of the property to store optional top cathode time
  static const std::string& top_cathode_time_key();

  /// Return the hit ID
  int32_t get_id() const;

  /// Return the module number
  int32_t get_module() const;

  /// Return the number of the tracker submodule
  int32_t get_side() const;

  /// Return the layer
  int32_t get_layer() const;

  /// Return the row
  int32_t get_row() const;

  /// Return the longitudinal position of the Geiger hit along the anode wire
  double get_z() const;

  /// Set the longitudinal position of the Geiger hit along the anode wire
  void set_z(double);

  /// Return the error on the longitudinal position of the Geiger hit along the anode wire
  double get_sigma_z() const;

  /// Set the error on the longitudinal position of the Geiger hit along the anode wire
  void set_sigma_z(double);

  /// Return the drift radius of the Geiger hit
  double get_r() const;

  /// Set the drift radius of the Geiger hit
  void set_r(double);

  /// Return the error on the drift radius of the Geiger hit
  double get_sigma_r() const;

  /// Set the error on the drift radius of the Geiger hit
  void set_sigma_r(double);

  /// Return the X position of the center of the cell in the module coordinates system
  double get_x() const;

  /// Return the Y position of the center of the cell in the module coordinates system
  double get_y() const;

  /// Check if the X/Y position of the center of the cell is stored
  bool has_xy() const;

  /// Set the X/Y position of the center of the cell in the module coordinates system
  void set_xy(double x_, double y_);

  /// Invalidate X/Y position of the center of the cell
  void invalidate_xy();

  /// Reset all calibration informations
  void invalidate_positions();

  /// Check if the anode drift time is stored as an auxiliary property
  bool has_anode_time() const;

  /// Store the anode drift time as an auxiliary property
  void set_anode_time(double);

  /// Get the anode drift time from a stored auxiliary property
  double get_anode_time() const;

  /// Interface to deal with trait bits
  bool get_trait_bit(uint32_t mask_) const;

  /// Check if the hit is marked as noisy
  bool is_noisy() const;

  /// Mark/unmark the hit as noisy
  void set_noisy(bool);

  /// Check if the hit is marked as delayed
  bool is_delayed() const;

  /// Mark/unmark the hit as delayed
  void set_delayed(bool);

  /// Check if the delayed time is stored
  bool has_delayed_time() const;

  /// Return the delayed reference time of the hit
  double get_delayed_time() const;

  /// Check if the delayed time error is stored
  bool has_delayed_time_error() const;

  /// Return the delayed reference time error of the hit
  double get_delayed_time_error() const;

  /// Set the hit as delayed and store its referece delayed time and associated error
  void set_delayed_time(double ref_delayed_time_, double ref_delayed_time_error_ = 0.0);

  /// Check if the hit is not marked as delayed
  bool is_prompt() const;

  /// Check if the hit is marked as peripheral
  bool is_peripheral() const;

  /// Mark/unmark the hit as peripheral
  void set_peripheral(bool);

  /// Check if the hit lacks the measurement of the bottom cathode signal
  bool is_bottom_cathode_missing() const;

  /// Mark/unmark the hit lacking the measurement of the bottom cathode signal
  void set_bottom_cathode_missing(bool);

  /// Check if the hit lacks the measurement of the top cathode signal
  bool is_top_cathode_missing() const;

  /// Mark/unmark the hit lacking the measurement of the bottom cathode signal
  void set_top_cathode_missing(bool);

  /// Check if the hit lacks both measurements of the top and bottom cathode signals
  bool are_both_cathodes_missing() const;

  /// Check if the hit is marked as sterile
  bool is_sterile() const;

  /// Mark/unmark the hit as sterile
  void set_sterile(bool);

  /// Check if the hit is marked as fake
  bool is_fake() const;

  /// Mark/unmark the hit as fake
  void set_fake(bool);

  /// Default constructor
  calibrated_tracker_hit();

  // Destructor
  virtual ~calibrated_tracker_hit();

  /// Check if minimal calibration informations are present to consider the hit as valid and usable
  bool is_valid() const;

  /// Invalidate calibration informations stored in the hit
  void invalidate();

  /// Invalidate calibration informations stored in the hit
  virtual void clear();

  /// Apply a measurement functor on the hit
  calibrated_tracker_hit& measure(i_measurement&);

  /// Smart print method
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

  /// Shortcut for the smart print method
  void dump() const;

 protected:
  /// Set the X position of the center of the cell in the module coordinates system
  void _set_x(double);

  /// Set the Y position of the center of the cell in the module coordinates system
  void _set_y(double);

  void _set_trait_bit(bool value_, uint32_t mask_);

  bool _get_trait_bit(uint32_t mask_) const;

 private:
  uint32_t _traits_;      //!< Bitset for special traits
  double _r_;             //!< Transverse drift distance within the cell coordinates system
  double _sigma_r_;       //!< Transverse drift distance error
  double _z_;             //!< Longitudinal position within the cell coordinates system
  double _sigma_z_;       //!< Longitudinal position error
  double _x_;             //!< X position of the anode wire within the module coordinates system
  double _y_;             //!< Y position of the anode wire within the module coordinates system
  double _delayed_time_;  //!< Delayed reference time
  double _delayed_time_error_;  //!< Delayed reference time error

  DATATOOLS_SERIALIZATION_DECLARATION()
};

/// Functor that compares hits by delayed time
struct compare_tracker_hit_by_delayed_time {
  bool operator()(const calibrated_tracker_hit& hit_i_, const calibrated_tracker_hit& hit_j_) const;
};

}  // end of namespace datamodel

}  // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::calibrated_tracker_hit, 1)

#endif  // FALAISE_SNEMO_DATAMODELS_CALIBRATED_TRACKER_HIT_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
