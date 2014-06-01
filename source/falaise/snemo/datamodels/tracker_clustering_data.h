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

    /// \brief SuperNEMO tracker clustering data model
    //
    // The tracker hits are initialy  sorted, by the so-called tracker
    // pre-clustering  algorithm,  within   time  bunches  of  typical
    // duration    ~10   us    (see   the    configuration   of    the
    // snemo::processing::base_tracker_clusterizer  class).
    //
    // The first expected bunch  is the so-called *prompt* pre-cluster
    // which contains all the trackeer  prompt hits that are collected
    // by  the  prompt  trigger  within  a  ~10  us  time  coincidence
    // window. Then  comes eventually *delayed* pre-clusters  that are
    // also collected by bunches of  ~10 us some arbitrary delay after
    // the prompt  bunch.  An event  should contains one and  only one
    // prompt pre-cluster and event can have zero or several *delayed*
    // pre-clusters.
    //
    //    ~10 us              ~10 us         ~10 us
    //    <----->             <---->         <------>
    // --[o-oo-oo]-----------[o-o--o]-------[o---oo-o]------------> time
    //   "prompt"            "delayed"      "delayed"
    //                        (id==0)        (id==1)
    //
    // The prompt  pre-cluster generally gathers all  prompt hits from
    // both  sides of  the source  frame.  This  is because  we expect
    // several prompt charged particle  tracks traversing any location
    // within the tracking chamber, and triggering tracker drift cells
    // everywhere in the detector, regardless of the side with respect
    // to  the  source frame.  It  is  expected  that the  full  event
    // topology  should be  reconstructed taking  into account  tracks
    // from both sides.  This is obviously the case when we search for
    // double-beta decay candidate events.
    //
    //      side==0   side==1
    //    +---------+---------+
    //    |  o      !         |
    //    |         !         |
    //    |         !         |
    //    |         !o        |
    //    |         ! oo      |
    //    |      ooo!  ooooo  |
    //    |   ooo   !     oooo|
    //    |ooo      !        o|
    //    |         !         |
    //    |         !         |
    //    |         !         |
    //    |         !         |
    //    |         !     o   |
    //    |         !         |
    //    |         !  o      |
    //    |         !         |
    //    |         !         |
    //    |         !         |
    //    +---------+---------+
    //
    // Delayed clusters are  built independantly on their  own side of
    // the source frame.  The purpose  of these clusters is the search
    // for delayed tracks mainly from the Bi214-Po214 decays which are
    // one of  the more  critical background  in hte  experiment. Each
    // delay cluster has its own delayed cluster Id.
    //
    // The tracker clustering algorithm  aims to gathered some tracker
    // hits  within  a  given  pre-cluster   and  build  from  them  a
    // collection  of tracker  clusters.  However,  several clustering
    // solutions  may exist  so the  final product  of the  clustering
    // algorithm  consists in  a  collection  of clustering  solutions
    // (a.k.a.   scenarii).  The  tracker clustering  data model  thus
    // stores a collection  of solutions. A subset  of these solutions
    // corresponds to  the clusterization  of the  prompt pre-cluster:
    // they are tagged with a dedicated *prompt* flag. Other subset of
    // clustering  solutions  may  have  been built  for  the  various
    // delayed  pre-clusters. In  such case,  each solution  is tagged
    // with a dedicated *delayed* flag  and an unique *delayed cluster
    // Id*.
    //
    // The contents  of a typical  tracker clustering data  object may
    // looks the  following, where  the prompt pre-clustered  has been
    // processed  by the  clustering  algorithm  which produced  three
    // different  clustering  scenarii,  namely   solution  0,  1  and
    // 2. Then, the two delayed pre-clusters have also been processed,
    // the first  one being associated  with two possible  solution (3
    // and 4) and the second one with only one solution (5):
    //
    //  [solution #0] prompt
    //  [solution #1] prompt
    //  [solution #2] prompt
    //  [solution #3] delayed cluster Id = 0
    //  [solution #4] delayed cluster Id = 0
    //  [solution #5] delayed cluster Id = 1
    //
    //
    class tracker_clustering_data : DATATOOLS_SERIALIZABLE_CLASS,
                                    public datatools::i_tree_dumpable,
                                    public datatools::i_clear
    {
    public:

      /// Key for the boolean property associated to prompt clustering solutions
      static const std::string & prompt_key();

      /// Key for the boolean property associated to delayed clustering solutions
      static const std::string & delayed_key();

      /// Key for the integer property associated to delayed clustering solutions
      static const std::string & delayed_id_key();

      /// Key for the string Id property documenting the clustering algorithm used to build a given clustering solution
      static const std::string & clusterizer_id_key();

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

      /// Return a non mutable reference to a clustering solution by index
      const tracker_clustering_solution & get_solution(int i_) const;

      /// Reset the clustering solutions
      void invalidate_solutions();

      /// Check if there is some default clustering solution
      bool has_default_solution() const;

      /// Return the non mutable reference to the collection of clustering solutions
      const tracker_clustering_data::solution_col_type & get_solutions() const;

      /// Return the mutable reference to the collection of clustering solutions
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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
