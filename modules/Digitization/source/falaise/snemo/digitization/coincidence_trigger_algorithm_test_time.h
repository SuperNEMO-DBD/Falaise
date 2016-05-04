// snemo/digitization/coincidence_trigger_algorithm_test_time.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Francois MAUGER <mauger@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_TEST_TIME_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_TEST_TIME_H

// Standard library :
#include <string>
#include <bitset>
#include <utility>

// Third part : 
// Boost :
#include <boost/scoped_ptr.hpp>

// This project :
#include <snemo/digitization/calo_ctw_data.h>
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/calo_trigger_algorithm_test_time.h>
#include <snemo/digitization/tracker_trigger_algorithm_test_time.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/trigger_display_manager.h>

namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {
		
		class coincidence_trigger_algorithm_test_time
		{
		
		public :
						
			/// Trigger display manager is a friend because it can access to members for display
			friend class trigger_display_manager;

			struct coincidence_base_record
			{
				coincidence_base_record();
				void reset();
				const void display() const;			
				std::bitset<trigger_info::NZONES> calo_zoning_word[mapping::NUMBER_OF_SIDES];
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_0;
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_1;
				bool LTO_side_0;
				bool LTO_side_1;
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_gveto;
				bool LTO_gveto;
				std::bitset<calo_trigger_algorithm_test_time::XT_INFO_BITSET_SIZE> xt_info_bitset;
				bool single_side_coinc;
				bool total_multiplicity_threshold;
				bool decision;
			};
			
			struct coincidence_calo_record : public coincidence_base_record
			{
				coincidence_calo_record();
				void reset();
				const void display() const;
				uint32_t clocktick_1600ns;
			};
			
			enum trigger_mode{
				INVALID = 0,
				CARACO  = 1,
				APE     = 2,
				DAVE    = 3					
			};

			struct coincidence_event_record : public coincidence_base_record
			{
				coincidence_event_record();
				void reset();
				const void display() const;
				uint32_t clocktick_1600ns;
				std::bitset<trigger_info::NZONES> zoning_word[mapping::NUMBER_OF_SIDES];
				std::bitset<trigger_info::DATA_FULL_BITSET_SIZE> tracker_finale_data_per_zone[trigger_info::NSIDES][trigger_info::NZONES];
				coincidence_trigger_algorithm_test_time::trigger_mode trigger_mode;
			};
			
			struct L2_coincidence_decision
			{
				L2_coincidence_decision();
				void reset();
				const void display() const;
				bool L2_coincidence_decision_bool;
				uint32_t L2_clocktick_decision;
				coincidence_trigger_algorithm_test_time::trigger_mode trigger_mode;				
			};
			
			/// Previous event structure who pass the caraco trigger, store trigger informations (total = 173 [bits]). Useful to search delayed alpha pattern.
			struct previous_event_record : public coincidence_base_record
			{
				previous_event_record();
				void reset();
				const void display() const;
				uint32_t previous_clocktick_1600ns;
				uint32_t counter_1600ns;
				std::bitset<trigger_info::NZONES> zoning_word[mapping::NUMBER_OF_SIDES];
				std::bitset<trigger_info::DATA_FULL_BITSET_SIZE> tracker_finale_data_per_zone[trigger_info::NSIDES][trigger_info::NZONES];
			};

			static const uint32_t SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS = 5;
			static const uint32_t SIZE_OF_L2_COINCIDENCE_DECISION_GATE = 5;

			/// Default constructor
			coincidence_trigger_algorithm_test_time();

			/// Destructor
			virtual ~coincidence_trigger_algorithm_test_time();

			/// Set the electronic mapping object
			void set_electronic_mapping(const electronic_mapping & my_electronic_mapping_);

			/// Check if calorimeter gate size is set
			bool has_calorimeter_gate_size() const;

			/// Set calorimeter gate size
			void set_calorimeter_gate_size(unsigned int calorimeter_gate_size_);

			/// Check if previous event circular buffer depth is set
			bool has_previous_event_buffer_depth() const;

			/// Set previous event circular buffer depth
			void set_previous_event_buffer_depth(unsigned int circular_buffer_depth_);

			/// Return previous event circular buffer depth value
			const	unsigned int get_previous_event_buffer_depth() const;

			/// Initializing
			void initialize_simple();

			/// Initializing
			void initialize(const datatools::properties & config_);

			/// Check if the algorithm is initialized 
			bool is_initialized() const;

			/// Reset the object
			void reset();
			
			/// Reset internal datas
			void reset_data();

			/// Get the vector of coincidence record
			const std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> get_coincidence_calo_records_vector() const;

			/// Get the finale caraco trigger decision
 			const bool get_caraco_decision() const;
			
			/// Get the finale delayed (ape or dave) trigger decision
 			const bool get_delayed_coincidence_decision() const;

			/// General process
			void process(const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> & calo_records_,
									 const std::vector<tracker_trigger_algorithm_test_time::tracker_record> & tracker_records_,
									 std::vector<coincidence_trigger_algorithm_test_time::coincidence_event_record> & coincidence_records_);
			
		protected :
			
			/// Preparing coincidence calo records structure
			void _preparing_calo_coincidence(const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> & calo_records_);

			/// Update a coinc calo record 1600 from a calo summary record 25 ns
			void _update_coinc_calo_record(const calo_trigger_algorithm_test_time::calo_summary_record & a_calo_summary_record_25ns, 
																		 coincidence_trigger_algorithm_test_time::coincidence_calo_record & a_coinc_calo_record_1600ns);			
				
			/// Compute clocktick 1600ns for calo records
			void _compute_clocktick_1600ns(const uint32_t clocktick_25ns_, uint32_t & clocktick_1600ns_);
			
			/// Creating a per of calo record and tracker record for each clocktick
			void _creating_pair_per_clocktick(const std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> & coinc_calo_records_,
																				const std::vector<tracker_trigger_algorithm_test_time::tracker_record> & tracker_records_);
			
			/// CAlo tRAcker COincidence (CARACO) process for spatial coincidence between calorimeter and tracker each 1600ns
			void _process_calo_tracker_coincidence(const std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> a_pair_for_a_clocktick_,
																						 coincidence_trigger_algorithm_test_time::coincidence_event_record & a_coincidence_record_);
			
			/// Build the previous event record based on CARACO process for searching delayed alpha coincidences 
			void _build_previous_event_record(coincidence_trigger_algorithm_test_time::coincidence_event_record & a_coincidence_record_);

			/// Alpha delayed Pattern Event (APE) process for delayed
			void _process_delayed_coincidence(const std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> a_pair_for_a_clocktick_,
																				const coincidence_trigger_algorithm_test_time::previous_event_record & a_previous_event_record_,
																				coincidence_trigger_algorithm_test_time::coincidence_event_record & a_delayed_record_);
			
			/// Process calo records and tracker records
			void _process(const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> & calo_records_,
										const std::vector<tracker_trigger_algorithm_test_time::tracker_record> & tracker_records_,
										std::vector<coincidence_trigger_algorithm_test_time::coincidence_event_record> & coincidence_records_);
			
		private :

			typedef boost::circular_buffer<coincidence_trigger_algorithm_test_time::previous_event_record> buffer_previous_event_record_type;

      // Configuration :
      bool _initialized_; //!< Initialization flag
      const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID
			unsigned int _coincidence_calorimeter_gate_size_; //!< Size of calorimeter gate for extension of calo records during X CT 1600ns
			unsigned int _previous_event_circular_buffer_depth_; //!< Depth for the previous events circular buffer (Pile of PERs)
			std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> _coincidence_calo_records_; //!< Collection of coincidence calo records
			std::vector<std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> > _pair_records_; //!< Pair of a coincidence calo record and a tracker record (at the same clocktick 1600 ns)
			
			// Circular buffer for PER : (size of 5 or 10 ????)
			boost::scoped_ptr<buffer_previous_event_record_type> _previous_event_records_; //!< Previous event record 
			bool _caraco_decision_; //!< Decision for caraco trigger algorihtm
			bool _delayed_coincidence_decision_; //!< Decision for delayed (APE or DAVE) trigger algorithm
			std::vector<coincidence_trigger_algorithm_test_time::L2_coincidence_decision> _L2_coincidence_decison_records_; //!< Vector of L2 decision for an event (it is possible to trig several times for the same "SD" event
		};
			
	} // end of namespace digitization
	
} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_TEST_TIME_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
