// snemo/digitization/mapping.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

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
				BOARD_INDEX = 2,
				CHANNEL_INDEX = 3
      };			
			
			static const bool THREE_WIRES_TRACKER_MODE = 0;
			static const bool TWO_WIRES_TRACKER_MODE   = 1;
			static const bool SIDE_MODE = 1;
			static const int  NUMBER_OF_CONNECTED_ROWS = 7;
			
			static const int32_t TRACKER_CONTROL_BOARD_TYPE     = 666;
			static const int32_t CALORIMETER_CONTROL_BOARD_TYPE = 666;

			static const int32_t CALO_RACK_ID    = 3;
			static const int32_t TRIGGER_RACK_ID = 3;
			static const int32_t GEIGER_RACK_ID  = 5;

			static const int32_t MAX_NUMBER_OF_CRATE  = 2;
			static const int32_t MAX_NUMBER_OF_FEB_BY_CRATE = 19;
			static const int32_t XWALL_GVETO_CRATE_ID = 2;
			static const int32_t TRIGGER_CRATE_ID     = 2;

			static const int32_t CONTROL_BOARD_ID = 10;
			static const int32_t TRIGGER_BOARD_ID = 20;
			
			/// Row limit to take accountthere is no FEB ID 10 for every crate
			static const int32_t BOARD_ID_SHIFT_CRATE_0_LIMIT = 19;
			static const int32_t BOARD_ID_SHIFT_CRATE_2_LIMIT = 94;
			/// Shift to take account there is no FEB ID 10 (place of the Control Board)
			static const int32_t NO_FEB_10_SHIFT              = 2;

			static const int32_t THREE_WIRES_CRATE_0_LIMIT = 37;
			static const int32_t THREE_WIRES_CRATE_1_LIMIT = 74;
			static const int32_t THREE_WIRES_LONELY_ROW    = 56;

			static const int32_t GEIGER_LAYER_SIZE = 9;
			static const int32_t GEIGER_ROW_SIZE   = 113;

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
