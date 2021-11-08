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
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/geomtools/i_shape_1d.h>

namespace snemo {

namespace datamodel {

/// \brief The base class of fitted trajectory pattern
class base_trajectory_pattern
  : public datatools::i_serializable
  , public datatools::i_tree_dumpable
{
 public:

  enum direction_type
    {
     DIRECTION_NONE     = 0,
     DIRECTION_BACKWARD = datatools::bit_mask::bit01,
     DIRECTION_FORWARD  = datatools::bit_mask::bit00,
     DIRECTION_BOTH     = DIRECTION_FORWARD | DIRECTION_BACKWARD
    };
  
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

  virtual geomtools::vector_3d get_first() const = 0;

  virtual geomtools::vector_3d get_first_direction() const = 0;

  virtual geomtools::vector_3d get_last() const = 0;

  virtual geomtools::vector_3d get_last_direction() const = 0;

  virtual unsigned int number_of_kinks() const = 0;
  
  virtual geomtools::vector_3d get_kink(unsigned int kink_index_) const = 0;
 
  virtual geomtools::vector_3d get_kink_direction(unsigned int kink_index_,
                                                  direction_type dir_) const = 0;
  
  virtual void tree_dump(std::ostream& out = std::clog,
                         const std::string& title = "",
                         const std::string& indent = "",
                         bool inherit_ = false) const override;

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
