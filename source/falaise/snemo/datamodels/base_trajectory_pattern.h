/// \file falaise/snemo/datamodels/base_trajectory_pattern.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: The base class of trajectory patterns
 */

#ifndef FALAISE_SNEMO_DATAMODEL_BASE_TRAJECTORY_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_BASE_TRAJECTORY_PATTERN_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/geomtools/i_shape_1d.h>

namespace snemo {

namespace datamodel {

/// \brief The base class of fitted trajectory pattern
class base_trajectory_pattern
  : public datatools::i_serializable
  , public datatools::i_tree_dumpable
{
 public:
  /// Constructors
  base_trajectory_pattern() = default;

  base_trajectory_pattern(std::string pid);

  /// Destructor
  virtual ~base_trajectory_pattern() = default;

  /// Check if a valid pattern ID exists
  bool has_pattern_id() const;

  /// Return the pattern ID
  const std::string& get_pattern_id() const;

  /// Return the reference to the 1D shape associated to the trajectory
  virtual const geomtools::i_shape_1d& get_shape() const = 0;

  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool is_last = false) const override;

 private:
  std::string _pattern_id_{""};  //!< The pattern identifier

  DATATOOLS_SERIALIZATION_DECLARATION()
};

using TrajectoryPattern = base_trajectory_pattern;
using TrajectoryPatternHdl = datatools::handle<TrajectoryPattern>;

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_BASE_TRAJECTORY_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
