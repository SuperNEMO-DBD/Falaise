/// \file falaise/snemo/datamodels/tracker_cluster.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-05
 * Last modified: 2022-12-16
 *
 * Description: A cluster of Geiger calibrated hits referenced by handles
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief A cluster of Geiger calibrated hits referenced by handles
    class tracker_cluster
      : public geomtools::base_hit
    {
    public:
  
      /// Check if the cluster is associated to delayed hits
      bool is_delayed() const;

      /// Check if the cluster is associated to prompt hits
      bool is_prompt() const;

      /// Mark the cluster as associated to delayed hits
      void make_delayed();

      /// Mark the cluster as associated to prompt hits
      void make_prompt();

      /// Get the cluster ID
      int get_cluster_id() const;

      /// Set the cluster ID
      void set_cluster_id(int32_t);

      /// Return a mutable reference on the container of handles on calibrated tracker hits
      TrackerHitHdlCollection & hits();

      /// Return a non mutable reference on the container of handles on calibrated tracker hits
      const TrackerHitHdlCollection & hits() const;

      /// Return the number of hits in the cluster
      size_t size() const;

      /// Return a non mutable reference on the calibrated tracker hit given its index
      const calibrated_tracker_hit & at(size_t index) const;

      /// Reset/invalidate the contents of the tracker cluster
      void clear() override;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_
                      /**/ = datatools::i_tree_dumpable::empty_options()) const override;

    private:
      
      TrackerHitHdlCollection hits_; //!< Collection of Geiger hit handles
      bool delayed_ = false;
      
      DATATOOLS_SERIALIZATION_DECLARATION()
  
    };

    using TrackerCluster = tracker_cluster;
    using TrackerClusterCollection = std::vector<TrackerCluster>;

    using TrackerClusterHdl = datatools::handle<TrackerCluster>;
    using TrackerClusterHdlCollection = std::vector<TrackerClusterHdl>;

  } // end of namespace datamodel

} // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::tracker_cluster, 1)

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
