// snemo/digitization/geiger_tp_constants.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_CONSTANTS_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_CONSTANTS_H

namespace snemo {
  
  namespace digitization {
  
		namespace geiger {
 
			namespace tp {
				
				/// Position of bits in the Geiger tracker TP bitset word (100 bits)
				enum gg_tp_bit_pos {
					TP_BEGIN           = 0,   //!< Trigger primitive beginning
					TP_THREE_WIRES_END = 35,  //!< Trigger primitive ending for three wires mode
					TP_TWO_WIRES_END   = 54,  //!< Trigger primitive ending for two wires mode (or 53, to check)
					TP_END             = 54,  //!< Trigger primitive ending
					THWS_BEGIN         = 55,  //!< Tracker hardware status beginning
					TRM_BIT0           = 55,  //!< Tracker row mode (bit 0)
					TRM_BIT1           = 56,  //!< Tracker row mode (bit 1)
					TRM_BIT2           = 57,  //!< Tracker row mode (bit 2)
					TSM_BIT            = 58,  //!< Tracker side mode bit
					TTM_BIT            = 59,  //!< Tracker trigger mode bit (0 is 3-wires mode; 1 is 2-wires mode)
					THWS_END           = 59,  //!< Tracker hardware status ending
					ADDRESS_BEGIN      = 60,  //!< Board address beginning (board ID [5 bits] , crate ID [2 bits] + Spare [3 bits])
					BOARD_ID_BIT0      = 60,  //!< Board ID (bit 0) 
					BOARD_ID_BIT1      = 61,  //!< Board ID (bit 1)
					BOARD_ID_BIT2      = 62,  //!< Board ID (bit 2)
					BOARD_ID_BIT3      = 63,  //!< Board ID (bit 3)
					BOARD_ID_BIT4      = 64,  //!< Board ID (bit 4)
					CRATE_ID_BIT0      = 65,  //!< Crate ID (bit 0)
					CRATE_ID_BIT1      = 66,  //!< Crate ID (bit 1)
					ADDRESS_END        = 69,  //!< Board address ending
					TTID_BEGIN         = 70,  //!< Tracker trigger ID beginning
					TTID_END           = 84,  //!< Tracker trigger ID ending
					CONTROL_BEGIN      = 85,  //!< Control beginning
					CONTROL_END        = 99   //!< Control ending
				}; 

				/// Size of bits word in the Geiger tracker TP bitset word (100 bits)
				enum gg_tp_size {
					TP_SIZE            = 55,
					THWS_SIZE          = 5,
					TRM_WORD_SIZE      = 3,
					ADDRESS_SIZE       = 10,
					BOARD_ID_WORD_SIZE = 5,
					CRATE_ID_WORD_SIZE = 2,
					SPARE_WORD_SIZE    = 3,
					TTID_SIZE          = 15,
					CONTROL_SIZE       = 15,
					FULL_SIZE          = 100
				};

			} // end of namespace tp

		} // end of namespace geiger

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
