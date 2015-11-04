// snemo/digitization/coincidence_trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_H

// Standard library :
#include <string>
#include <bitset>

// This project :
#include <snemo/digitization/calo_ctw_data.h>
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/tracker_trigger_algorithm.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>

namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {
		
		class coincidence_trigger_algorithm
		{
		
		public :
		
			/// Default constructor
			coincidence_trigger_algorithm();

			/// Destructor
			virtual ~coincidence_trigger_algorithm();

			/// Set the electronic mapping object
			void set_electronic_mapping(const electronic_mapping & my_electronic_mapping_);

			/// Initializing
			void initialize_simple();

			/// Initializing
			void initialize(const datatools::properties & config_);

			/// Check if the algorithm is initialized 
			bool is_initialized() const;

			/// Reset the object
			void reset();
			
			/// General process
			void process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
									 const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_);
			
		protected :
			
			/// Process calo records and tracker records
			void _process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
										const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_);
			
		private :

      // Configuration :
      bool _initialized_; //!< Initialization flag
      const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID

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
