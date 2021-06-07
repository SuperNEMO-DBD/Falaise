/// \file falaise/snemo/datamodels/helix_trajectory_pattern.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: The helix trajectory pattern
 */

#ifndef FALAISE_SNEMO_DATAMODEL_HELIX_TRAJECTORY_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_HELIX_TRAJECTORY_PATTERN_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/helix_3d.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

namespace snemo {

namespace datamodel {

/// \brief The fitted helix trajectory pattern
class helix_trajectory_pattern : public base_trajectory_pattern {
 public:
  /// Return pattern identifier of the pattern
  static const std::string& pattern_id();

  /// Default constructor
  helix_trajectory_pattern();

  /// Destructor
  virtual ~helix_trajectory_pattern() = default;

  /// Get a reference to the mutable helix embedded model
  geomtools::helix_3d& get_helix();

  /// Get a reference to the non mutable helix embedded model
  const geomtools::helix_3d& get_helix() const;

  /// Return the reference to the 1D shape associated to the trajectory
  virtual const geomtools::i_shape_1d& get_shape() const override;

 private:
  geomtools::helix_3d _helix_{};  //!< The helix embedded model

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::helix_trajectory_pattern,
                        "snemo::datamodel::helix_trajectory_pattern")

#endif  // FALAISE_SNEMO_DATAMODEL_HELIX_TRAJECTORY_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
