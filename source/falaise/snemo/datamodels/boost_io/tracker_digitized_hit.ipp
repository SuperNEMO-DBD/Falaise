// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/tracker_digitized_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_TRACKER_DIGITIZED_HIT_IPP
#define FALAISE_SNEMO_DATAMODEL_TRACKER_DIGITIZED_HIT_IPP 1

// This project:
#include <falaise/snemo/datamodels/tracker_digitized_hit.h>

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
    void tracker_digitized_hit::rtd_origin::serialize(Archive & ar_,
                                                      const unsigned int /* version */)
    {
      ar_ & boost::serialization::make_nvp("hit_number", _hit_number_);
      ar_ & boost::serialization::make_nvp("trigger_id", _trigger_id_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void tracker_digitized_hit::gg_times::serialize(Archive & ar_,
                                                    const unsigned int /* version */)
    {
      ar_ & boost::serialization::make_nvp("anode_origins", _anode_origins_);
      ar_ & boost::serialization::make_nvp("anode_times", _anode_times_);
      ar_ & boost::serialization::make_nvp("bottom_cathode_origin", _bottom_cathode_origin_);
      ar_ & boost::serialization::make_nvp("bottom_cathode_time", _bottom_cathode_time_);
      ar_ & boost::serialization::make_nvp("top_cathode_origin", _top_cathode_origin_);
      ar_ & boost::serialization::make_nvp("top_cathode_time",  _top_cathode_time_);
      return;
    }

    /// Serialization method
    template <class Archive>
    void tracker_digitized_hit::serialize(Archive & ar_,
                                          const unsigned int /* version */)
    {
      // Inherit from the 'base_hit' mother class:
      ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                           boost::serialization::base_object<geomtools::base_hit>(*this));
      ar_ & boost::serialization::make_nvp("times", _times_);
      // ar_ & boost::serialization::make_nvp("cell_id", _cell_id_);
      return;
    }

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_TRACKER_DIGITIZED_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
