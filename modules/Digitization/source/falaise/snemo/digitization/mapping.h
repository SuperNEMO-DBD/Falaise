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
