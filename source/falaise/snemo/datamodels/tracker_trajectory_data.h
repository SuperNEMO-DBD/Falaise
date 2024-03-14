/// \file falaise/snemo/datamodels/tracker_trajectory_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2024-03-13
 *
 * Description: SuperNEMO Tracker trajectory data model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_DATA_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_DATA_H 1

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_clear.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

// This project::
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

namespace snemo {

  namespace datamodel {

    /// SuperNEMO Tracker trajectory data model
    class tracker_trajectory_data
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
      , public datatools::i_clear
    {
    public:
			
      /// Check if there are some trajectory solutions
      bool has_solutions() const;

      /// Returns the number of solutions
      size_t get_number_of_solutions() const;

      /// Add a trajectory solution
      void add_solution(const TrackerTrajectorySolutionHdl & solution, bool is_default = false);

      /// Return a non mutable reference to a trajectory solution by index
      const tracker_trajectory_solution & get_solution(size_t index) const;

      /// Reset the trajectory solutions
      void invalidate_solutions();

      /// Check if there is some default trajectory solution
      bool has_default_solution() const;

      const TrackerTrajectorySolutionHdlCollection & get_solutions() const;

      TrackerTrajectorySolutionHdlCollection & get_solutions();

      /// Return a non mutable reference to the default trajectory solution is any
      const tracker_trajectory_solution & get_default_solution() const;

      /// Reset the default trajectory solution is any
      void invalidate_default_solution();

      /// Set the default trajectory solution
      void set_default_solution(size_t index);

      /// Reset the internals
      void reset();

      /// Clear the object
      void clear() override;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:
      
      TrackerTrajectorySolutionHdlCollection solutions_; //!< Collection of tracker trajectory solutions
      TrackerTrajectorySolutionHdl default_; //!< The default solution
      datatools::properties _auxiliaries_; //!< Auxiliaries properties

      DATATOOLS_SERIALIZATION_DECLARATION()

    }; // end of class tracker_trajectory_data

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
