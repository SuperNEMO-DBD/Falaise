// -*- mode: c++ ; -*-
// falaise/snemo/datamodels/raw_trigger_infos.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_RAW_TRIGGER_INFOS_IPP
#define FALAISE_SNEMO_DATAMODEL_RAW_TRIGGER_INFOS_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/raw_trigger_infos.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/bitset.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template<class Archive>
    void raw_trigger_infos::serialize(Archive & ar, const unsigned int /* version */)
    {
      ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar & boost::serialization::make_nvp("trigger_number", _trigger_number_);
      ar & boost::serialization::make_nvp("VTZW", _VTZW_);
      ar & boost::serialization::make_nvp("CTZW", _CTZW_);
      ar & boost::serialization::make_nvp("TTP",  _TTP_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_RAW_TRIGGER_INFOS_IPP
