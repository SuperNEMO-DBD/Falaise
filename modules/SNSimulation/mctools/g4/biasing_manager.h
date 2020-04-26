/// \file mctools/g4/biasing_manager.h
/* Author(s) :  Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *              François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-20
 * Last modified: 2014-11-20
 *
 * License:
 *
 * Description:
 *
 *   GEANT 4 biasing manager
 *
 * History:
 *
 */

#ifndef SNSIM_BIASING_MANAGER_H
#define SNSIM_BIASING_MANAGER_H 1

// Standard library:
#include <string>
// #include <map>
// #include <vector>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>

namespace datatools {
// Forward declaration:
class properties;
}  // namespace datatools

namespace snsim {

// Forward declaration:
class detector_construction;

/// \brief Manager for Geant4 biasing algorithm associated to elements of the geometry
class biasing_manager {
 public:
  /// Default constructor
  biasing_manager();

  /// Destructor
  virtual ~biasing_manager();

  /// Set the reference to the parent detector construction
  void set_detector_construction(detector_construction &dc_);

  /// Check initialization flag
  bool is_initialized() const;

  /// Initialize from a container of properties
  void initialize(const datatools::properties &);

  // Reset
  void reset();

 protected:
  void _set_default();

 private:
  bool _initialized_;           //!< Initialization flag
  detector_construction *_dc_;  //!< Handle to the parent detector construction
};

}  // namespace snsim

#endif  // SNSIM_BIASING_MANAGER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
