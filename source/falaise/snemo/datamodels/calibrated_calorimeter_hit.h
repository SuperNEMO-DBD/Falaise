// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/calibrated_calorimeter_hit.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2014-01-27
 *
 * License:
 *
 * Description:
 *   Calibrated calorimeter hit
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_H
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_H 1

// Third party:
// - Bayeux/datatools:
//#if defined(__clang__)
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wunused-local-typedefs"
//#endif
//#if defined(__GNUC__)
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused"
//#endif
#include <datatools/handle.h>
//#if defined(__GNUC__)
//#pragma GCC diagnostic pop
//#endif
//#if defined(__clang__)
//#pragma clang diagnostic pop
//#endif

// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace snemo {

namespace datamodel {

/// \brief Model of a calibrated calorimeter hit
class calibrated_calorimeter_hit : public geomtools::base_hit {
 public:
  /// Return the time associated to the hit
  double get_time() const;

  /// Set the time associated to the hit
  void set_time(double);

  /// Return the error on the time associated to the hit
  double get_sigma_time() const;

  /// Set the error on the time associated to the hit
  void set_sigma_time(double);

  /// Return the energy associated to the hit
  double get_energy() const;

  /// Set the energy associated to the hit
  void set_energy(double);

  /// Return the error on the energy associated to the hit
  double get_sigma_energy() const;

  /// Set the error on the energy associated to the hit
  void set_sigma_energy(double);

  /// Check if the internal data of the hit are valid
  bool is_valid() const;

  /// Invalidate the internal data of hit
  void invalidate();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 private:
  double _energy_{datatools::invalid_real()};        //!< Energy associated to the hit
  double _sigma_energy_{datatools::invalid_real()};  //!< Error on the energy associated to the hit
  double _time_{datatools::invalid_real()};          //!< Time associated to the hit
  double _sigma_time_{datatools::invalid_real()};    //!< Error on the time associated to the hit

  DATATOOLS_SERIALIZATION_DECLARATION()
};

/// Handle of calibrated calorimeter hit
// typedef datatools::handle<calibrated_calorimeter_hit> handle_type;
/// Collection of handles of calibrated calorimeter hit
// typedef std::vector<handle_type> collection_type;
using CalorimeterHit = calibrated_calorimeter_hit;
using CalorimeterHitCollection = std::vector<CalorimeterHit>;

using CalorimeterHitHdl = datatools::handle<CalorimeterHit>;
using CalorimeterHitHdlCollection = std::vector<CalorimeterHitHdl>;

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_H
