// snemo/digitization/trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H

// Standard library :
#include <string>
#include <bitset>

// This project :
#include <snemo/digitization/calo_ctw_data.h>
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/tracker_trigger_algorithm.h>
#include <snemo/digitization/coincidence_trigger_algorithm.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>

namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {

    /// \brief Full trigger algorithm for the process. The trigger decision is taken here.
    class trigger_algorithm
    {
    public : 

      /// Default constructor
      trigger_algorithm();

      /// Destructor
      virtual ~trigger_algorithm();

      /// Set the electronic mapping object
      void set_electronic_mapping(const electronic_mapping & my_electronic_mapping_);

      /// Initializing
      void initialize();

      /// Initializing
      void initialize(const datatools::properties & config_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 
			
			/// Clear the record vectors
			void clear_records();

			/// Get the vector of calo summary record
			const std::vector<tracker_trigger_algorithm::tracker_record> get_tracker_records_vector() const;
			
			/// Get the vector of calo summary record
			const std::vector<calo_trigger_algorithm::calo_summary_record> get_calo_records_vector() const;
			
      /// General process
      void process(const calo_ctw_data & calo_ctw_data_,
									 const geiger_ctw_data & geiger_ctw_data_);

    protected :
			
			/// Process calo algorithm
			void _process_calo_algo(const calo_ctw_data & calo_ctw_data_);
			
			/// Process tracker algorithm
			void _process_tracker_algo(const geiger_ctw_data & geiger_ctw_data_);
			
			/// Process coincidence module algorithm			
			void _process_coinc_algo();
			
      /// Protected general process
      void _process(const calo_ctw_data & calo_ctw_data_,
										const geiger_ctw_data & geiger_ctw_data_);

    private :
     
      // Configuration :
      bool _initialized_; //!< Initialization flag
      const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID

		  tracker_trigger_algorithm   _tracker_algo_; //!< Tracker trigger algorithm @ 1600ns
		  calo_trigger_algorithm         _calo_algo_; //!< Calo trigger algorithm @ 25ns
			coincidence_trigger_algorithm _coinc_algo_; //!< Coincidence trigger algorithm for matching calo and tracker trigger
			
			// Data :
			std::vector<calo_trigger_algorithm::calo_summary_record> _calo_records_; //!< Collection of calo summary record
			std::vector<tracker_trigger_algorithm::tracker_record> _tracker_records_; //!< Collection of tracker record
			std::vector<coincidence_trigger_algorithm::coincidence_output> _coincidence_records_; //!< Collection of coincidence record
    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
