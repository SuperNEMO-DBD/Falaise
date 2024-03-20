/// \file falaise/snemo/datamodels/precalibrated_cluster.h
/* Author: Emmanuel Chauveau <chauveau@lp2ib.in2p3.fr>
 * Creation date: 2024-01-24
 * Last modified: 2024-01-24
 *
 * Description: A cluster of precalibrated hits referenced by handles
 */

#ifndef FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CLUSTER_H
#define FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CLUSTER_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/precalibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief A cluster of precalibrated hits referenced by handles
    class precalibrated_cluster
      : public geomtools::base_hit
    {
    public:
  
      /// Get the cluster ID
      int get_cluster_id() const;

      /// Set the cluster ID
      void set_cluster_id(int32_t);

      /// Return a mutable reference on the container of handles on precalibrated calorimeter hits
      PreCalibratedCalorimeterHitHdlCollection & calorimeter_hits();

      /// Return a non mutable reference on the container of handles on precalibrated calorimeter hits
      const PreCalibratedCalorimeterHitHdlCollection & calorimeter_hits() const;

      /// Return the number of hits in the cluster
      size_t calorimeter_size() const;

      /// Return a mutable reference on the container of handles on precalibrated tracker hits
      PreCalibratedTrackerHitHdlCollection & tracker_hits();

      /// Return a non mutable reference on the container of handles on precalibrated tracker hits
      const PreCalibratedTrackerHitHdlCollection & tracker_hits() const;

      /// Return the number of hits in the cluster
      size_t tracker_size() const;

      /// Reset/invalidate the contents of the tracker cluster
      void clear() override;

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_
                              /**/ = datatools::i_tree_dumpable::empty_options()) const override;

    private:

      PreCalibratedCalorimeterHitHdlCollection _calorimeter_hits_; //!< Collection of calorimeter hit handles
      PreCalibratedTrackerHitHdlCollection     _tracker_hits_;     //!< Collection of tracker hit handles
      
      DATATOOLS_SERIALIZATION_DECLARATION()
  
    };

    using precalibrated_cluster_collection = std::vector<precalibrated_cluster>;
    using precalibrated_cluster_handle = datatools::handle<precalibrated_cluster>;
    using precalibrated_cluster_handle_collection = std::vector<precalibrated_cluster_handle>;

    using PreCalibratedratedCluster = precalibrated_cluster;
    using PreCalibratedratedClusterCollection = std::vector<PreCalibratedratedCluster>;

    using PreCalibratedratedClusterHdl = datatools::handle<PreCalibratedratedCluster>;
    using PreCalibratedratedClusterHdlCollection = std::vector<PreCalibratedratedClusterHdl>;

  } // end of namespace datamodel

} // end of namespace snemo

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(snemo::datamodel::precalibrated_cluster, 1)

#endif // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_CLUSTER_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
