/// \file falaise/datamodels/sim_tracker_digi_hit.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <bayeux/geomtools/base_hit.ipp>

namespace snemo {

	namespace datamodel {

		template <class Archive>
		void sim_tracker_digi_hit::serialize(Archive& ar, const unsigned int /* version */) {
			ar & boost::serialization::make_nvp("geomtools__base_hit",
																					boost::serialization::base_object<geomtools::base_hit>(*this));

			ar & boost::serialization::make_nvp("elec_id", _elec_id_);

			if (has_anode_R0()) {
				ar & boost::serialization::make_nvp("anode_R0", _anode_R0_);
			}
			if (has_anode_R1()) {
				ar & boost::serialization::make_nvp("anode_R1", _anode_R1_);
			}
			if (has_anode_R2()) {
				ar & boost::serialization::make_nvp("anode_R2", _anode_R2_);
			}
			if (has_anode_R3()) {
				ar & boost::serialization::make_nvp("anode_R3", _anode_R3_);
			}
			if (has_anode_R4()) {
				ar & boost::serialization::make_nvp("anode_R4", _anode_R4_);
			}
			if (has_cathode_R5()) {
				ar & boost::serialization::make_nvp("cathode_R5", _cathode_R5_);
			}
			if (has_cathode_R6()) {
				ar & boost::serialization::make_nvp("cathode_R6", _cathode_R6_);
			}
			return;
		}

	}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
