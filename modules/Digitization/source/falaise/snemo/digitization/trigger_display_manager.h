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

			const bool is_calo_25ns() const;
			const bool is_calo_1600ns() const;
			const bool is_tracker_1600ns() const;
			const bool is_coinc_1600ns() const;
			const bool is_decision_trigger() const;

			
			void display_calo_trigger_25ns() const;

			void display_calo_trigger_1600ns() const;
			
			void display_tracker_trigger_1600ns() const;

			void display_coincidence_trigger_1600ns() const;

			void display_decision_trigger() const;			
			

    private :
      
      // Configuration : 
      bool _initialized_;      //!< Initialization
      bool _calo_25ns_;        //!< Configuration to display calo@25ns
      bool _calo_1600ns_;      //!< Configuration to display calo@1600ns
      bool _tracker_1600ns_;   //!< Configuration to display tracker@1600ns
      bool _coinc_1600ns_;     //!< Configuration to display coinc@1600ns
			bool _decision_trigger_; //!< Configuration to display the moment when the trigger decision is true

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
