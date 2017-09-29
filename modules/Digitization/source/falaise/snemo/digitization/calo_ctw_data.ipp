// snemo/digitization/calo_ctw_data.ipp
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_IPP
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_IPP

// Ourselves:
#include <snemo/digitization/calo_ctw_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// This project :
#include <snemo/digitization/calo_ctw.ipp>

namespace snemo {

namespace digitization {

template <class Archive>
void calo_ctw_data::serialize(Archive& ar_, const unsigned int /* version_ */) {
  // Inherit from the 'datatools::i_serializable' base class:
  ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;

  ar_& boost::serialization::make_nvp("calo_ctws", _calo_ctws_);

  return;
}

}  // end of namespace digitization

}  // end of namespace snemo

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
