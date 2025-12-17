/// \file falaise/snemo/datamodels/boost_io/reconstructed_particle.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_PARTICLE_IPP
#define FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_PARTICLE_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/reconstructed_particle.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

// - Bayeux/geomtools:
// #include <geomtools/blur_spot.ipp>

// This project:

namespace snemo {

  namespace datamodel {
    
    template <class Archive>
    void reconstructed_particle::serialize(Archive & ar_ , const unsigned int version_)
    {
      ar_ & boost::serialization::make_nvp("particle_code", _particle_code_);
      ar_ & boost::serialization::make_nvp("delayed", _delayed_);
      ar_ & boost::serialization::make_nvp("track_handle", _track_handle__);
 
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_PARTICLE_IPP
