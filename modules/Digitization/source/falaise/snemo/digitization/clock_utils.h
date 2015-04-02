// snemo/digitization/clock_utils.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CLOCK_UTILS_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CLOCK_UTILS_TP_H

namespace snemo {
  
  namespace digitization {

    class clock_utils 
    {
      static const int32_t MAIN_CLOCKTICK = 25; //!< Main clocktick, 40 MHz => 25ns.
      static const int32_t TRACKER_CLOCKTICK = 800; //!< Clocktick for tracker, 800ns.
      
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
