// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/tracker_trajectory.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_IPP
#define FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/base_trajectory_pattern.ipp>
#include <falaise/snemo/datamodels/boost_io/calibrated_tracker_hit.ipp>
#include <falaise/snemo/datamodels/boost_io/tracker_cluster.ipp>
#include <falaise/snemo/datamodels/boost_io/track_fitting_utils.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization
    template <class Archive>
    void tracker_trajectory::serialize(Archive & ar_, const unsigned int version_)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar_ & boost::serialization::make_nvp("cluster", cluster_);
      ar_ & boost::serialization::make_nvp("orphans", orphans_);
      ar_ & boost::serialization::make_nvp("pattern", pattern_);
      if (version_ >= 1) {
        ar_ & boost::serialization::make_nvp("fit_infos", fit_infos_);
      } else {
        if (Archive::is_loading::value) {
          // Original version 0:
          // Load fit infos struc from properties:
          fit_infos_.reset();
          if (get_auxiliaries().has_key("guess")) {
            fit_infos_.set_guess(get_auxiliaries().fetch_string("guess"));
            if (fit_infos_.get_guess().size() == 2) {
              fit_infos_.set_algo(TRACK_FIT_ALGO_LINE);
            }
            if (fit_infos_.get_guess().size() == 3) {
              fit_infos_.set_algo(TRACK_FIT_ALGO_HELIX);
            }
          }
          if (get_auxiliaries().has_key("chi2")) {
            fit_infos_.set_chi2(get_auxiliaries().fetch_real("chi2"));
          }
          if (get_auxiliaries().has_key("ndof")) {
            fit_infos_.set_ndof(get_auxiliaries().fetch_integer("ndof"));
          }
          if (get_auxiliaries().has_key("pvalue")) {
            fit_infos_.set_pvalue(get_auxiliaries().fetch_real("pvalue"));
          }
          if (get_auxiliaries().has_key("best")) {
            fit_infos_.set_best(get_auxiliaries().fetch_boolean("best"));
          }
        }
      }
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_TRAJECTORY_IPP
