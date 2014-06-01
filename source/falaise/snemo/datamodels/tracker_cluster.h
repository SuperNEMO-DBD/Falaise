/** \file falaise/snemo/datamodels/tracker_cluster.h */
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-05
 * Last modified: 2014-01-27
 *
 * Description: A cluster of Geiger calibrated hits referenced by handles
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_H 1

// Third party:
// - Boost
#include <boost/cstdint.hpp>
// - Bayeux/datatools
#include <datatools/handle.h>
#include <datatools/properties.h>
// - Bayeux/geomtools
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief A cluster of Geiger calibrated hits referenced by handles
    class tracker_cluster : public geomtools::base_hit
    {
    public:

      /// Handle on tracker cluster
      typedef datatools::handle<tracker_cluster> handle_type;

      /// Flag for a auxiliary property
      static const std::string & delayed_cluster_flag();

      /// Check if the cluster is associated to delayed hits
      bool is_delayed() const;

      /// Check if the cluster is associated to prompt hits
      bool is_prompt() const;

      /// Mark the cluster as associated to delayed hits
      void make_delayed();

      /// Mark the cluster as associated to prompt hits
      void make_prompt();

      /// Default constructor
      tracker_cluster();

      /// Destructor
      virtual ~tracker_cluster();

      /// Check if there is a valid cluster ID
      bool has_cluster_id() const;

      /// Get the cluster ID
      int get_cluster_id() const;

      /// Set the cluster ID
      void set_cluster_id(int32_t);

      /// Invalidate the cluster ID
      void invalidate_cluster_id();

      /// Return a mutable reference on the container of handles on calibrated tracker hits
      calibrated_tracker_hit::collection_type & grab_hits();

      /// Return a non mutable reference on the container of handles on calibrated tracker hits
      const calibrated_tracker_hit::collection_type & get_hits() const;

      /// Return a non mutable reference on the calibrated tracker hit given its index
      const calibrated_tracker_hit & get_hit(int i_) const;

      /// Reset/invalidate the contents of the tracker cluster
      void clear();

      /// Reset/invalidate the tracker cluster(see clear)
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private:

      calibrated_tracker_hit::collection_type _hits_; /// Collection of Geiger hit handles

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTER_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
