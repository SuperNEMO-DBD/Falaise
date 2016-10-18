// snemo/digitization/trigger_structures.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Francois MAUGER <mauger@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_STRUCTURES_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_STRUCTURES_H

// Standard library :
#include <bitset>

// Third part : 
// Boost :

// This project :
#include <snemo/digitization/calo_ctw_constants.h>
#include <snemo/digitization/trigger_info.h>

namespace snemo {
  
  namespace digitization {

     /// \brief Trigger structures build for trigger response.

    class trigger_structures
    {
    public :
      
      /// Trigger display manager is a friend because it can access to members for display
      friend class trigger_display_manager;
      
      enum L2_trigger_mode{
	INVALID   = 0,
	CALO_ONLY = 1,
	CALO_TRACKER_TIME_COINC = 2,
	CARACO       = 3,
	OPEN_DELAYED = 4,
	APE          = 5,
	DAVE         = 6					
      };

      // Calorimeter trigger structures :
      struct calo_record
      {
	calo_record();
	void reset();
	void display() const;
	uint32_t clocktick_25ns;
	std::bitset<trigger_info::NZONES> zoning_word[trigger_info::NSIDES];
	std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_0;
	std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_1;
	bool LTO_side_0;
	bool LTO_side_1;
	std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_gveto;
	bool LTO_gveto;
	std::bitset<trigger_info::CALO_XT_INFO_BITSET_SIZE> xt_info_bitset;
      };
			
      struct calo_summary_record : public calo_record
      {
	calo_summary_record();
	void reset();
	void reset_summary_boolean_only();
	void display() const;
	bool is_empty() const;
	bool single_side_coinc;
	bool total_multiplicity_threshold;
	bool calo_finale_decision;
      };
      
      // Tracker trigger structures :
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
	bool is_empty() const;
	void display();
	uint32_t clocktick_1600ns;
	std::bitset<trigger_info::DATA_FULL_BITSET_SIZE> finale_data_per_zone[trigger_info::NSIDES][trigger_info::NZONES];
	std::bitset<trigger_info::NZONES> zoning_word_pattern[trigger_info::NSIDES];
	std::bitset<trigger_info::NZONES> zoning_word_near_source[trigger_info::NSIDES];
	bool single_side_coinc;
	bool finale_decision;
      };

      struct geiger_matrix
      {
	geiger_matrix();
	void reset();
	bool is_empty() const;
	void display() const;
	uint32_t clocktick_1600ns;				
	bool matrix[trigger_info::NSIDES][trigger_info::NLAYERS][trigger_info::NROWS];
      };

      // Coincidence trigger structures :
      struct coincidence_base_record
      {
	coincidence_base_record();
	void reset();
	void display() const;	
	std::bitset<trigger_info::NZONES> calo_zoning_word[trigger_info::NSIDES];
	std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_0;
	std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_side_1;
	bool LTO_side_0;
	bool LTO_side_1;
	std::bitset<calo::ctw::HTM_BITSET_SIZE> total_multiplicity_gveto;
	bool LTO_gveto;
	std::bitset<trigger_info::CALO_XT_INFO_BITSET_SIZE> xt_info_bitset;
	bool single_side_coinc;
	bool total_multiplicity_threshold;
	bool decision;
      };
			
      struct coincidence_calo_record : public coincidence_base_record
      {
	coincidence_calo_record();
	void reset();
	void display() const;
	bool is_empty() const;	
	uint32_t clocktick_1600ns;
      };

      struct coincidence_event_record : public coincidence_base_record
      {
	coincidence_event_record();
	void reset();
	void display() const;
	bool is_empty() const;	
	uint32_t clocktick_1600ns;
	// Coincidence zoning word :
	std::bitset<trigger_info::NZONES> coincidence_zoning_word[trigger_info::NSIDES];
	// Traker pattern zoning word :
	std::bitset<trigger_info::NZONES> tracker_zoning_word_pattern[trigger_info::NSIDES];
	// Tracker near source zoning word :
	std::bitset<trigger_info::NZONES> tracker_zoning_word_near_source[trigger_info::NSIDES];
	std::bitset<trigger_info::DATA_FULL_BITSET_SIZE> tracker_finale_data_per_zone[trigger_info::NSIDES][trigger_info::NZONES];
	trigger_structures::L2_trigger_mode trigger_mode;
      };

      struct previous_event_record : public coincidence_base_record
      {
	previous_event_record();
	void reset();
	void display() const;
	bool is_empty() const;	
	uint32_t previous_clocktick_1600ns;
	uint32_t counter_1600ns;
	// Coincidence zoning word :
	std::bitset<trigger_info::NZONES> coincidence_zoning_word[trigger_info::NSIDES];
	// Traker pattern zoning word :
	std::bitset<trigger_info::NZONES> tracker_zoning_word_pattern[trigger_info::NSIDES];
	// Tracker near source zoning word :
	std::bitset<trigger_info::NZONES> tracker_zoning_word_near_source[trigger_info::NSIDES];
	std::bitset<trigger_info::DATA_FULL_BITSET_SIZE> tracker_finale_data_per_zone[trigger_info::NSIDES][trigger_info::NZONES];
	trigger_structures::L2_trigger_mode trigger_mode;
      };

      // L1 and L2 trigger decision structures :

      struct L1_calo_decision
      {
	L1_calo_decision();
	void reset();
	void display() const;
	bool L1_calo_decision_bool;
	uint32_t L1_calo_ct_decision; // CT @ 25 ns
      };

      struct L1_tracker_decision
      {
	L1_tracker_decision();
	void reset();
	void display() const;
	bool L1_tracker_decision_bool;
	uint32_t L1_tracker_ct_decision; // CT @ 1600 ns
      };
			
      struct L2_decision
      {
	L2_decision();
	void reset();
	void display() const;
	bool L2_decision_bool;
	uint32_t L2_ct_decision; // CT @ 1600 ns
	trigger_structures::L2_trigger_mode L2_trigger_mode;
      };

    };

  } // end of namespace digitization
  
} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_STRUCTURES_H */
