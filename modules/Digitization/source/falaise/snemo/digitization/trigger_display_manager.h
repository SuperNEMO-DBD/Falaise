// snemo/digitization/trigger_display_manager.h
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_DISPLAY_MANAGER_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_DISPLAY_MANAGER_H

// Standard library :
#include <string>
#include <bitset>

// This project :
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>

namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {
    
    /// \brief 
    /// bla bla bla
    class trigger_display_manager
    {
    public :

      /// Default constructor
      trigger_display_manager();

      /// Destructor
      virtual ~trigger_display_manager();

      /// Initializing
      void initialize_simple();

      /// Initializing
      void initialize(const datatools::properties & config_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;
      
      /// Reset the object
      void reset();

    private :
      
      // Configuration : 
      bool _initialized_; //!< Initialization
      
    };
					
  } // end of namespace digitization
	
} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCDIDENCE_TRIGGER_ALGORITHM_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
