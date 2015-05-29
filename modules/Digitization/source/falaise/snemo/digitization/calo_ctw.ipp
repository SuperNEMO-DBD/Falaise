// snemo/digitization/calo_ctw.ipp
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_IPP
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_IPP

// Ourselves:
#include <snemo/digitization/calo_ctw.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/bitset.hpp>
// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>
	 
namespace snemo {
  
  namespace digitization {

    template<class Archive>
    void calo_ctw::serialize (Archive            & ar,
															const unsigned int  version)
    {
      // inherit from the 'base_hit' mother class:
      ar & boost::serialization::make_nvp ("geomtools__base_hit",
																					 boost::serialization::base_object<geomtools::base_hit>(*this));

      ar & boost::serialization::make_nvp ("locked_ctw", _locked_);

      if (_store & STORE_CLOCKTICK_25NS)
				{
					ar & boost::serialization::make_nvp ("clocktick_25ns", _clocktick_25ns_);
				}
		
      if (_store & STORE_CTW)
				{
					ar & boost::serialization::make_nvp ("TP", _ctw_);
				}

      return;
    }

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
