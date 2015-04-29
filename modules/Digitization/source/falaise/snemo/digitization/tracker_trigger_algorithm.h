// snemo/digitization/tracker_trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_H

// Standard library :
#include <string>

// This project :
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/calo_ctw_data.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Trigger algorithm general process
    class tracker_trigger_algorithm
    {
		
			enum zone_id_limits{
				GEIGER_ZONE_ID_0_BEGIN = 0,
				GEIGER_ZONE_ID_0_END   = 8,
				GEIGER_ZONE_ID_1_BEGIN = 9,
				GEIGER_ZONE_ID_1_END   = 20,
				GEIGER_ZONE_ID_2_BEGIN = 21,
				GEIGER_ZONE_ID_2_END   = 32,
				GEIGER_ZONE_ID_3_BEGIN = 33,
				GEIGER_ZONE_ID_3_END   = 44,
				GEIGER_ZONE_ID_4_BEGIN = 45,
				GEIGER_ZONE_ID_4_END   = 56,
				GEIGER_ZONE_ID_5_BEGIN = 57,
				GEIGER_ZONE_ID_5_END   = 67,
				GEIGER_ZONE_ID_6_BEGIN = 68,
				GEIGER_ZONE_ID_6_END   = 79,
				GEIGER_ZONE_ID_7_BEGIN = 80,
				GEIGER_ZONE_ID_7_END   = 91,
				GEIGER_ZONE_ID_8_BEGIN = 92,
				GEIGER_ZONE_ID_8_END   = 103,
				GEIGER_ZONE_ID_9_BEGIN = 104,
				GEIGER_ZONE_ID_9_END   = 112
			};

			enum zone_id_size {
				GEIGER_HORIZONTAL_SIDE_SIZE = 5,
				GEIGER_HORIZONTAL_SIZE      = 6,
				GEIGER_VERTICAL_SIZE        = 5,
				GEIGER_ZONE_ID_0_SIZE       = 9,
				GEIGER_ZONE_ID_1_SIZE       = 12,
				GEIGER_ZONE_ID_2_SIZE       = 12,
				GEIGER_ZONE_ID_3_SIZE       = 12,
				GEIGER_ZONE_ID_4_SIZE       = 12,
				GEIGER_ZONE_ID_5_SIZE       = 11,
				GEIGER_ZONE_ID_6_SIZE       = 12,
				GEIGER_ZONE_ID_7_SIZE       = 12,
				GEIGER_ZONE_ID_8_SIZE       = 12,
				GEIGER_ZONE_ID_9_SIZE       = 9
			};
			
			enum zone_id_index {
				GEIGER_ZONE_0_INDEX = 0,
				GEIGER_ZONE_1_INDEX = 1,
				GEIGER_ZONE_2_INDEX = 2,
				GEIGER_ZONE_3_INDEX = 3,
				GEIGER_ZONE_4_INDEX = 4,
				GEIGER_ZONE_5_INDEX = 5,
				GEIGER_ZONE_6_INDEX = 6,
				GEIGER_ZONE_7_INDEX = 7,
				GEIGER_ZONE_8_INDEX = 8,
				GEIGER_ZONE_9_INDEX = 9
			};

			enum subzone_size {
				GEIGER_SUB_ZONE_HORIZONTAL_SIZE      = 6,
				GEIGER_SUB_ZONE_HORIZONTAL_SIDE_SIZE = 5,
				GEIGER_SUB_ZONE_VERTICAL_SIZE        = 5		
			};

			enum subzone_index {
				GEIGER_SUB_ZONE_0_INDEX = 0,
				GEIGER_SUB_ZONE_1_INDEX = 1,
				GEIGER_SUB_ZONE_2_INDEX = 2,
				GEIGER_SUB_ZONE_3_INDEX = 3
			};

    public : 

      /// Default constructor
			tracker_trigger_algorithm();

      /// Destructor
      virtual ~tracker_trigger_algorithm();

			/// Initializing
      void initialize(const electronic_mapping & my_electronic_mapping_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

			/// Return the board id from the bitset of 100 bits
			uint32_t get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const;

			/// Convert the electronic ID of active geiger cells into geometric ID
			void build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_, std::vector<geomtools::geom_id> & hit_cells_gids_) const;

			/// Fill the geiger cells matrix
			void fill_matrix(const std::vector<geomtools::geom_id> & hit_cells_gids_);

			/// ASCII display for the geiger cells matrix
			void display_matrix() const;
			
			/// Reset the geiger cells matrix
			void reset_matrix();
			
			/// Build the sub-zone of a more generic zone
			void sub_zone_builder(int32_t side_, int32_t sub_zone_index_);

      /// General process
      void process(const geiger_ctw_data & geiger_ctw_data_);
	
    private :
      
      bool _initialized_; //!< Initialization flag
      bool _geiger_matrix_[mapping::MAX_NUMBER_OF_SIDE][mapping::GEIGER_LAYER_SIZE][mapping::GEIGER_ROW_SIZE]; //!< Geiger cells matrix
			const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID
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
