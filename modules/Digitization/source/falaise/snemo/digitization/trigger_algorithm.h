// snemo/digitization/trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H

// Standard library :
#include <string>
#include <list>

// This project :
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/calo_ctw_data.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Trigger algorithm general process
    class trigger_algorithm
    {
    public : 

      /// Default constructor
      trigger_algorithm();

      /// Destructor
      virtual ~trigger_algorithm();

     /// Initializing
      void initialize(const electronic_mapping & my_electronic_mapping_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

			uint32_t get_board_id(const std::bitset<geiger::tp::FULL_SIZE> & my_bitset_) const;

			void build_hit_cells_gids_from_ctw(const geiger_ctw & my_geiger_ctw_, std::vector<geomtools::geom_id> & hit_cells_gids_) const;

      /// General process
      void process(const geiger_ctw_data & geiger_ctw_data_);
	
    private :
      
      bool _initialized_; //!< Initialization flag
      bool _geiger_matrix_[mapping::MAX_NUMBER_OF_SIDE][mapping::GEIGER_LAYER_SIZE][mapping::GEIGER_ROW_SIZE]; //!< Geiger cells matrix
			const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID
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
