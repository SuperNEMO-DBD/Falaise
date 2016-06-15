// snemo/digitization/coincidence_trigger_algorithm_test_time.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm_test_time.h>
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {

    const uint32_t coincidence_trigger_algorithm_test_time::SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS;
    const uint32_t coincidence_trigger_algorithm_test_time::SIZE_OF_L2_COINCIDENCE_DECISION_GATE;


    coincidence_trigger_algorithm_test_time::coincidence_trigger_algorithm_test_time()
    {
      _initialized_ = false;
      _caraco_decision_ = false;
      _delayed_coincidence_decision_ = false;
      return;
    }

    coincidence_trigger_algorithm_test_time::~coincidence_trigger_algorithm_test_time()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void coincidence_trigger_algorithm_test_time::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void coincidence_trigger_algorithm_test_time::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized ! ");
      _initialized_ = true;
      return;
    }

    bool coincidence_trigger_algorithm_test_time::is_initialized() const
    {
      return _initialized_;
    }

    void coincidence_trigger_algorithm_test_time::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      reset_data();
      _initialized_ = false;
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _caraco_decision_ = false;
      _delayed_coincidence_decision_ = false;
      return;
    }

    bool coincidence_trigger_algorithm_test_time::get_caraco_decision() const
    {
      return _caraco_decision_;
    }
    
    bool coincidence_trigger_algorithm_test_time::get_delayed_coincidence_decision() const
    {
      return _delayed_coincidence_decision_;
    }
    
    void coincidence_trigger_algorithm_test_time::_process_calo_tracker_coincidence(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> a_pair_for_a_clocktick_,
										   trigger_structures::coincidence_event_record & a_coincidence_record_)

    { 
      // trigger_structures::coincidence_calo_record a_calo_record = a_pair_for_a_clocktick_.first;
      // trigger_structures::tracker_record a_tracker_record = a_pair_for_a_clocktick_.second;
      
      // a_coincidence_record_.clocktick_1600ns = a_calo_record.clocktick_1600ns;
      
      // for (unsigned int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      // 	{
      // 	  for (unsigned int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
      // 	    {
      // 	      std::bitset<3> hpattern_for_a_zone = 0x0;

      // 	      int right = 0;
      // 	      int mid   = 1;
      // 	      int left  = 2;
			  
      // 	      hpattern_for_a_zone[right] = a_tracker_record.finale_data_per_zone[iside][izone][right+2];
      // 	      hpattern_for_a_zone[mid]   = a_tracker_record.finale_data_per_zone[iside][izone][mid+2];
      // 	      hpattern_for_a_zone[left]  = a_tracker_record.finale_data_per_zone[iside][izone][left+2];
      // 	      if (hpattern_for_a_zone.any())
      // 		{
      // 		  if (hpattern_for_a_zone.test(mid) && a_calo_record.calo_zoning_word[iside].test(izone) != 0)
      // 		    {
      // 		      a_coincidence_record_.zoning_word[iside].set(izone, true);
      // 		      a_coincidence_record_.decision = true;
      // 		      _caraco_decision_ = true;
      // 		    }

      // 		  if ((hpattern_for_a_zone.test(right) && a_calo_record.calo_zoning_word[iside].test(izone) != 0) ||
      // 		      (izone+1 < 10 && hpattern_for_a_zone.test(right) && a_calo_record.calo_zoning_word[iside].test(izone+1) != 0))
      // 		    {	  
      // 		      a_coincidence_record_.zoning_word[iside].set(izone, true);
      // 		      a_coincidence_record_.decision = true;
      // 		      _caraco_decision_ = true;
      // 		    }
			      
      // 		  if ((hpattern_for_a_zone.test(left) && a_calo_record.calo_zoning_word[iside].test(izone) != 0) ||
      // 		      (izone-1 > -1 && hpattern_for_a_zone.test(left) && a_calo_record.calo_zoning_word[iside].test(izone-1) != 0))
      // 		    {		 
      // 		      a_coincidence_record_.zoning_word[iside].set(izone, true);
      // 		      a_coincidence_record_.decision = true;
      // 		      _caraco_decision_ = true;
      // 		    }			  
    
      // 		} // end of hpattern.any()
      // 	    } // end of izone
      // 	} // end of iside
      
      // if (a_coincidence_record_.decision) 
      // 	{
      // 	  a_coincidence_record_.calo_zoning_word[0] = a_calo_record.calo_zoning_word[0];
      // 	  a_coincidence_record_.calo_zoning_word[1] = a_calo_record.calo_zoning_word[1];
      // 	  a_coincidence_record_.total_multiplicity_side_0 = a_calo_record.total_multiplicity_side_0;
      // 	  a_coincidence_record_.total_multiplicity_side_1 = a_calo_record.total_multiplicity_side_1;
      // 	  a_coincidence_record_.LTO_side_0 = a_calo_record.LTO_side_0;
      // 	  a_coincidence_record_.LTO_side_1 = a_calo_record.LTO_side_1;
      // 	  a_coincidence_record_.total_multiplicity_gveto = a_calo_record.total_multiplicity_gveto;
      // 	  a_coincidence_record_.LTO_gveto = a_calo_record.LTO_gveto;
      // 	  a_coincidence_record_.xt_info_bitset = a_calo_record.xt_info_bitset;
      // 	  a_coincidence_record_.single_side_coinc = a_calo_record.single_side_coinc;
      // 	  a_coincidence_record_.total_multiplicity_threshold = a_calo_record.total_multiplicity_threshold;

      // 	  for (unsigned int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      // 	    {
      // 	      for (unsigned int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
      // 		{
      // 		  a_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 		} // end of izone
      // 	    } // end of iside
      // 	  a_coincidence_record_.trigger_mode = CARACO;
	  
      // 	  bool decision_already_true_in_last_CTs = false;
      // 	  for (unsigned int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
      // 	    {
      // 	      coincidence_trigger_algorithm_test_time::L2_coincidence_decision already_created_L2_coinc_decision = _L2_coincidence_decison_records_[i]; 
      // 	      int32_t clocktick_maximum_for_decision = already_created_L2_coinc_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE;
      // 	      if (a_coincidence_record_.clocktick_1600ns < clocktick_maximum_for_decision && already_created_L2_coinc_decision.L2_clocktick_decision < a_coincidence_record_.clocktick_1600ns)
      // 		{
      // 		  decision_already_true_in_last_CTs = true;
      // 		}
      // 	    } // end of icoinc
	  
      // 	  if (decision_already_true_in_last_CTs == false)
      // 	    {
      // 	      coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_coinc_decision;
      // 	      a_L2_coinc_decision.L2_coincidence_decision_bool = true;
      // 	      a_L2_coinc_decision.L2_clocktick_decision = a_coincidence_record_.clocktick_1600ns;
      // 	      a_L2_coinc_decision.trigger_mode = CARACO;
      // 	      _L2_coincidence_decison_records_.push_back(a_L2_coinc_decision);
      // 	    }
      // 	}

      return;
    }

    void coincidence_trigger_algorithm_test_time::_build_previous_event_record(trigger_structures::coincidence_event_record & a_coincidence_record_)    
    {
      // coincidence_trigger_algorithm_test_time::previous_event_record a_previous_event_record;
      // unsigned int max_mult_side_0 = a_previous_event_record.total_multiplicity_side_0.to_ulong();
      // unsigned int max_mult_side_1 = a_previous_event_record.total_multiplicity_side_1.to_ulong();
      // unsigned int max_mult_gveto  = a_previous_event_record.total_multiplicity_gveto.to_ulong();

      // a_previous_event_record.previous_clocktick_1600ns = a_coincidence_record_.clocktick_1600ns;
      // a_previous_event_record.counter_1600ns = clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK; // Equal to 625 CT 1600 because 625 * 1600 ns = 1 ms

      // if (a_coincidence_record_.total_multiplicity_side_0.to_ulong() > max_mult_side_0) a_previous_event_record.total_multiplicity_side_0 = a_coincidence_record_.total_multiplicity_side_0;
      // if (a_coincidence_record_.total_multiplicity_side_1.to_ulong() > max_mult_side_1) a_previous_event_record.total_multiplicity_side_1 = a_coincidence_record_.total_multiplicity_side_1;

      // if (a_coincidence_record_.LTO_side_0) a_previous_event_record.LTO_side_0 = true;
      // if (a_coincidence_record_.LTO_side_1) a_previous_event_record.LTO_side_1 = true;

      // if (a_coincidence_record_.total_multiplicity_gveto.to_ulong() > max_mult_gveto) a_previous_event_record.total_multiplicity_gveto = a_coincidence_record_.total_multiplicity_gveto;

      // if (a_coincidence_record_.LTO_gveto)  a_previous_event_record.LTO_gveto  = true;

      // for (unsigned int ibit = 0; ibit < calo_trigger_algorithm_test_time::XT_INFO_BITSET_SIZE; ibit ++)
      // 	{
      // 	  if (a_coincidence_record_.xt_info_bitset.test(ibit)) a_previous_event_record.xt_info_bitset.set(ibit);
      // 	}
      // if (a_coincidence_record_.single_side_coinc) a_previous_event_record.single_side_coinc = true;
      // if (a_coincidence_record_.total_multiplicity_threshold) a_previous_event_record.total_multiplicity_threshold = true;

      // for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      // 	{
      // 	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      // 	    {
      // 	      if (a_coincidence_record_.zoning_word[iside].test(izone)) a_previous_event_record.zoning_word[iside].set(izone, true);
      // 	      if (a_coincidence_record_.calo_zoning_word[iside].test(izone)) a_previous_event_record.calo_zoning_word[iside].set(izone, true);
      // 	      for (unsigned int ibit = 0; ibit < trigger_info::DATA_FULL_BITSET_SIZE; ibit ++)
      // 		{
      // 		  if (a_coincidence_record_.tracker_finale_data_per_zone[iside][izone].test(ibit)) a_previous_event_record.tracker_finale_data_per_zone[iside][izone].set(ibit);
      // 		}
      // 	    }
      // 	}
      // _previous_event_records_->push_back(a_previous_event_record);
      
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::_process_delayed_coincidence(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> a_pair_for_a_clocktick_,
									       const trigger_structures::previous_event_record & a_previous_event_record_,
									       trigger_structures::coincidence_event_record & a_delayed_record_)
    {
      // if (a_previous_event_record_.counter_1600ns != 0) // counter != 0 ->   > 1ms (625 * 1600ns) 
      // 	{
      // 	  tracker_trigger_algorithm_test_time::tracker_record a_tracker_record = a_pair_for_a_clocktick_.second;
	  
      // 	  for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      // 	    {
      // 	      for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      // 		{
      // 		  std::bitset<3> delayed_hpattern_per_zone = 0x0;
		  
      // 		  int right = 0;
      // 		  int mid   = 1;
      // 		  int left  = 2;

		  
      // 		  delayed_hpattern_per_zone[right] = a_tracker_record.finale_data_per_zone[iside][izone][right+2]; // right + 2 -> bit position to change (bit enum in trigger_info.h)
      // 		  delayed_hpattern_per_zone[mid]   = a_tracker_record.finale_data_per_zone[iside][izone][mid+2];
      // 		  delayed_hpattern_per_zone[left]  = a_tracker_record.finale_data_per_zone[iside][izone][left+2];
		   
      // 		  // APE trigger (Tracker previous / Tracker delayed coincidence)
      // 		  if (delayed_hpattern_per_zone.any())
      // 		    { 
      // 		      if (delayed_hpattern_per_zone.test(left) && izone == 0 &&(a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
      // 										|| a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 										|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
      // 										|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
      // 			{
      // 			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			  a_delayed_record_.trigger_mode = APE;
      // 			  a_delayed_record_.decision = true;
      // 			  _delayed_coincidence_decision_ = true;
      // 			}

      // 		      if (delayed_hpattern_per_zone.test(left) && izone-1 > -1 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(right+2)
      // 										   || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
      // 										   || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 										   || a_previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(right+2)
      // 										   || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
      // 										   || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
      // 			{
      // 			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			  a_delayed_record_.trigger_mode = APE;
      // 			  a_delayed_record_.decision = true;
      // 			  _delayed_coincidence_decision_ = true;
      // 			}    
		      
      // 		      if (delayed_hpattern_per_zone.test(mid) && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
      // 								  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 								  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
      // 								  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
      // 								  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
      // 								  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
      // 			{
      // 			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			  a_delayed_record_.trigger_mode = APE;
      // 			  a_delayed_record_.decision = true;
      // 			  _delayed_coincidence_decision_ = true;
      // 			} 
		      
      // 		      if (delayed_hpattern_per_zone.test(right) && izone == 10 &&(a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 										  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
      // 										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
      // 										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
      // 			{
      // 			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			  a_delayed_record_.trigger_mode = APE;
      // 			  a_delayed_record_.decision = true;
      // 			  _delayed_coincidence_decision_ = true;
      // 			}

      // 		      if (delayed_hpattern_per_zone.test(right) && izone + 1 < 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 										      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
      // 										      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(left+2)
      // 										      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
      // 										      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)
      // 										      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(left+2)))
      // 			{
      // 			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			  a_delayed_record_.trigger_mode = APE;
      // 			  a_delayed_record_.decision = true;
      // 			  _delayed_coincidence_decision_ = true;
      // 			}
      // 		    }
      // 		} // end of izone
      // 	    } // end of iside
	  
      // 	  if (a_delayed_record_.decision)
      // 	    {
      // 	      bool decision_already_true_in_last_CTs = false;
      // 	      for (unsigned int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
      // 		{
      // 		  coincidence_trigger_algorithm_test_time::L2_coincidence_decision already_created_L2_coinc_decision = _L2_coincidence_decison_records_[i]; 
      // 		  int32_t clocktick_maximum_for_decision = already_created_L2_coinc_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE;
      // 		  if (a_delayed_record_.clocktick_1600ns < clocktick_maximum_for_decision && already_created_L2_coinc_decision.L2_clocktick_decision < a_delayed_record_.clocktick_1600ns)
      // 		    {
      // 		      decision_already_true_in_last_CTs = true;
      // 		    }
      // 		} // end of icoinc
	  
      // 	      if (decision_already_true_in_last_CTs == false)
      // 		{
      // 		  coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_coinc_decision;
      // 		  a_L2_coinc_decision.L2_coincidence_decision_bool = true;
      // 		  a_L2_coinc_decision.L2_clocktick_decision = a_delayed_record_.clocktick_1600ns;
      // 		  a_L2_coinc_decision.trigger_mode = APE;
      // 		  _L2_coincidence_decison_records_.push_back(a_L2_coinc_decision);
      // 		}
      // 	    }
	  
      // 	  // Delayed Alpha Veto Event (DAVE) trigger
      // 	  else
      // 	    {	  
      // 	      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      // 		{
      // 		  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      // 		    {
      // 		      std::bitset<2> delayed_near_source_per_zone = 0x0;
      // 		      int near_source_right = 0;
      // 		      int near_source_left  = 1;
      // 		      delayed_near_source_per_zone[near_source_right] = a_tracker_record.finale_data_per_zone[iside][izone][near_source_right + 5];
      // 		      delayed_near_source_per_zone[near_source_left]  = a_tracker_record.finale_data_per_zone[iside][izone][near_source_left + 5];
		      
      // 		      if (delayed_near_source_per_zone.any())
      // 			{
      // 			  if (delayed_near_source_per_zone.test(near_source_left) && izone == 0 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
      // 												    || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
      // 												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
      // 												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
      // 			    {
      // 			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			      a_delayed_record_.trigger_mode = DAVE;
      // 			      a_delayed_record_.decision = true;
      // 			      _delayed_coincidence_decision_ = true;
      // 			    }
			    

      // 			  if (delayed_near_source_per_zone.test(near_source_left) && izone - 1 > -1 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(near_source_right+5)
      // 													|| a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
      // 													|| a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
      // 													|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(near_source_right+5)
      // 													|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
      // 													|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
			    
      // 			    {
      // 			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			      a_delayed_record_.trigger_mode = DAVE;
      // 			      a_delayed_record_.decision = true;
      // 			      _delayed_coincidence_decision_ = true;
      // 			    }
		      

      // 			  if (delayed_near_source_per_zone.test(near_source_right) && izone == 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
      // 												      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
      // 												      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
      // 												      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
      // 			    {
      // 			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			      a_delayed_record_.trigger_mode = DAVE;
      // 			      a_delayed_record_.decision = true;
      // 			      _delayed_coincidence_decision_ = true;
      // 			    }
			  
      // 			  if (delayed_near_source_per_zone.test(near_source_right) && izone + 1 < 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
      // 													 || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
      // 													 || a_previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(near_source_left+5)
      // 													 || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
      // 													 || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)
      // 													 || a_previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(near_source_left+5)))
      // 			    {
      // 			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
      // 			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
      // 			      a_delayed_record_.trigger_mode = DAVE;
      // 			      a_delayed_record_.decision = true;
      // 			      _delayed_coincidence_decision_ = true;
      // 			    }
      // 			} // enf of if delayed any
		      
      // 		    } // end of izone
      // 		} // end of iside
	  
      // 	      if (a_delayed_record_.decision)
      // 		{
      // 		  bool decision_already_true_in_last_CTs = false;
      // 		  for (unsigned int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
      // 		    {
      // 		      coincidence_trigger_algorithm_test_time::L2_coincidence_decision already_created_L2_coinc_decision = _L2_coincidence_decison_records_[i]; 
      // 		      int32_t clocktick_maximum_for_decision = already_created_L2_coinc_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE;
      // 		      if (a_delayed_record_.clocktick_1600ns < clocktick_maximum_for_decision && already_created_L2_coinc_decision.L2_clocktick_decision < a_delayed_record_.clocktick_1600ns)
      // 			{
      // 			  decision_already_true_in_last_CTs = true;
      // 			}
      // 		    } // end of icoinc
	  
      // 		  if (decision_already_true_in_last_CTs == false)
      // 		    {
      // 		      coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_coinc_decision;
      // 		      a_L2_coinc_decision.L2_coincidence_decision_bool = true;
      // 		      a_L2_coinc_decision.L2_clocktick_decision = a_delayed_record_.clocktick_1600ns;
      // 		      a_L2_coinc_decision.trigger_mode = DAVE;
      // 		      _L2_coincidence_decison_records_.push_back(a_L2_coinc_decision);
      // 		    }
      // 		}
	    
      // 	    } //end of else
	  
      // 	} // end of if counter != 0
  
      return;
    }

    void coincidence_trigger_algorithm_test_time::process(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> pair_for_a_clocktick_,
							  trigger_structures::coincidence_event_record & a_coincidence_record_,
							  trigger_structures::L2_decision & a_L2_decision_record_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(pair_for_a_clocktick_,
	       a_coincidence_record_,
	       a_L2_decision_record_);
      return;
    }

    void coincidence_trigger_algorithm_test_time::_process(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> pair_for_a_clocktick_,
							   trigger_structures::coincidence_event_record & a_coincidence_record_,
							   trigger_structures::L2_decision & a_L2_decision_record_)
    {
      

      return;
    }
    
  } // end of namespace digitization
  
} // end of namespace snemo
