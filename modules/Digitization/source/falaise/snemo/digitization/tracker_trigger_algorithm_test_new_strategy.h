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
#include <snemo/digitization/tracker_trigger_mem_maker.h>
#include <snemo/digitization/trigger_display_manager.h>

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
		 	friend class trigger_display_manager;

			/// Number of layers near source can be activated for vertex localization
			static const int32_t NUMBER_OF_NEAR_SOURCE_LAYERS = 3;

			/// Size of the subzone layer projection bitset
			static const int32_t SUBZONE_PROJECTION_LAYER_SIZE = 5;

			/// Size of the subzone row projection bitset
			static const int32_t SUBZONE_PROJECTION_ROW_SIZE = 6;

			/// Size of the bitset for one zone after projections on subzones and near source bits
			static const int32_t TOTAL_ZONE_AFTER_PROJECTIONS_BITSET_SIZE = 10;
			
			/// Size of the bitset for one interzone after projections on subzones only
			static const int32_t TOTAL_INTERZONE_AFTER_PROJECTIONS_BITSET_SIZE = 8;
			
			/// Final bitset size for a zone : 2 bits for near source localization and 2 bits for pattern
			static const int32_t FINAL_ZONE_BITSET_SIZE = 4;
			
			/// Final bitset size for an interzone : 2 bits for pattern only
			static const int32_t FINAL_INTERZONE_BITSET_SIZE = 2;

			/// Final bitset size for the finale decision in the full tracker
			static const int32_t FINALE_DECISION_BITSET_SIZE = 2;


			struct tracker_record
			{
				tracker_record();
				void reset();
				void display();
				uint32_t clocktick_1600ns;
				std::bitset<FINALE_DECISION_BITSET_SIZE> finale_decision;
				std::bitset<FINAL_ZONE_BITSET_SIZE> final_tracker_trigger_info[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES];
			};

			struct geiger_matrix
			{
				geiger_matrix();
				bool is_empty();
				uint32_t clocktick_1600ns;				
				bool matrix[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_LAYERS][mapping::NUMBER_OF_GEIGER_ROWS];
			};
			
			struct sub_zone_location_info {
				int32_t row_begin;
				int32_t row_end;
				int32_t layer_begin;		
			};

			enum tracker_zoning_row_id_limits {
				ZONE_0_BEGIN = 0,
				ZONE_0_END   = 8,
				ZONE_1_BEGIN = 9,
				ZONE_1_END   = 20,
			  ZONE_2_BEGIN = 21,
			  ZONE_2_END   = 32,
			  ZONE_3_BEGIN = 33,
			  ZONE_3_END   = 44,
			  ZONE_4_BEGIN = 45,
			  ZONE_4_END   = 56,
			  ZONE_5_BEGIN = 57,
				ZONE_5_END   = 67,
				ZONE_6_BEGIN = 68,
				ZONE_6_END   = 79,
				ZONE_7_BEGIN = 80,
				ZONE_7_END   = 91,
				ZONE_8_BEGIN = 92,
			  ZONE_8_END   = 103,
				ZONE_9_BEGIN = 104,
			  ZONE_9_END   = 112
			};

			enum tracker_zoning_id_size {
				HORIZONTAL_SIDED_ZONE_SIZE  = 5,
				HORIZONTAL_SIZE             = 6,
				VERTICAL_SIZE               = 5,
			  ZONE_ID_0_SIZE              = 9,
			  ZONE_ID_1_SIZE              = 12,
			  ZONE_ID_2_SIZE              = 12,
			  ZONE_ID_3_SIZE              = 12,
			  ZONE_ID_4_SIZE              = 12,
				ZONE_ID_5_SIZE              = 11,
				ZONE_ID_6_SIZE              = 12,
			  ZONE_ID_7_SIZE              = 12,
			  ZONE_ID_8_SIZE              = 12,
				ZONE_ID_9_SIZE              = 9
			};
			
			enum tracker_zoning_id_index {
				ZONE_0_INDEX = 0,
				ZONE_1_INDEX = 1,
			  ZONE_2_INDEX = 2,
				ZONE_3_INDEX = 3,
				ZONE_4_INDEX = 4,
				ZONE_5_INDEX = 5,
				ZONE_6_INDEX = 6,
				ZONE_7_INDEX = 7,
				ZONE_8_INDEX = 8,
				ZONE_9_INDEX = 9
			};

			enum tracker_subzone_index {
			  SUBZONE_0_INDEX = 0,
			  SUBZONE_1_INDEX = 1,
			  SUBZONE_2_INDEX = 2,
				SUBZONE_3_INDEX = 3
			};

			enum zone_after_projections_bitset_index {
				SUBZONE_0_LAYER_INDEX   = 0,
				SUBZONE_0_ROW_INDEX     = 1,
				SUBZONE_1_LAYER_INDEX   = 2,
				SUBZONE_1_ROW_INDEX     = 3,
				SUBZONE_2_LAYER_INDEX   = 4,
				SUBZONE_2_ROW_INDEX     = 5,
				SUBZONE_3_LAYER_INDEX   = 6,
				SUBZONE_3_ROW_INDEX     = 7,
			  RIGHT_NEAR_SOURCE_AP_INDEX = 8,
				LEFT_NEAR_SOURCE_AP_INDEX  = 9
			};			
			
			enum final_zone_bitset_index {
				PATTERN_BIT_0_FINAL_INDEX     = 0,
				PATTERN_BIT_1_FINAL_INDEX     = 1,
			  RIGHT_NEAR_SOURCE_FINAL_INDEX = 2,
				LEFT_NEAR_SOURCE_FINAL_INDEX  = 3
			};

      /// Default constructor
			tracker_trigger_algorithm_test_new_strategy();

      /// Destructor
      virtual ~tracker_trigger_algorithm_test_new_strategy();

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

			/// Reset private tables
			void reset_trigger_info();

			/// Get the tracker record finale decision structure
			const tracker_trigger_algorithm_test_new_strategy::tracker_record get_tracker_record_finale_decision_structure() const;

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
			
			/// Display the intermediate tracker trigger info
		  void display_intermediate_tracker_trigger_info() const;

			/// Display the finale tracker trigger info (bits per zone and finale decision)
			void display_finale_tracker_trigger_info() const;

			/// Fill an A6D1 level 0 to level 1 memory for a given subzone
			void fill_row_memory_per_subzone(const std::string & filename_,
																			 int32_t side_,
																			 int32_t zone_,
																			 int32_t subzone_);

			/// Fill an A6D1 level 0 to level 1 memory for all subzones
			void fill_all_row_memory(const std::string & filename_);

			/// Fill an A5D1 level 0 to level 1 memory for a given subzone
			void fill_layer_memory_per_subzone(const std::string & filename_,
																				 int32_t side_,
																				 int32_t zone_,
																				 int32_t subzone_);

			/// Fill an A6D1 level 0 to level 1 memory for all subzones
			void fill_all_layer_memory(const std::string & filename_);

			/// Fill an A4D2 level 1 to level 2 memory for a given zone
			void fill_a4_d2_memory_per_zone(const std::string & filename_,
																			int32_t side_,
																			int32_t zone_);

			/// Fill an A4D2 level 1 to level 2 memory for all zones
			void fill_all_a4_d2_memory(const std::string & filename_);

			/// For a given side and zone index, give the row index begin and end
			void fetch_zone_limits(int32_t side_,
														 int32_t zone_index_,
														 int32_t & row_index_begin_,
														 int32_t & row_index_end_);

			/// For a given side and row index, give the zone index
			void fetch_zone_index(int32_t side_,
														int32_t row_index_,
														int32_t & zone_index_);

			/// For a given subzone indx, give the subzone row begin, row end and layer begin
			void fetch_subzone_limits(int32_t side_,
																int32_t subzone_index_,
																int32_t & subzone_row_index_begin_,
																int32_t & subzone_row_index_end_,
																int32_t & subzone_layer_index_begin_);

			/// Build the level one tracker trigger primitive bitsets
			void build_zone_tracker_trigger_after_projections(); 

			/// Build the level two tracker trigger primitive bitsets
			void build_zone_tracker_trigger_info();

			/// Build the level 1 finale structure for the tracker
			void build_tracker_record();

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
			sub_zone_location_info _sub_zone_location_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES_PER_SIDE]; //!< Table of 2x40 (10 zones subdivided in 4 subzones on 2 sides)

			memory<6,1> _mem_projection_row_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES][mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES]; //!< A6D1 memory for projection in rows depending of algorithm uses (multiplicity or gap)
			memory<5,1> _mem_projection_layer_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES][mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES]; //!< A6D1 memory for projection in layers depending of algorithm uses (multiplicity or gap)
			memory<4,2> _mem_pattern_for_a_zone_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES]; //!< A4D2 memory pattern
			
			// Data :
			// Maybe use boost::multiarray instead of C-type array
      bool _geiger_matrix_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_LAYERS][mapping::NUMBER_OF_GEIGER_ROWS]; //!< Geiger cells matrix
			std::vector<geiger_matrix> _geiger_matrix_records_; //!< Vector of Geiger matrix for each clocktick
			
			std::bitset<TOTAL_ZONE_AFTER_PROJECTIONS_BITSET_SIZE> _zone_tracker_trigger_info_after_projections_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES]; //!< Table of 2x10 containing 10 bits bitset representing the state of all zones after projections on layers and rows

			std::bitset<TOTAL_INTERZONE_AFTER_PROJECTIONS_BITSET_SIZE> _interzone_tracker_trigger_info_after_projections_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES];  //!< Table of 2x9 containing 8 bits bitset representing the state of all interzones after projections on layers and rows

			std::bitset<FINAL_ZONE_BITSET_SIZE> _zone_tracker_trigger_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES]; //!< Table of 2x10 containing 4 bits bitset representing if there is a track / pretrack or nothing in one zone	and the location if a cells in 3 first layers is activated
			
			std::bitset<FINAL_INTERZONE_BITSET_SIZE> _interzone_tracker_trigger_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES]; //!< Table of 2x9 containing 2 bits bitset representing if there is a track / pretrack or nothing in one interzone
			
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
