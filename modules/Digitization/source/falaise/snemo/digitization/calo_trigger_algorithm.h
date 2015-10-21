// snemo/digitization/calo_trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TRIGGER_ALGORITHM_H

// Standard library :
#include <string>
#include <bitset>

// Boost :
#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>

// This project :
#include <snemo/digitization/calo_ctw_data.h>
#include <snemo/digitization/calo_ctw_constants.h>
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/mapping.h>

namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {

    /// \brief Calorimeter algorithm general process
    class calo_trigger_algorithm
    {
    public : 

			/// Level one zoning size of a bitset for a zone
			static const int32_t LEVEL_ONE_MULT_BITSET_SIZE = 2;

			/// Size of the zoning bitset for a side of the calorimeter
			static const int32_t ZONING_PER_SIDE_BITSET_SIZE = 10;
			
			/// Size of the zoning bitset for gamma veto
			static const int32_t ZONING_GVETO_BITSET_SIZE = 4;

			/// Size of the information bitset containing XT bit and spare bits (up to 4)
			static const int32_t XT_INFO_BITSET_SIZE = 3;

			struct trigger_record
			{
				uint32_t clocktick_25ns;
				std::bitset<ZONING_PER_SIDE_BITSET_SIZE> zoning_word[mapping::NUMBER_OF_SIDES];
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_0;
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_1;
				bool LTO_side_0;
				bool LTO_side_1;
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_gveto;
				bool LTO_gveto;
				std::bitset<XT_INFO_BITSET_SIZE> xt_info_bitset;
			};
			
			struct trigger_summary_record : public trigger_record
			{
				bool single_side_coinc;
				bool threshold_total_multiplicity;
				bool trigger_finale_decision;
			};
			
			enum side_id_index {
				SIDE_0_INDEX = 0,
				SIDE_1_INDEX = 1
			};
			
			enum zoning_id_index {
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

			enum zoning_column_id_limits{
				ZONE_0_BEGIN = 0,
				ZONE_0_END   = 1,
				ZONE_1_BEGIN = 2,
				ZONE_1_END   = 3,
			  ZONE_2_BEGIN = 4,
			  ZONE_2_END   = 5,
			  ZONE_3_BEGIN = 6,
			  ZONE_3_END   = 7,
			  ZONE_4_BEGIN = 8,
			  ZONE_4_END   = 9,
			  ZONE_5_BEGIN = 10,
				ZONE_5_END   = 11,
				ZONE_6_BEGIN = 12,
				ZONE_6_END   = 13,
				ZONE_7_BEGIN = 14,
				ZONE_7_END   = 15,
				ZONE_8_BEGIN = 16,
			  ZONE_8_END   = 17,
				ZONE_9_BEGIN = 18,
			  ZONE_9_END   = 19
			};

			enum info_bitset_pos{
				XT_INFO_BIT        = 0,
				CONTROL_INFO_BIT_0 = 1,
				CONTROL_INFO_BIT_1 = 2,
				CONTROL_INFO_BIT_2 = 3,
				CONTROL_INFO_BIT_3 = 4
			};

      /// Default constructor
      calo_trigger_algorithm();

      /// Destructor
      virtual ~calo_trigger_algorithm();

      /// Set the electronic mapping object
      void set_electronic_mapping(const electronic_mapping & my_electronic_mapping_);

			/// Set the calo circular buffer depth
			void set_circular_buffer_depth(unsigned int & circular_buffer_depth_);

			/// Set the boolean for inhibited back to back coincidence
			void inhibit_both_side_coinc();

			/// Check if back to back coinc is set 
			bool is_inhibited_both_side_coinc() const;

			/// Set the boolean for inhibited single side coincidence
			void inhibit_single_side_coinc();

			/// Check if single side coinc is set 
			bool is_inhibited_single_side_coinc() const;
			
			/// Set the threshold of multiplicity for coincidences
			void set_threshold_total_multiplicity(unsigned int & threshold_);

			/// Check if total multiplicity threshold is set
			bool is_activated_threshold_total_multiplicity() const;

			/// Get the calo threshold for coincidences
			const	std::bitset<calo::ctw::HTM_BITSET_SIZE> get_threshold_total_multiplicity_coinc() const;
         													 																	
      /// Initializing
      void initialize_simple();

      /// Initializing
      void initialize(const datatools::properties & config_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

      /// Reset private tables
      void reset_trigger_info();

			/// Get the level 1 finale decision structure
			const trigger_summary_record get_calo_level_1_finale_decision() const;
    
			/// General process
      void process(const calo_ctw_data & calo_ctw_data_);

		protected :
		 
			/// Build the trigger record structure for a clocktick
			void _build_trigger_record_per_clocktick(const calo_ctw & my_calo_ctw_);

			/// Build intermediate working data structure
			void _build_trigger_record_structure();   

			/// Build summary calo trigger structure
			void _build_trigger_record_summary_structure(trigger_summary_record & my_trigger_summary_record_);

			/// Compute the trigger finale decision
			void _compute_trigger_finale_decision(trigger_summary_record & my_trigger_summary_record_);

			/// Protected general process
			void _process(const calo_ctw_data & calo_ctw_data_);

			/// Display the level one calo trigger info (bitsets)
			void _display_trigger_info_for_a_clocktick();
	
			/// Display the level one calo trigger info and internal working data (bitsets)
			void _display_trigger_summary(trigger_summary_record & my_trigger_summary_record_);

    private :

			typedef boost::circular_buffer<trigger_record> buffer_type;

      // Configuration :
      bool _initialized_; //!< Initialization flag
      const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID
			bool _inhibit_both_side_coinc_;
			bool _inhibit_single_side_coinc_;
			std::bitset<calo::ctw::HTM_BITSET_SIZE> _threshold_total_multiplicity_;
			bool _activated_threshold_;
			unsigned int _circular_buffer_depth_;

      // Data :	 

			trigger_record _trigger_record_per_clocktick_; //!< Trigger record structure for built for each clocktick

			boost::scoped_ptr<buffer_type> _gate_circular_buffer_; //!< Scoped pointer to a circular buffer containing output data structure
 
			trigger_summary_record _calo_level_1_finale_decision_; //!< Finale decision for level 1 calorimeter

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TRIGGER_ALGORITHM_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
