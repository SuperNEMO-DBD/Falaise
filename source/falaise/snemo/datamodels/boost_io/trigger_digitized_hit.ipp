// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/trigger_digitized_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_TRIGGER_DIGITIZED_HIT_IPP
#define FALAISE_SNEMO_DATAMODEL_TRIGGER_DIGITIZED_HIT_IPP 1

// This project:
#include <falaise/snemo/datamodels/trigger_digitized_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template <class Archive>
    void trigger_digitized_hit::rtd_origin::serialize(Archive & ar_,
                                                      const unsigned int /* version */)
    {
      ar_ & boost::serialization::make_nvp("hit_number", _hit_number_);
      ar_ & boost::serialization::make_nvp("trigger_id", _trigger_id_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void trigger_digitized_hit::serialize(Archive & ar_,
                                          const unsigned int /* version */)
    {
      // Inherit from the 'base_hit' mother class:
      ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                           boost::serialization::base_object<geomtools::base_hit>(*this));
      return;
    }

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_TRIGGER_DIGITIZED_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
