/// \file falaise/snemo/datamodels/polyline_trajectory_pattern.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-28
 *
 * Description: The polyline trajectory pattern
 */

#ifndef FALAISE_SNEMO_DATAMODEL_POLYLINE_TRAJECTORY_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_POLYLINE_TRAJECTORY_PATTERN_H 1

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>
// - Bayeux/geomtools:
#include <geomtools/polyline_3d.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

namespace snemo {

namespace datamodel {

/// The fitted polyline trajectory pattern
class polyline_trajectory_pattern : public base_trajectory_pattern {
 public:
  /// Return pattern identifier of the pattern
  static const std::string& pattern_id();

  /// Default constructor
  polyline_trajectory_pattern();

  /// Destructor
  virtual ~polyline_trajectory_pattern() = default;

  /// Get a reference to the mutable polyline path embedded model
  geomtools::polyline_3d& get_path();

  /// Get a reference to the non mutable polyline path embedded model
  const geomtools::polyline_3d& get_path() const;

  /// Return the reference to the 1D shape associated to the trajectory
  virtual const geomtools::i_shape_1d& get_shape() const override;

  geomtools::vector_3d get_first() const override;

  geomtools::vector_3d get_first_direction() const override;

  geomtools::vector_3d get_last() const override;

  geomtools::vector_3d get_last_direction() const override;

  unsigned int number_of_kinks() const override;
  
  geomtools::vector_3d get_kink(unsigned int kink_index_) const override;

  geomtools::vector_3d get_kink_direction(unsigned int kink_index_,
                                          direction_type dir_) const override;
 
 private:
  geomtools::polyline_3d _path_{}; //!< The polyline path embedded model

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_POLYLINE_TRAJECTORY_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
