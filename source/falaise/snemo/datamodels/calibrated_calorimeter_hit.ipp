// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/calibrated_calorimeter_hit.ipp */

#ifndef FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_IPP 1

// Ourselves
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>

// Third party
// - Boost
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template<class Archive>
    void calibrated_calorimeter_hit::serialize (Archive & ar,
                                                const unsigned int /* version */)
    {
      ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP (base_hit);
      ar & boost::serialization::make_nvp ("energy",       _energy_);
      ar & boost::serialization::make_nvp ("sigma_energy", _sigma_energy_);
      ar & boost::serialization::make_nvp ("time",         _time_);
      ar & boost::serialization::make_nvp ("sigma_time",   _sigma_time_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_CALIBRATED_CALORIMETER_HIT_IPP

// end of falaise/snemo/datamodel/calibrated_calorimeter_hit.ipp
