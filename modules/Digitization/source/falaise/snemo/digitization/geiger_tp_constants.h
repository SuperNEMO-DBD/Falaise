// snemo/digitization/geiger_tp_constants.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_CONSTANTS_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_CONSTANTS_H

namespace snemo {
  
  namespace digitization {

    static const unsigned int GEIGER_TP_CONSTANTS_HARDWARE_STATUS_SIZE = 5;
    static const unsigned int GEIGER_TP_CONSTANTS_BOARD_ID_WORD_SIZE   = 5;
    static const unsigned int GEIGER_TP_CONSTANTS_CRATE_ID_WORD_SIZE   = 2;
    static const unsigned int GEIGER_TP_CONSTANTS_TP_FULL_SIZE         = 100;
	       
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
