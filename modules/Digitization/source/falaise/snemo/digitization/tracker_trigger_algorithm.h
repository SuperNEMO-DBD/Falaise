// snemo/digitization/tracker_trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H

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
    class tracker_trigger_algorithm
    {				
    public : 
			
			/// Trigger display manager is a friend because it can access to members for display
		 	friend class trigger_display_manager;

			/// Level one zoning size of a bitset for a zone
			static const int32_t GEIGER_LEVEL_ONE_ZONING_BITSET_SIZE = 8;
			
			/// Level two zoning size of a bitset for a zone
			static const int32_t GEIGER_LEVEL_TWO_ZONING_BITSET_SIZE = 2;
			
			/// Final bitset size for geiger zoning
			static const int32_t GEIGER_ZONING_FINAL_BITSET_SIZE = 2;
	
			/// Final bitset size for geiger zoning
			static const int32_t GEIGER_FINAL_DECISION_BITSET_SIZE = 2;

			/// Size of the subzone layer projection bitset
			static const int32_t GEIGER_LEVEL_ONE_SUBZONE_LAYER_SIZE = 5;

			/// Size of the subzone row projection bitset
			static const int32_t GEIGER_LEVEL_ONE_SUBZONE_ROW_SIZE   = 6;

			struct tracker_record
			{
				tracker_record();
				void reset();
				void display();
				uint32_t clocktick_1600ns;
				std::bitset<GEIGER_FINAL_DECISION_BITSET_SIZE> level_one_finale_decision;
				std::bitset<GEIGER_ZONING_FINAL_BITSET_SIZE> final_tracker_trigger_info[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES];
				bool geiger_matrix[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_LAYERS][mapping::NUMBER_OF_GEIGER_ROWS];
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

			enum _tracker_trigger_info_bitset_index {
				SUBZONE_0_LAYER_INDEX = 0,
				SUBZONE_0_ROW_INDEX   = 1,
				SUBZONE_1_LAYER_INDEX = 2,
				SUBZONE_1_ROW_INDEX   = 3,
				SUBZONE_2_LAYER_INDEX = 4,
				SUBZONE_2_ROW_INDEX   = 5,
				SUBZONE_3_LAYER_INDEX = 6,
				SUBZONE_3_ROW_INDEX   = 7	
			};

      /// Default constructor
			tracker_trigger_algorithm();

      /// Destructor
      virtual ~tracker_trigger_algorithm();

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

			/// Get the level 1 finale decision structure
			const tracker_trigger_algorithm::tracker_record get_tracker_level_1_finale_decision_structure() const;

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
			
			/// Display the intermediate level one and level two tracker trigger info (bitsets)
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
			void build_trigger_level_one_bitsets(); 

			/// Build the level two tracker trigger primitive bitsets
			void build_trigger_level_one_to_level_two();

			/// Build the level 1 finale structure for the tracker
			void build_tracker_record();

      /// General process
      void process(const geiger_ctw_data & geiger_ctw_data_,
									 std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_);
			
		protected :
			
			/// Process for a clocktick
			void _process_for_a_clocktick(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_);

			/// Protected general process
			void _process(const geiger_ctw_data & geiger_ctw_data_,
										std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_);
			
    private :
			
      // Configuration : 
      bool _initialized_; //!< Initialization
			const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID flag

			// Data :
      bool _geiger_matrix_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_LAYERS][mapping::NUMBER_OF_GEIGER_ROWS]; //!< Geiger cells matrix

			std::bitset<GEIGER_LEVEL_ONE_ZONING_BITSET_SIZE> _level_one_tracker_trigger_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES]; //!< Table of 2x10 containing 8 bits bitset representing the level one tracker trigger zoning (side = {0-1}, zones = {0-9})			
			std::bitset<GEIGER_LEVEL_ONE_ZONING_BITSET_SIZE> _level_one_prime_tracker_trigger_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES]; //!< Table of 2x10 containing 8 bits bitset representing the level one tracker trigger for the interzones	 
			std::bitset<GEIGER_LEVEL_TWO_ZONING_BITSET_SIZE> _level_two_tracker_trigger_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES]; //!< Table of 2x10 containing 2 bits bitset representing if there is a track / pretrack or nothing in one zone	
			std::bitset<GEIGER_LEVEL_TWO_ZONING_BITSET_SIZE> _level_two_prime_tracker_trigger_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_INTERZONES]; //!< Table of 2x9 containing 2 bits bitset representing if there is a track / pretrack or nothing in one interzone

			sub_zone_location_info _sub_zone_location_info_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES_PER_SIDE]; //!< Table of 2x40 (10 zones subdivided in 4 subzones on 2 sides)
			
			memory<6,1> _mem_lvl0_to_lvl1_row_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES][mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES]; //!< A6D1 memory for level 0 to level 1 row projection depending of algorithm uses (multiplicity or gap)
			memory<5,1> _mem_lvl0_to_lvl1_layer_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES][mapping::NUMBER_OF_TRACKER_TRIGGER_SUBZONES]; //!< A6D1 memory for level 0 to level 1 layer projection depending of algorithm uses (multiplicity or gap)
			memory<4,2> _mem_lvl1_to_lvl2_[mapping::NUMBER_OF_SIDES][mapping::NUMBER_OF_TRIGGER_ZONES]; //!< A4D2 memory for level 1 to level 2 (4 bits --> 2 bits)
			
			tracker_record _tracker_level_1_finale_decision_; //!< Structure representing the finale decision for level 1 tracker
			bool _tracker_finale_decision_; //!< Finale tracker decision
		};

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
