// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/precalibrated_tracker_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void precalibrated_tracker_hit::serialize(Archive& ar_, const unsigned int /* version_ */) {
      ar_& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
    }

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_PRECALIBRATED_TRACKER_HIT_IPP
