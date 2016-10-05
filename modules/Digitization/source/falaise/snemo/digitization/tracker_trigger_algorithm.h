// snemo/digitization/tracker_trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H

// - Bayeux/datatools :
#include <datatools/temporary_files.h>

// Standard library :
#include <string>
#include <bitset>
#include <iostream>

// This project :
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/tracker_trigger_mem_maker.h>
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/trigger_info.h>
#include <snemo/digitization/tracker_zone.h>
#include <snemo/digitization/tracker_sliding_zone.h>

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

			struct tracker_record
			{
				enum bit_index{
					FINALE_DATA_BIT_INNER   = 0,
					FINALE_DATA_BIT_OUTER   = 1,
					FINALE_DATA_BIT_RIGHT   = 2,
					FINALE_DATA_BIT_MIDDLE  = 3,
					FINALE_DATA_BIT_LEFT    = 4,
					FINALE_DATA_BIT_NSZ_RIGHT = 5,
					FINALE_DATA_BIT_NSZ_LEFT  = 6
				};
				
				tracker_record();
				void reset();
				void display(std::ostream & out_ = std::clog);
				int32_t clocktick_1600ns;
				std::bitset<trigger_info::DATA_FULL_BITSET_SIZE> finale_data_per_zone[trigger_info::NSIDES][trigger_info::NZONES];
				std::bitset<trigger_info::NZONES> zoning_word_pattern[trigger_info::NSIDES]; // not fill atm
				std::bitset<trigger_info::NZONES> zoning_word_near_source[trigger_info::NSIDES]; // not fill atm
				// ajout de choses (cf calo record -> symétrie)
				bool single_side_coinc; // not fill atm
				bool finale_decision;
			};

			struct geiger_matrix
			{
				geiger_matrix();
				bool is_empty();
				void display_matrix_garrido(datatools::temp_file & tmp_file_) const;
				int32_t clocktick_1600ns;				
				bool matrix[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS];
			};

			/// Default constructor
			tracker_trigger_algorithm();

      /// Destructor
      virtual ~tracker_trigger_algorithm();

			/// Set the electronic mapping object
      void set_electronic_mapping(const electronic_mapping & my_electronic_mapping_);

			/// Fill memory 1 for all zones
			void fill_mem1_all(const std::string & filename_);

			/// Fill memory 2 for all zones
			void fill_mem2_all(const std::string & filename_);

			/// Fill memory 3 for all zones
			void fill_mem3_all(const std::string & filename_);

			/// Fill memory 4 for all zones
			void fill_mem4_all(const std::string & filename_);

			/// Fill memory 5 for all zones
			void fill_mem5_all(const std::string & filename_);

			/// Initializing
      void initialize();

			/// Initializing
      void initialize(const datatools::properties & config_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

			/// Set a temporary file for display
			void set_tmp_file(std::string & path_, std::string & prefix_, bool & remove_at_destroy_);
			
			/// Return the temporary file non const
			datatools::temp_file & grab_tmp_file();

			/// Check if a temporary file is set
			bool is_temporary_file() const;

			/// Return the board id from the bitset of 100 bits
			uint32_t get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const;
			
			/// Convert the electronic ID of active geiger cells into geometric ID
			void build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_,
																				 std::vector<geomtools::geom_id> & hit_cells_gids_) const;

			/// Return the tracker decision
			bool get_tracker_decision() const;
			
			/// Display in an extern temporary file
			void display_in_file(std::string & display_mode_);

			/// Fill the geiger cells matrix
			void fill_matrix(const std::vector<geomtools::geom_id> & hit_cells_gids_);

			/// ASCII display for the geiger cells matrix
			void display_matrix() const;
			
			/// Reset the geiger cells matrix
			void reset_matrix();

			/// Reset bitsets in zone and sliding zone
			void reset_zones_informations();

			/// Build one sliding zone information for a clocktick
			void build_sliding_zone(tracker_sliding_zone & szone_, unsigned int side_, unsigned int szone_id_);

			/// Build all sliding zones with memories mem1 and mem2 for projections
			void build_sliding_zones(tracker_trigger_mem_maker::mem1_type & mem1_, 
															 tracker_trigger_mem_maker::mem2_type & mem2_);

			
			/// Build one zone information for a clocktick
			void build_zone(tracker_zone & zone_, unsigned int side_, unsigned int zone_id_);

			/// Build all zones responses for a clocktick
			void build_zones();    

			/// Build the vertical information for a zone
			void build_in_out_pattern(tracker_zone & zone_, 
																tracker_trigger_mem_maker::mem3_type & mem3_);

			/// Build the horizeontal information for a zone
			void build_left_mid_right_pattern(tracker_zone & zone_,
																				tracker_trigger_mem_maker::mem4_type & mem4_,
																				tracker_trigger_mem_maker::mem5_type & mem5_);

			/// Build near source information for a zone
			void build_near_source_pattern(tracker_zone & zone_);

			/// Build tracker record for each clocktick
			void build_tracker_record();

			/// Print all tracker with zones boundaries
			void print_zones(std::ostream & out_) const;
			
			/// Print information for one zone (results of sliding zones projections and computing)
			void print_zone_information(std::ostream & out_) const;

			/// Print all tracker sliding zones projections
			void print_sliding_zones(std::ostream & out_) const;

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
			
			// Display into a file :
			bool _is_temporary_file_;
			datatools::temp_file _display_tmp_file_;
			
			tracker_trigger_mem_maker::mem1_type _sliding_zone_vertical_memory_;
			tracker_trigger_mem_maker::mem2_type _sliding_zone_horizontal_memory_;
			tracker_trigger_mem_maker::mem3_type _zone_vertical_memory_;
			tracker_trigger_mem_maker::mem4_type _zone_horizontal_memory_;
			tracker_trigger_mem_maker::mem5_type _zone_vertical_for_horizontal_memory_;

			// Data :
			bool _geiger_matrix_[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS];
			std::vector<geiger_matrix> _geiger_matrix_records_; //!< Vector of Geiger matrix for each clocktick

			tracker_zone _zones_[trigger_info::NSIDES][trigger_info::NZONES];
			tracker_sliding_zone _sliding_zones_[trigger_info::NSIDES][trigger_info::NSLZONES];

			tracker_record _tracker_record_finale_decision_; //!< Tracker record structure for a clocktick
			bool _tracker_finale_decision_; //!< Finale tracker decision boolean
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
