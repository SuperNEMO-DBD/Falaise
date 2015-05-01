// snemo/digitization/mapping.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_H

// Boost : 
#include <boost/cstdint.hpp>

namespace snemo {
  
  namespace digitization {

    class mapping 
    {
    public :
      /// Electronic id index coming from a geom id	
      enum tp_electronic_ID_index {
				RACK_INDEX    = 0,
				CRATE_INDEX   = 1,
				BOARD_INDEX   = 2,
				CHANNEL_INDEX = 3
      }; 

			enum tp_geom_ID_index {
				MODULE_INDEX    = 0,
				SIDE_INDEX      = 1,
				LAYER_INDEX     = 2,
				COLUMN_INDEX    = 2,
				WALL_INDEX      = 2,
				ROW_INDEX       = 3,
				XWALL_ROW_INDEX = 4
			};
			
			enum tp_electronic_ID_depth {
				RACK_DEPTH    = 1,
				CRATE_DEPTH   = 2,
				BOARD_DEPTH   = 3,
				CHANNEL_DEPTH = 4
			};

			enum tp_tracker_wire_mode {
				// INVALID_WIRES_TRACKER_MODE = -1,
				// THREE_WIRES_TRACKER_MODE   = 0,
				// TWO_WIRES_TRACKER_MODE     = 1
			};

			enum tp_tracker_side_mode {
				INVALID_TRACKER_SIDE_MODE = -1,
				TRACKER_ONE_SIDE_MODE  = 0,
				TRACKER_TWO_SIDES_MODE = 1			
			};
			
			static const int INVALID_MODULE_NUMBER = -1;
			static const int DEMONSTRATOR_MODULE_NUMBER = 0;
			
			static const std::string  & geiger_type();
			static const std::string  & main_calo_type();
			static const std::string  & x_wall_type();
			static const std::string  & gveto_type();
			
			static const int32_t GEIGER_CATEGORY_TYPE             = 1204;
			static const int32_t CALO_MAIN_WALL_CATEGORY_TYPE     = 1302;
			static const int32_t CALORIMETER_X_WALL_CATEGORY_TYPE = 1232;
			static const int32_t CALORIMETER_GVETO_CATEGORY_TYPE  = 1251;

			static const int32_t FEB_CATEGORY_TYPE = 666; 

			static const bool THREE_WIRES_TRACKER_MODE = 0;
			static const bool TWO_WIRES_TRACKER_MODE   = 1;
			static const bool SIDE_MODE = 1;
			static const int32_t  NUMBER_OF_CONNECTED_ROWS = 7;
			
			static const int32_t TRACKER_CONTROL_BOARD_TYPE     = 666;
			static const int32_t CALORIMETER_CONTROL_BOARD_TYPE = 666;

			static const int32_t CALO_RACK_ID    = 3;
			static const int32_t TRIGGER_RACK_ID = 3;
			static const int32_t GEIGER_RACK_ID  = 5;

			static const uint32_t NUMBER_OF_SIDES = 2;
			static const uint32_t NUMBER_OF_WALLS = 2;

			static const uint32_t NUMBER_OF_LAYERS = 9;
			static const uint32_t NUMBER_OF_MAIN_CALO_COLUMNS = 20;
			static const uint32_t NUMBER_OF_MAIN_CALO_ROWS = 13;

			static const uint32_t NUMBER_OF_X_CALO_COLUMNS = 2;
			static const uint32_t NUMBER_OF_X_CALO_ROWS = 16;

			static const uint32_t NUMBER_OF_GVETO_COLUMNS = 16;
			static const uint32_t NUMBER_OF_GEIGER_ROWS = 113;
			static const int32_t  NUMBER_OF_TRACKER_TRIGGER_ZONES = 10;
			static const int32_t  NUMBER_OF_SUB_TRACKER_TRIGGER_ZONES = 4;

			static const uint32_t NUMBER_OF_CRATES  = 2;
			static const uint32_t NUMBER_OF_FEBS_BY_CRATE = 19;
			static const int32_t XWALL_GVETO_CRATE_ID = 2;
			static const int32_t TRIGGER_CRATE_ID     = 2;

			static const int32_t CONTROL_BOARD_ID = 10;
			static const int32_t TRIGGER_BOARD_ID = 20;
			
			/// Row limit to take accountthere is no FEB ID 10 for every crate
			static const int32_t BOARD_ID_SHIFT_CRATE_0_LIMIT = 19;
			static const int32_t BOARD_ID_SHIFT_CRATE_2_LIMIT = 94;
			/// Shift to take account there is no FEB ID 10 (place of the Control Board)
			static const int32_t NO_FEB_NUMBER_10_SHIFT = 2;

			static const int32_t THREE_WIRES_CRATE_0_LIMIT     = 37;
			static const int32_t THREE_WIRES_CRATE_1_BEGINNING = 38;
			static const int32_t THREE_WIRES_LONELY_ROW        = 56;
			static const int32_t THREE_WIRES_CRATE_1_LIMIT     = 74;
			static const int32_t THREE_WIRES_CRATE_2_BEGINNING = 75;

			static const int32_t GEIGER_LAYERS_SIZE = 9;
			static const int32_t GEIGER_ROWS_SIZE   = 113;

    };
	       
  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
