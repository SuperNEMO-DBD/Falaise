// mapping_tp.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H

namespace snemo {
  
  namespace digitization {

    class mapping 
    {
    public :
      /// Electronic id index coming from a geom id	
      enum tp_electronic_ID_index {
				RACK_INDEX  = 0,
				CRATE_INDEX = 1,
				BOARD_INDEX = 2
      };

			static const int32_t TRACKER_CONTROL_BOARD_TYPE = 42;
			static const int32_t CALORIMETER_CONTROL_BOARD_TYPE = 666;

			static const int32_t CALO_RACK_ID = 3;
			static const int32_t TRIGGER_RACK_ID = 3;
			static const int32_t GEIGER_RACK_ID = 5;

			static const int32_t MAX_NUMBER_OF_CRATE = 2;
			static const int32_t XWALL_GVETO_CRATE_ID = 2;
			static const int32_t TRIGGER_CRATE_ID = 2;

			static const int32_t CONTROL_BOARD_ID = 10;
			static const int32_t TRIGGER_BOARD_ID = 20;
    };
	       
  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
