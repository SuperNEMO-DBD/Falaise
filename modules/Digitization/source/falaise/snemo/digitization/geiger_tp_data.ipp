// geiger_tp_data.ipp
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_IPP
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_IPP

// Ourselves:
#include <snemo/digitization/geiger_tp_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/vector.hpp>

// This project :
#include <snemo/digitization/geiger_tp.ipp>
	 
namespace snemo {
  
  namespace digitization {

    template<class Archive>
    void geiger_tp_data::serialize(Archive & ar_, const unsigned int /* version_ */)
    {
      // Inherit from the 'datatools::i_serializable' base class:
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;

		 	ar_ & boost::serialization::make_nvp("locked_tps", _locked_tps_);
		 	ar_ & boost::serialization::make_nvp("geiger_tps", _geiger_tps_);
		
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
