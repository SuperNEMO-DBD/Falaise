// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/topology_1eNg_pattern.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1ENG_PATTERN_IPP
#define FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1ENG_PATTERN_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/topology_1eNg_pattern.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

// This project:
#include <falaise/snemo/datamodels/base_topology_pattern.ipp>

namespace snemo {

  namespace datamodel {

    /// Serialization method
    template<class Archive>
    void topology_1eNg_pattern::serialize(Archive & ar_, const unsigned int /* version_ */)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(topology_1e_pattern);
      ar_ & boost::serialization::make_nvp("number_of_gammas", _number_of_gammas_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1ENG_PATTERN_IPP
