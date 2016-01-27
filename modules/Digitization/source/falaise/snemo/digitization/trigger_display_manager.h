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
#include <snemo/digitization/trigger_info.h>


namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {

		class trigger_algorithm;	
	
    /// \brief This is the trigger display manager
    /// It can display : 
		/// - Calorimeter zones each 25ns or 1600ns
		/// - Geiger matrix each 1600ns
		/// - Coincidences between calorimeter zones and Geiger matrix each 1600ns
		// /// - Coincidence when the trigger decision is true for the first time

    class trigger_display_manager
    {
    public :
			
			/// Number of horizontal characters for display
			static const uint8_t NUMBER_OF_HORIZONTAL_CHAR = 117;

			/// Number of vertical characters for display
			static const uint8_t NUMBER_OF_VERTICAL_CHAR = 21;
			
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
			
			/// Check if the calorimeter 25ns config is activated 
			const bool is_calo_25ns() const;

			/// Check if the calorimeter 1600ns config is activated 
			const bool is_calo_1600ns() const;

			/// Check if the tracker 1600ns config is activated 
			const bool is_tracker_1600ns() const;

			/// Check if the coincidence 1600ns config is activated 
			const bool is_coinc_1600ns() const;

			/// Check if the decision trigger config is activated 
			const bool is_decision_trigger() const;

			/// Fill calorimeter zones for 25ns
			void fill_calo_trigger_matrix_25ns(std::bitset<10> zoning_word_[trigger_info::NSIDES]);
			
			/// Fill calorimeter zones for 1600ns
			void fill_calo_trigger_matrix_1600ns(std::bitset<10> zoning_word_[trigger_info::NSIDES]);
						
			/// Fill tracker matrix for 1600ns
			void fill_tracker_trigger_matrix_1600ns(bool geiger_matrix_[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS]);
			// Fill matrix for coincidences for 1600ns
			void fill_coincidence_trigger_matrix_1600ns(std::bitset<10> zoning_word_[trigger_info::NSIDES], bool geiger_matrix_[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS]);

			/// Display calorimeter zones each 25ns for a given clocktick
			void display_calo_trigger_25ns(trigger_algorithm & a_trigger_algo_, uint32_t clocktick_25ns_);

			/// Display calorimeter zones each 25ns for all available clocktick
			void display_calo_trigger_25ns(trigger_algorithm & a_trigger_algo_); 

			/// Display calorimeter zones each 1600 for a given clocktick
			void display_calo_trigger_1600ns(trigger_algorithm & a_trigger_algo_, uint32_t clocktick_1600ns_);

			/// Display calorimeter zones each 1600ns for all available clocktick 
			void display_calo_trigger_1600ns(trigger_algorithm & a_trigger_algo_);
			
			/// Display tracker matrix each 1600 for a given clocktick
			void display_tracker_trigger_1600ns(trigger_algorithm & a_trigger_algo_, uint32_t clocktick_1600ns_);

			/// Display tracker matrix each 1600 for all available clocktick
			void display_tracker_trigger_1600ns(trigger_algorithm & a_trigger_algo_);

			/// Display calorimeter zones and tracker matrix each 1600 for a given clocktick
			void display_coincidence_trigger_1600ns(trigger_algorithm & a_trigger_algo_, uint32_t clocktick_1600ns_);

			/// Display calorimeter zones and tracker matrix each 1600 for all available clocktick
			void display_coincidence_trigger_1600ns(trigger_algorithm & a_trigger_algo_);
			
			/// Display calorimeter zones and tracker matrix each 1600 for the clocktick decision
			void display_decision_trigger();			
			
			/// Fill display matrix with the initial pattern 
			void fill_matrix_pattern();
		 
			/// Reset matrix with the constructed pattern
			void reset_matrix_pattern();
			
			/// Reset calorimeter zones display
			void reset_calo_display();

			/// Reset tracker matrix display
			void reset_tracker_display();
						
			/// Display the total matrix
			void display_matrix();

    private :
      
      // Configuration : 
      bool _initialized_;      //!< Initialization
      bool _calo_25ns_;        //!< Configuration to display calo@25ns
      bool _calo_1600ns_;      //!< Configuration to display calo@1600ns
      bool _tracker_1600ns_;   //!< Configuration to display tracker@1600ns
      bool _coinc_1600ns_;     //!< Configuration to display coinc@1600ns
			bool _decision_trigger_; //!< Configuration to display the moment when the trigger decision is true

			// Data :
			char _char_matrix_[NUMBER_OF_VERTICAL_CHAR][NUMBER_OF_HORIZONTAL_CHAR]; //!< Matrix of characters representing calorimeter zones and geiger matrix			
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
