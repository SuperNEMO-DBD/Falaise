/// \file falaise/snemo/datamodels/tracker_trajectory_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: SuperNEMO Tracker trajectory data model
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_DATA_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_DATA_H 1

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/properties.h>

// This project::
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

namespace snemo {

  namespace datamodel {

    /// SuperNEMO Tracker trajectory data model
    class tracker_trajectory_data : public datatools::i_serializable,
                                    public datatools::i_tree_dumpable,
                                    public datatools::i_clear
    {
    public:

      /// Collection of handles on tracker trajectory solutions
      typedef std::vector<tracker_trajectory_solution::handle_type> solution_col_type;

      /// Default constructor
      tracker_trajectory_data();

      /// Destructor:
      virtual ~tracker_trajectory_data();

      /// Check if there are some trajectory solutions
      bool has_solutions() const;

      /// Returns the number of solutions
      size_t get_number_of_solutions() const;

      /// Add a trajectory solution
      void add_solution(const tracker_trajectory_solution::handle_type & handle_,
                        bool default_solution_ = false);

      //tracker_trajectory_solution & grab_solution(int i_);

      /// Return a non mutable reference to a trajectory solution by index
      const tracker_trajectory_solution & get_solution(int i_) const;

      /// Reset the trajectory solutions
      void invalidate_solutions();

      /// Check if there is some default trajectory solution
      bool has_default_solution() const;

      const tracker_trajectory_data::solution_col_type & get_solutions() const;

      tracker_trajectory_data::solution_col_type & grab_solutions();

      /// Return a non mutable reference to the default trajectory solution is any
      const tracker_trajectory_solution & get_default_solution() const;

      /// Reset the default trajectory solution is any
      void invalidate_default_solution();

      /// Set the default trajectory solution
      void set_default_solution(int index_);

      /// Reset the internals
      void reset();

      /// Return a mutable reference on the container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return a non mutable reference on the container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Clear the object
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private :

      solution_col_type _solutions_;                               //!< Collection of tracker trajectory solution handles
      tracker_trajectory_solution::handle_type _default_solution_; //!< Handle to the default/best solution
      datatools::properties _auxiliaries_;                         //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION();

    };  // end of class tracker_trajectory_data

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::tracker_trajectory_data,
                        "snemo::datamodel::tracker_trajectory_data")

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
