// snemo/digitization/tracker_trigger_algorithm_test_new_strategy.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_TEST_NEW_STRATEGY_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_TEST_NEW_STRATEGY_H

// Standard library :
#include <string>
#include <bitset>

// This project :
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/tracker_trigger_mem_maker_new_strategy.h>
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/tracker_info.h>
#include <snemo/digitization/tracker_zone.h>
#include <snemo/digitization/tracker_sliding_zone.h>

namespace datatools {
	class properties;
}

namespace snemo {
  
  namespace digitization {		

    /// \brief Trigger algorithm general process
    class tracker_trigger_algorithm_test_new_strategy
    {
		public : 
			/// Trigger display manager is a friend because it can access to members for display
		 	//friend class trigger_display_manager;

			struct tracker_record
			{
				tracker_record();
				void reset();
				void display();
				uint32_t clocktick_1600ns;
				//std::bitset<FINALE_DECISION_BITSET_SIZE> finale_decision;
				//std::bitset<FINAL_ZONE_BITSET_SIZE> final_tracker_trigger_info[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES];
			};

			struct geiger_matrix
			{
				geiger_matrix();
				bool is_empty();
				uint32_t clocktick_1600ns;				
				bool matrix[tracker_info::NSIDES][tracker_info::NLAYERS][tracker_info::NROWS];
			};

			/// Default constructor
			tracker_trigger_algorithm_test_new_strategy();

      /// Destructor
      virtual ~tracker_trigger_algorithm_test_new_strategy();

			/// Set the electronic mapping object
      void set_electronic_mapping(const electronic_mapping & my_electronic_mapping_);

			/// Fill memory 1
			void fill_mem1_all(const std::string & filename_);

			/// Fill memory 2
			void fill_mem2_all(const std::string & filename_);

			/// Fill memory 3
			void fill_mem3_all(const std::string & filename_);

			/// Fill memory 4
			void fill_mem4_all(const std::string & filename_);

			/// Fill memory 5
			void fill_mem5_all(const std::string & filename_);

			/// Initializing
      void initialize();

			/// Initializing
      void initialize(const datatools::properties & config_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

			/// Return the board id from the bitset of 100 bits
			uint32_t get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const;
			



			/// Convert the electronic ID of active geiger cells into geometric ID
			void build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_,
																				 std::vector<geomtools::geom_id> & hit_cells_gids_) const;

			/// Return the tracker decision
			const	bool get_tracker_decision() const;

			/// Fill the geiger cells matrix
			void fill_matrix(const std::vector<geomtools::geom_id> & hit_cells_gids_);

			/// ASCII display for the geiger cells matrix
			void display_matrix() const;
			
			/// Reset the geiger cells matrix
			void reset_matrix();

			/// General process
      void process(const geiger_ctw_data & geiger_ctw_data_,
									 std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_);

		protected :

			/// Process for a clocktick
			void _process_for_a_clocktick(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_);

			/// Protected general process
			void _process(const geiger_ctw_data & geiger_ctw_data_,
										std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_);

		private :
			
      // Configuration : 
      bool _initialized_; //!< Initialization
			const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID flag

			tracker_trigger_mem_maker_new_strategy::mem1_type _sliding_zone_vertical_memory_;
			tracker_trigger_mem_maker_new_strategy::mem2_type _sliding_zone_horizontal_memory_;
			tracker_trigger_mem_maker_new_strategy::mem3_type _zone_vertical_memory_;
			tracker_trigger_mem_maker_new_strategy::mem4_type _zone_horizontal_memory_;
			tracker_trigger_mem_maker_new_strategy::mem5_type _zone_vertical_for_horizontal_memory_;

			// Data :
			bool _geiger_matrix_[tracker_info::NSIDES][tracker_info::NLAYERS][tracker_info::NROWS];
			tracker_zone _zones_[tracker_info::NSIDES][tracker_info::NZONES];
			tracker_sliding_zone _sliding_zones_[tracker_info::NSIDES][tracker_info::NSLZONES];

			tracker_record _tracker_record_finale_decision_; //!< Tracker record structure for a clocktick
			bool _tracker_finale_decision_; //!< Finale tracker decision boolean
		};

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_TEST_NEW_STRATEGY_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
