// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/unified_digitized_data.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_UNIFIED_DIGITIZED_DATA_IPP
#define FALAISE_SNEMO_DATAMODEL_UNIFIED_DIGITIZED_DATA_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/unified_digitized_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>

// This project:
#include <falaise/snemo/datamodels/boost_io/calorimeter_digitized_hit.ipp>
#include <falaise/snemo/datamodels/boost_io/tracker_digitized_hit.ipp>

namespace snemo {

  namespace datamodel {

    template <class Archive>
    void unified_digitized_data::serialize(Archive & ar_, const unsigned int /* version */)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp("run_id",   _run_id_);
      ar_ & boost::serialization::make_nvp("event_id", _event_id_);
      ar_ & boost::serialization::make_nvp("reference_timestamp", _reference_timestamp_);
      ar_ & boost::serialization::make_nvp("calorimeter_digitized_hits", _calorimeter_digitized_hits_);
      ar_ & boost::serialization::make_nvp("tracker_digitized_hits", _tracker_digitized_hits_);
      ar_ & boost::serialization::make_nvp("origin_trigger_ids", _origin_trigger_ids_);
      ar_ & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_UNIFIED_DIGITIZED_DATA_IPP
