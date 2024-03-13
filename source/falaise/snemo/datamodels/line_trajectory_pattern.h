/// \file falaise/snemo/datamodels/line_trajectory_pattern.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: The line trajectory pattern
 */

#ifndef FALAISE_SNEMO_DATAMODEL_LINE_TRAJECTORY_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_LINE_TRAJECTORY_PATTERN_H 1

// Third party:
// - Bayeux/geomtools:
#include <geomtools/line_3d.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

namespace snemo {

  namespace datamodel {

    /// \brief The fitted line trajectory pattern
    class line_trajectory_pattern
      : public base_trajectory_pattern
    {
    public:
      
      /// Return pattern identifier of the pattern
      static const std::string & pattern_id();

      /// Default constructor
      line_trajectory_pattern();

      /// Destructor
      virtual ~line_trajectory_pattern() = default;

      /// Get a reference to the mutable line/segment embedded model
      geomtools::line_3d & get_segment();

      /// Get a reference to the non mutable line/segment embedded model
      const geomtools::line_3d & get_segment() const;

      /// Return the reference to the 1D shape associated to the trajectory
      const geomtools::i_shape_1d & get_shape() const override;

      geomtools::vector_3d get_first() const override;

      geomtools::vector_3d get_first_direction() const override;

      geomtools::vector_3d get_last() const override;

      geomtools::vector_3d get_last_direction() const override;

      unsigned int number_of_kinks() const override;
  
      geomtools::vector_3d get_kink(unsigned int kink_index_) const override;

      geomtools::vector_3d get_kink_direction(unsigned int kink_index_,
                                              direction_type dir_) const override;
    
    private:
  
      geomtools::line_3d _segment_{};  //!< The line/segment embedded model

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::line_trajectory_pattern,
                        "snemo::datamodel::line_trajectory_pattern")

#endif // FALAISE_SNEMO_DATAMODEL_LINE_TRAJECTORY_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
