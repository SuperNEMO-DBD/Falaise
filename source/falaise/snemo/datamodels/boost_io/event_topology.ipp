// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/boost_io/event_topology.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_IPP
#define FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/event_topology.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

// This project:
#include <falaise/snemo/datamodels/boost_io/vertex_utils.ipp>
#include <falaise/snemo/datamodels/boost_io/reconstructed_particle.ipp>

namespace snemo {

  namespace datamodel {
    
    template <class Archive>
    void particle_track::serialize(Archive & ar_ , const unsigned int version_)
    {
      ar_ & boost::serialization::make_nvp("decay_timestamp", _decay_timestamp_);
      ar_ & boost::serialization::make_nvp("decay_vertex", _decay_vertex_handle_);
      ar_ & boost::serialization::make_nvp("particles", _particles_);
      ar_ & boost::serialization::make_nvp("likelihood", _likelihood_);       
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_IPP
