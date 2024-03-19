/// \file falaise/snemo/datamodels/tracker_clustering_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-05
 * Last modified: 2024-03-13
 *
 * Description:  SuperNEMO Tracker clustering data model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_H 1

// Standard library:
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_clear.h>
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/utils.h>

// This project:
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO tracker clustering data model
    //
    // The tracker hits are initialy  sorted, by the so-called tracker
    // pre-clustering  algorithm,  within   time  bunches  of  typical
    // duration    ~10   us    (see   the    configuration   of    the
    // snemo::processing::base_tracker_clusterizer  of snemo::processing::base_tracking_driver classes).
    //
    // The first expected bunch  is the so-called *prompt* pre-cluster
    // which contains all the tracker  prompt hits that are collected
    // by  the  prompt  trigger  within  a  ~10  us  time  coincidence
    // window. Then  comes eventually *delayed* pre-clusters  that are
    // also collected by bunches of  ~10 us with some arbitrary delay after
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
    // one of  the more  critical background  in the  experiment. Each
    // delay cluster has its own delayed cluster ID.
    //
    // The tracker clustering algorithm  aims to gathered some tracker
    // hits  within  a  given  pre-cluster   and  build  from  them  a
    // collection  of tracker  clusters.  However,  several clustering
    // solutions  may exist  so the  final product  of the  clustering
    // algorithm  consists in  a  collection  of clustering  solutions
    // (a.k.a. clustering  scenarii).  The  tracker clustering  data model  thus
    // stores a collection  of solutions. A subset  of these solutions
    // corresponds to  the clusterization  of the  prompt pre-cluster:
    // they are tagged with a dedicated *prompt* flag. Other subsets of
    // clustering  solutions  may  have  been built  for  the  various
    // delayed  pre-clusters. In  such case,  each solution  is tagged
    // with a dedicated *delayed* flag  and an unique *delayed cluster
    // ID*.
    //
    // The contents  of a typical  tracker clustering data  object may
    // looks the  following, where  the prompt pre-clustered  has been
    // processed  by the  clustering  algorithm  which produced  three
    // different  clustering  scenarii,  namely   solution  0,  1  and
    // 2. Then, the two delayed pre-clusters have also been processed,
    // the first  one being associated  with two possible  solutions (3
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
    class tracker_clustering_data
			: public datatools::i_serializable
			, public datatools::i_tree_dumpable
			,	public datatools::i_clear
		{
    public:
			
      /// Check if there are some clustering solutions
      bool empty() const;

      /// Returns the number of solutions
      size_t size() const;

      /// Add a clustering solution
      void append_solution(const TrackerClusteringSolutionHdl & solution, bool isDefault = false);

      /// Return a non mutable reference to a clustering solution by index
      const tracker_clustering_solution & at(size_t index) const;

      /// Reset the clustering solutions
      void clear() override;

      /// Check if there is some default clustering solution
      bool has_default() const;

      /// Return the non mutable reference to the collection of clustering solutions
      const TrackerClusteringSolutionHdlCollection & solutions() const;

      /// Return the mutable reference to the collection of clustering solutions
      TrackerClusteringSolutionHdlCollection & solutions();

      /// Return a non mutable reference to the default clustering solution is any
      const tracker_clustering_solution & get_default() const;

      /// Return a mutable reference to the default clustering solution is any
      tracker_clustering_solution & get_default();

      /// Set the default clustering solution
      void set_default(size_t index);

			const datatools::properties & auxiliaries() const;

			datatools::properties & auxiliaries();
			
      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snemo::datamodel::tracker_clustering_data tcd;
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Tracker clustering data:");
      /// poptions.put("indent", ">>> ");
      /// poptions.put("list_solutions", true);
      /// poptions.put("solutions.list_clusters", true);
      /// poptions.put("solutions.list_unclustered_hits", true);
      /// poptions.put("solutions.list_properties", true);
			/// poptions.put("list_properties", true);
      /// tcd.print_tree(std::clog, poptions);
      /// \endcode
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;
  
    private:
			
      TrackerClusteringSolutionHdlCollection solutions_{};  //!< Collection of Geiger cluster solutions
      TrackerClusteringSolutionHdl default_{}; //!< Handle to the default solution
      datatools::properties _auxiliaries_{}; //!< Auxiliary properties
			
      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::tracker_clustering_data,
                        "snemo::datamodel::tracker_clustering_data")

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
