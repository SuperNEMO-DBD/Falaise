// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/raw_data.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_RAW_DATA_IPP
#define FALAISE_SNEMO_DATAMODEL_RAW_DATA_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/raw_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

// This project:
#include <falaise/snemo/datamodels/raw_calorimeter_hit.ipp>
#include <falaise/snemo/datamodels/raw_tracker_hit.ipp>

namespace snemo {

  namespace datamodel {

    template<class Archive>
    void raw_data::serialize (Archive & ar, const unsigned int /* version */)
    {
      ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar & boost::serialization::make_nvp ("raw_trigger_infos",    _raw_trigger_infos_);
      ar & boost::serialization::make_nvp ("raw_calorimeter_hits", _raw_calorimeter_hits_);
      ar & boost::serialization::make_nvp ("raw_tracker_hits",     _raw_tracker_hits_);
      ar & boost::serialization::make_nvp ("auxiliaries",          _auxiliaries_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::raw_data, 1)

#endif // FALAISE_SNEMO_DATAMODEL_RAW_DATA_IPP
