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
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/trigger_info.h>

namespace datatools {
  class properties;
}

namespace snemo {
  
  namespace digitization {

    /// \brief Calorimeter algorithm general process
    class calo_trigger_algorithm
    {
		public :
			
			/// Trigger display manager is a friend because it can access to members for display
			friend class trigger_display_manager;
			
			/// Level one zoning size of a bitset for a zone
			static const int32_t LEVEL_ONE_MULT_BITSET_SIZE = 2;

			/// Size of the zoning bitset for a side of the calorimeter
			static const int32_t ZONING_PER_SIDE_BITSET_SIZE = 10;
			
			/// Size of the zoning bitset for gamma veto
			static const int32_t ZONING_GVETO_BITSET_SIZE = 4;

			/// Size of the information bitset containing XT bit and spare bits (up to 4)
			static const int32_t XT_INFO_BITSET_SIZE = 3;

			struct calo_record
			{
				calo_record();
				void reset();
			  void display();
				uint32_t clocktick_25ns;
				std::bitset<trigger_info::NZONES> zoning_word[trigger_info::NSIDES];
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_0;
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_1;
				bool LTO_side_0;
				bool LTO_side_1;
				std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_gveto;
				bool LTO_gveto;
				std::bitset<XT_INFO_BITSET_SIZE> xt_info_bitset;
			};
			
			struct calo_summary_record : public calo_record
			{
				calo_summary_record();
				void reset();
				void reset_summary_boolean_only();
				void display();
				bool single_side_coinc;
				bool total_multiplicity_threshold;
				bool calo_finale_decision;
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

			/// Check if circular buffer depth is set
			bool has_circular_buffer_depth() const;

			/// Set calo circular buffer depth
			void set_circular_buffer_depth(unsigned int circular_buffer_depth_);

			/// Return calo circular buffer depth value
			const	unsigned int get_circular_buffer_depth() const;

			/// Set the boolean for inhibited back to back coincidence
			void inhibit_both_side_coinc();

			/// Check if back to back coinc is set 
			bool is_inhibited_both_side_coinc() const;

			/// Set the boolean for inhibited single side coincidence
			void inhibit_single_side_coinc();

			/// Check if single side coinc is set 
			bool is_inhibited_single_side_coinc() const;
			
			/// Set the total multiplicity threshold
			void set_total_multiplicity_threshold(unsigned int threshold_);
	
			/// Check if the total multiplicity threshold is set 
			bool has_total_multiplicity_threshold() const;

			/// Check if total multiplicity threshold is set
			bool is_activated_total_multiplicity_threshold() const;

			/// Get the calo threshold for coincidences
			const	std::bitset<calo::ctw::HTM_BITSET_SIZE> get_total_multiplicity_threshold_coinc() const;
         													 																	
      /// Initializing
      void initialize_simple();

      /// Initializing
      void initialize(const datatools::properties & config_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset();

			/// Reset trigger record structure for a clocktick
			void reset_calo_record_per_clocktick();

			/// Get the level 1 finale decision bool
			const bool get_calo_decision() const; 

			/// Get the level 1 finale decision structure
			const calo_trigger_algorithm::calo_summary_record get_calo_level_1_finale_decision_structure() const;  
			
			/// General process
      void process(const calo_ctw_data & calo_ctw_data_,
									 std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_);

		protected :
		 
			/// Build the trigger record structure for a clocktick
			void _build_calo_record_per_clocktick(const calo_ctw & my_calo_ctw_);

			/// Build summary calo trigger structure
			void _build_calo_record_summary_structure(calo_summary_record & my_calo_summary_record_);

			/// Compute the trigger finale decision
			void _compute_calo_finale_decision(calo_summary_record & my_calo_summary_record_);

			/// Protected general process
			void _process(const calo_ctw_data & calo_ctw_data_,
										std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_);

			/// Display the level one calo trigger info (bitsets)
			void _display_calo_info_for_a_clocktick();
	
			/// Display the level one calo trigger info and internal working data (bitsets)
			void _display_calo_summary(calo_summary_record & my_calo_summary_record_);

    private :

			typedef boost::circular_buffer<calo_record> buffer_type;

      // Configuration :
      bool _initialized_; //!< Initialization flag
      const electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID
			bool _inhibit_both_side_coinc_;
			bool _inhibit_single_side_coinc_;
			std::bitset<calo::ctw::HTM_BITSET_SIZE> _total_multiplicity_threshold_;
			bool _activated_threshold_;
			unsigned int _circular_buffer_depth_;

      // Data :	 

			calo_record _calo_record_per_clocktick_; //!< Trigger record structure for built for each clocktick

			boost::scoped_ptr<buffer_type> _gate_circular_buffer_; //!< Scoped pointer to a circular buffer containing output data structure
 
			calo_summary_record _calo_level_1_finale_decision_; //!< Structure representing the finale decision for level 1 calorimeter
			
			bool _calo_finale_decision_; //!< Calo finale decision for an event
			
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
