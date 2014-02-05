/** \file falaise/snemo/datamodels/tracker_clustering_data.h */
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-05
 * Last modified: 2014-01-27
 *
 * Description:  SuperNEMO Tracker clustering data model
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_H 1

// Standard library
#include <typeinfo>
#include <sstream>
#include <vector>
#include <stdexcept>

// Third party:
// - Boost
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/utils.h>
#include <datatools/properties.h>

// This project
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO Tracker clustering data model
    class tracker_clustering_data : DATATOOLS_SERIALIZABLE_CLASS,
                                    public datatools::i_tree_dumpable,
                                    public datatools::i_clear
    {
    public:

      /// Collection of handles on tracker clustering solutions
      typedef std::vector<tracker_clustering_solution::handle_type> solution_col_type;

      /// Default constructor
      tracker_clustering_data();

      /// Destructor:
      virtual ~tracker_clustering_data();

      /// Check if there are some clustering solutions
      bool has_solutions() const;

      /// Returns the number of solutions
      size_t get_number_of_solutions() const;

      /// Add a clustering solution
      void add_solution(const tracker_clustering_solution::handle_type & handle_,
                        bool default_solution_ = false);

      //tracker_clustering_solution & grab_solution(int i_);

      /// Return a non mutable reference to a clustering solution by index
      const tracker_clustering_solution & get_solution(int i_) const;

      /// Reset the clustering solutions
      void invalidate_solutions();

      /// Check if there is some default clustering solution
      bool has_default_solution() const;

      const tracker_clustering_data::solution_col_type & get_solutions() const;

      tracker_clustering_data::solution_col_type & grab_solutions();

      /// Return a non mutable reference to the default clustering solution is any
      const tracker_clustering_solution & get_default_solution() const;

      /// Return a mutable reference to the default clustering solution is any
      tracker_clustering_solution & grab_default_solution();

      /// Reset the default clustering solution is any
      void invalidate_default_solution();

      /// Set the default clustering solution
      void set_default_solution(int index_);

      /// Reset the internals
      void reset();

      /// Check if the object has a valid internal structure
      bool is_valid() const;

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

      bool                         _debug_;                        //!< Debug flag
      solution_col_type            _solutions_;                    //!< Collection of Geiger cluster solution handles
      tracker_clustering_solution::handle_type _default_solution_; //!< Handle to the default/best solution
      datatools::properties        _auxiliaries_;                  //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::tracker_clustering_data, "snemo::datamodel::tracker_clustering_data")

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_H

// end of falaise/snemo/datamodels/tracker_clustering_data.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
