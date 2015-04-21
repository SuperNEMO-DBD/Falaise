// snemo/digitization/electronic_mapping.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H

// - Boost:
#include <boost/cstdint.hpp>

namespace snemo {
  
  namespace digitization {

    class electronic_mapping 
    {
    public :
      /// Default constructor
      electronic_mapping();

      /// Destructor
      virtual ~electronic_mapping();

      /// Initializing
      void initialize();

      /// Check if the object is initialized 
      bool is_initialized();

      /// Reset the object : 
      void reset();
      
    private :
      
      bool _initialized_; //!< Initialization flag
    };
	       
  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ELECTRONIC_MAPPING_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
