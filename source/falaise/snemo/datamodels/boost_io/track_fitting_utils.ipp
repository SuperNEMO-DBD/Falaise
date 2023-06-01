// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/boost_io/track_fitting_utils.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_IPP
#define FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/track_fitting_utils.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

namespace snemo {

  namespace datamodel {

    /// Serialization
    template <class Archive>
    void track_fit_infos::serialize(Archive & ar_, const unsigned int /* version_ */)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp("algo", _algo_);
      ar_ & boost::serialization::make_nvp("guess", _guess_);
      ar_ & boost::serialization::make_nvp("t0", _t0_);
      ar_ & boost::serialization::make_nvp("chi2", _chi2_);
      ar_ & boost::serialization::make_nvp("ndof", _ndof_);
      ar_ & boost::serialization::make_nvp("pvalue", _pvalue_);
      ar_ & boost::serialization::make_nvp("best", _best_);
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_IPP
