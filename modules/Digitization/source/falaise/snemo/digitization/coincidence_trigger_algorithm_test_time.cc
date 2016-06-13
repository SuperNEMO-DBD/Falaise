// snemo/digitization/coincidence_trigger_algorithm_test_time.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm_test_time.h>
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    const uint32_t coincidence_trigger_algorithm_test_time::SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS;
    const uint32_t coincidence_trigger_algorithm_test_time::SIZE_OF_L2_COINCIDENCE_DECISION_GATE;

    coincidence_trigger_algorithm_test_time::coincidence_base_record::coincidence_base_record()
    {
      coincidence_base_record::reset();
      return;
    }

    void coincidence_trigger_algorithm_test_time::coincidence_base_record::reset()
    {
      calo_zoning_word[0].reset();
      calo_zoning_word[1].reset();
      total_multiplicity_side_0.reset();
      total_multiplicity_side_1.reset();
      LTO_side_0 = false;
      LTO_side_1 = false;
      total_multiplicity_gveto.reset();
      LTO_gveto = false;
      xt_info_bitset.reset();
      single_side_coinc = true;
      total_multiplicity_threshold = false;
      decision = false;
      return;
    }
    
    void  coincidence_trigger_algorithm_test_time::coincidence_base_record::display() const
    {
      std::clog << "XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl; 
      std::clog << xt_info_bitset << ' ';
      std::clog << LTO_gveto << ' ';
      std::clog << total_multiplicity_gveto << ' ';
      std::clog << LTO_side_1 << ' ';
      std::clog << LTO_side_0 << ' ';
      std::clog << total_multiplicity_side_1 << ' ';
      std::clog << total_multiplicity_side_0 << ' ';
      for (unsigned int iside = trigger_info::NSIDES-1; iside > 0; iside--)
      	{
      	  for (unsigned int izone = trigger_info::NZONES-1; izone > 0 ; izone--)
      	    {
      	      std::clog << calo_zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}      
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << single_side_coinc 
		<< "  |  Threshold total mult : "   << total_multiplicity_threshold << std::endl;     
      return;
    }
    
    coincidence_trigger_algorithm_test_time::coincidence_calo_record::coincidence_calo_record()
    {
      coincidence_calo_record::reset();
      return;
    }

    void coincidence_trigger_algorithm_test_time::coincidence_calo_record::reset()
    {
      coincidence_base_record::reset();
      clocktick_1600ns = -1;
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::coincidence_calo_record::display() const
    {
      std::clog << "************************************************************************************" << std::endl;
      std::clog << "*************************** Coincidence calo record ********************" << std::endl;
      std::clog << "*************************** Clocktick 1600 = " << clocktick_1600ns << " ***************************" << std::endl;
      coincidence_base_record::display();
      std::clog << "Coincidence calo record decision : [" << decision << "]" << std::endl;
      std::clog << std::endl;
      return;
    }

    coincidence_trigger_algorithm_test_time::coincidence_event_record::coincidence_event_record()
    {
      coincidence_event_record::reset();
      return;
    }

    void coincidence_trigger_algorithm_test_time::coincidence_event_record::reset()
    {
      coincidence_base_record::reset();
      clocktick_1600ns = -1;
      trigger_mode = INVALID;
      zoning_word[0].reset();
      zoning_word[1].reset();
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      tracker_finale_data_per_zone[iside][izone].reset();
	    }
	}	       
      return;
    }

    void coincidence_trigger_algorithm_test_time::coincidence_event_record::display() const
    {
      std::clog << "************************************************************************************" << std::endl;
      std::clog << "*************************** Coincidence event record ********************" << std::endl;
      std::clog << "*************************** Clocktick 1600 = " << clocktick_1600ns << " ***************************" << std::endl;

      coincidence_base_record::display();
      std::clog << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      std::clog << "[" << tracker_finale_data_per_zone[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	}	      
      std::clog << "Coincidence zoning word : " << std::endl;
      std::clog << "ZW [0] = " << zoning_word[0] << std::endl;
      std::clog << "ZW [1] = " << zoning_word[1] << std::endl;
      std::clog << "Coincidence trigger mode : [" << trigger_mode << "]" << std::endl;
      std::clog << "Coincidence event record decision : [" << decision << "]" << std::endl;
      return;
    }

    coincidence_trigger_algorithm_test_time::L2_coincidence_decision::L2_coincidence_decision()
    {
      L2_coincidence_decision::reset();
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::L2_coincidence_decision::reset()
    {
      L2_coincidence_decision_bool = false;
      L2_clocktick_decision = -1;
      trigger_mode = INVALID;
      return;
    }

    void coincidence_trigger_algorithm_test_time::L2_coincidence_decision::display() const
    {
      std::clog << "L2 coincidence decision structure : " << std::endl;
      std::clog << "L2 decision   [" << L2_coincidence_decision_bool << "]" << std::endl;
      std::clog << "CT decision  : " << L2_clocktick_decision << std::endl;
      std::clog << "Trigger mode : " << trigger_mode << std::endl;
      return;
    }

    coincidence_trigger_algorithm_test_time::previous_event_record::previous_event_record()
    {
      previous_event_record::reset();
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::previous_event_record::reset()
    {
      coincidence_base_record::reset();
      previous_clocktick_1600ns = -1;
      counter_1600ns = 0;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      tracker_finale_data_per_zone[iside][izone].reset();
	    }
	}	       
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::previous_event_record::display() const
    {
      std::clog << "************************************************************************************" << std::endl;
      std::clog << "*************************** Previous event record ********************" << std::endl;
      std::clog << "*************************** Previous clocktick 1600 = " << previous_clocktick_1600ns << " ********************" << std::endl;
      std::clog << "*************************** Counter 1600 = " << counter_1600ns << " ***************************" << std::endl;
      coincidence_base_record::display();
      std::clog << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      std::clog << "[" << tracker_finale_data_per_zone[iside][izone] << "] ";
	    } // end of izone
	  std::clog << std::endl;
	}	     
      std::clog << "Coincidence zoning word : " << std::endl;
      std::clog << "ZW [0] = " << zoning_word[0] << std::endl;
      std::clog << "ZW [1] = " << zoning_word[1] << std::endl;
      std::clog << std::endl;
      return;
    }

    coincidence_trigger_algorithm_test_time::coincidence_trigger_algorithm_test_time()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _coincidence_calorimeter_gate_size_ = 0;
      _previous_event_circular_buffer_depth_ = 0;
      _coincidence_calo_records_.reserve(SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS);
      _caraco_decision_ = false;
      _delayed_coincidence_decision_ = false;
      _previous_event_records_.reset();
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

    void coincidence_trigger_algorithm_test_time::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    bool coincidence_trigger_algorithm_test_time::has_calorimeter_gate_size() const
    {
      return _coincidence_calorimeter_gate_size_ != 0;
    }

    void coincidence_trigger_algorithm_test_time::set_calorimeter_gate_size(unsigned int calorimeter_gate_size_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized, coincidence calorimeter gate size can't be set ! ");
      _coincidence_calorimeter_gate_size_ = calorimeter_gate_size_;
      return;
    }

    
    bool coincidence_trigger_algorithm_test_time::has_previous_event_buffer_depth() const
    {
      return _previous_event_circular_buffer_depth_ != 0;
    }

    void coincidence_trigger_algorithm_test_time::set_previous_event_buffer_depth(unsigned int previous_event_circular_buffer_depth_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized, previous event buffer depth can't be set ! ");
      _previous_event_circular_buffer_depth_ = previous_event_circular_buffer_depth_;
      return;
    }

    unsigned int coincidence_trigger_algorithm_test_time::get_previous_event_buffer_depth() const
    {
      return _previous_event_circular_buffer_depth_;
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
      _electronic_mapping_ = 0;
      reset_data();
      _initialized_ = false;
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _coincidence_calo_records_.clear();
      _caraco_decision_ = false;
      _delayed_coincidence_decision_ = false;
      _pair_records_.clear();
      _previous_event_records_.reset();
      _L2_coincidence_decison_records_.clear();
      return;
    }

    const std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> coincidence_trigger_algorithm_test_time::get_coincidence_calo_records_vector() const
    {
      return _coincidence_calo_records_;
    }

    bool coincidence_trigger_algorithm_test_time::get_caraco_decision() const
    {
      return _caraco_decision_;
    }
    
    bool coincidence_trigger_algorithm_test_time::get_delayed_coincidence_decision() const
    {
      return _delayed_coincidence_decision_;
    }

    void coincidence_trigger_algorithm_test_time::_creating_pair_per_clocktick(const std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> & coinc_calo_records_,
									       const std::vector<tracker_trigger_algorithm_test_time::tracker_record> & tracker_records_)
    {
      std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record,tracker_trigger_algorithm_test_time::tracker_record> pair_for_a_clocktick;
      
      if (coinc_calo_records_.size() != 0 || tracker_records_.size() != 0)
	{
	  std::vector<coincidence_calo_record>::const_iterator it_calo = coinc_calo_records_.begin();
	  for (; it_calo != _coincidence_calo_records_.end(); it_calo++)
	    {
	      coincidence_calo_record a_coinc_calo_record = *it_calo;
	      pair_for_a_clocktick.first = a_coinc_calo_record;
	      _pair_records_.push_back(pair_for_a_clocktick);
	    } 
	  std::vector<tracker_trigger_algorithm_test_time::tracker_record>::const_iterator it_tracker = tracker_records_.begin();
	  // Update of the pair if calo CT = tracker CT
	  for (; it_tracker != tracker_records_.end(); it_tracker++)
	    {
	      tracker_trigger_algorithm_test_time::tracker_record a_tracker_record = *it_tracker;
	      for (unsigned int i = 0; i < _pair_records_.size(); i++)
		{
		  std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record,tracker_trigger_algorithm_test_time::tracker_record> a_pair = _pair_records_[i]; 
		  coincidence_calo_record a_coinc_calo_record = a_pair.first;
		  if (a_tracker_record.clocktick_1600ns == a_coinc_calo_record.clocktick_1600ns)
		    {
		      a_pair.second = a_tracker_record;
		      _pair_records_[i] = a_pair;
		    }
		  else 
		    {
		      a_pair.second.clocktick_1600ns = a_coinc_calo_record.clocktick_1600ns;
		    }
		}
	    }
	}

      int32_t last_clocktick = -1;
      // Create a new pair if there is only tracker for example :
      if (_pair_records_.size() != 0)
	{
	  std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> last_pair = _pair_records_.back();
	  last_clocktick = last_pair.first.clocktick_1600ns;
	}
      
      std::vector<tracker_trigger_algorithm_test_time::tracker_record>::const_iterator it_tracker =  tracker_records_.begin();
      for (; it_tracker != tracker_records_.end(); it_tracker++)
	{
	  tracker_trigger_algorithm_test_time::tracker_record a_tracker_record = *it_tracker;
	  if (last_clocktick == -1 || a_tracker_record.clocktick_1600ns > last_clocktick)
	    {
	      std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record,tracker_trigger_algorithm_test_time::tracker_record> a_new_pair;	 
	      a_new_pair.second = a_tracker_record;
	      a_new_pair.first.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
	      _pair_records_.push_back(a_new_pair);
	    }
	}
      return;
    }

    void coincidence_trigger_algorithm_test_time::_process_calo_tracker_coincidence(const std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> a_pair_for_a_clocktick_,
										    coincidence_trigger_algorithm_test_time::coincidence_event_record & a_coincidence_record_)

    { 
      coincidence_trigger_algorithm_test_time::coincidence_calo_record a_calo_record = a_pair_for_a_clocktick_.first;
      tracker_trigger_algorithm_test_time::tracker_record a_tracker_record = a_pair_for_a_clocktick_.second;
      
      a_coincidence_record_.clocktick_1600ns = a_calo_record.clocktick_1600ns;
      
      for (unsigned int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (unsigned int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      std::bitset<3> hpattern_for_a_zone = 0x0;

	      int right = 0;
	      int mid   = 1;
	      int left  = 2;
			  
	      hpattern_for_a_zone[right] = a_tracker_record.finale_data_per_zone[iside][izone][right+2];
	      hpattern_for_a_zone[mid]   = a_tracker_record.finale_data_per_zone[iside][izone][mid+2];
	      hpattern_for_a_zone[left]  = a_tracker_record.finale_data_per_zone[iside][izone][left+2];
	      if (hpattern_for_a_zone.any())
		{
		  if (hpattern_for_a_zone.test(mid) && a_calo_record.calo_zoning_word[iside].test(izone) != 0)
		    {
		      a_coincidence_record_.zoning_word[iside].set(izone, true);
		      a_coincidence_record_.decision = true;
		      _caraco_decision_ = true;
		    }

		  if ((hpattern_for_a_zone.test(right) && a_calo_record.calo_zoning_word[iside].test(izone) != 0) ||
		      (izone+1 < 10 && hpattern_for_a_zone.test(right) && a_calo_record.calo_zoning_word[iside].test(izone+1) != 0))
		    {	  
		      a_coincidence_record_.zoning_word[iside].set(izone, true);
		      a_coincidence_record_.decision = true;
		      _caraco_decision_ = true;
		    }
			      
		  if ((hpattern_for_a_zone.test(left) && a_calo_record.calo_zoning_word[iside].test(izone) != 0) ||
		      (izone-1 > -1 && hpattern_for_a_zone.test(left) && a_calo_record.calo_zoning_word[iside].test(izone-1) != 0))
		    {		 
		      a_coincidence_record_.zoning_word[iside].set(izone, true);
		      a_coincidence_record_.decision = true;
		      _caraco_decision_ = true;
		    }			  
    
		} // end of hpattern.any()
	    } // end of izone
	} // end of iside
      
      if (a_coincidence_record_.decision) 
	{
	  a_coincidence_record_.calo_zoning_word[0] = a_calo_record.calo_zoning_word[0];
	  a_coincidence_record_.calo_zoning_word[1] = a_calo_record.calo_zoning_word[1];
	  a_coincidence_record_.total_multiplicity_side_0 = a_calo_record.total_multiplicity_side_0;
	  a_coincidence_record_.total_multiplicity_side_1 = a_calo_record.total_multiplicity_side_1;
	  a_coincidence_record_.LTO_side_0 = a_calo_record.LTO_side_0;
	  a_coincidence_record_.LTO_side_1 = a_calo_record.LTO_side_1;
	  a_coincidence_record_.total_multiplicity_gveto = a_calo_record.total_multiplicity_gveto;
	  a_coincidence_record_.LTO_gveto = a_calo_record.LTO_gveto;
	  a_coincidence_record_.xt_info_bitset = a_calo_record.xt_info_bitset;
	  a_coincidence_record_.single_side_coinc = a_calo_record.single_side_coinc;
	  a_coincidence_record_.total_multiplicity_threshold = a_calo_record.total_multiplicity_threshold;

	  for (unsigned int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	    {
	      for (unsigned int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
		{
		  a_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
		} // end of izone
	    } // end of iside
	  a_coincidence_record_.trigger_mode = CARACO;
	  
	  bool decision_already_true_in_last_CTs = false;
	  for (unsigned int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
	    {
	      coincidence_trigger_algorithm_test_time::L2_coincidence_decision already_created_L2_coinc_decision = _L2_coincidence_decison_records_[i]; 
	      int32_t clocktick_maximum_for_decision = already_created_L2_coinc_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE;
	      if (a_coincidence_record_.clocktick_1600ns < clocktick_maximum_for_decision && already_created_L2_coinc_decision.L2_clocktick_decision < a_coincidence_record_.clocktick_1600ns)
		{
		  decision_already_true_in_last_CTs = true;
		}
	    } // end of icoinc
	  
	  if (decision_already_true_in_last_CTs == false)
	    {
	      coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_coinc_decision;
	      a_L2_coinc_decision.L2_coincidence_decision_bool = true;
	      a_L2_coinc_decision.L2_clocktick_decision = a_coincidence_record_.clocktick_1600ns;
	      a_L2_coinc_decision.trigger_mode = CARACO;
	      _L2_coincidence_decison_records_.push_back(a_L2_coinc_decision);
	    }
	}

      return;
    }

    void coincidence_trigger_algorithm_test_time::_build_previous_event_record(coincidence_trigger_algorithm_test_time::coincidence_event_record & a_coincidence_record_)    
    {
      coincidence_trigger_algorithm_test_time::previous_event_record a_previous_event_record;
      unsigned int max_mult_side_0 = a_previous_event_record.total_multiplicity_side_0.to_ulong();
      unsigned int max_mult_side_1 = a_previous_event_record.total_multiplicity_side_1.to_ulong();
      unsigned int max_mult_gveto  = a_previous_event_record.total_multiplicity_gveto.to_ulong();

      a_previous_event_record.previous_clocktick_1600ns = a_coincidence_record_.clocktick_1600ns;
      a_previous_event_record.counter_1600ns = clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK; // Equal to 625 CT 1600 because 625 * 1600 ns = 1 ms

      if (a_coincidence_record_.total_multiplicity_side_0.to_ulong() > max_mult_side_0) a_previous_event_record.total_multiplicity_side_0 = a_coincidence_record_.total_multiplicity_side_0;
      if (a_coincidence_record_.total_multiplicity_side_1.to_ulong() > max_mult_side_1) a_previous_event_record.total_multiplicity_side_1 = a_coincidence_record_.total_multiplicity_side_1;

      if (a_coincidence_record_.LTO_side_0) a_previous_event_record.LTO_side_0 = true;
      if (a_coincidence_record_.LTO_side_1) a_previous_event_record.LTO_side_1 = true;

      if (a_coincidence_record_.total_multiplicity_gveto.to_ulong() > max_mult_gveto) a_previous_event_record.total_multiplicity_gveto = a_coincidence_record_.total_multiplicity_gveto;

      if (a_coincidence_record_.LTO_gveto)  a_previous_event_record.LTO_gveto  = true;

      for (unsigned int ibit = 0; ibit < calo_trigger_algorithm_test_time::XT_INFO_BITSET_SIZE; ibit ++)
	{
	  if (a_coincidence_record_.xt_info_bitset.test(ibit)) a_previous_event_record.xt_info_bitset.set(ibit);
	}
      if (a_coincidence_record_.single_side_coinc) a_previous_event_record.single_side_coinc = true;
      if (a_coincidence_record_.total_multiplicity_threshold) a_previous_event_record.total_multiplicity_threshold = true;

      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      if (a_coincidence_record_.zoning_word[iside].test(izone)) a_previous_event_record.zoning_word[iside].set(izone, true);
	      if (a_coincidence_record_.calo_zoning_word[iside].test(izone)) a_previous_event_record.calo_zoning_word[iside].set(izone, true);
	      for (unsigned int ibit = 0; ibit < trigger_info::DATA_FULL_BITSET_SIZE; ibit ++)
		{
		  if (a_coincidence_record_.tracker_finale_data_per_zone[iside][izone].test(ibit)) a_previous_event_record.tracker_finale_data_per_zone[iside][izone].set(ibit);
		}
	    }
	}
      _previous_event_records_->push_back(a_previous_event_record);
      
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::_process_delayed_coincidence(const std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> a_pair_for_a_clocktick_,
									       const coincidence_trigger_algorithm_test_time::previous_event_record & a_previous_event_record_,
									       coincidence_trigger_algorithm_test_time::coincidence_event_record & a_delayed_record_)
    {
      if (a_previous_event_record_.counter_1600ns != 0) // counter != 0 ->   > 1ms (625 * 1600ns) 
      	{
	  tracker_trigger_algorithm_test_time::tracker_record a_tracker_record = a_pair_for_a_clocktick_.second;
	  
	  for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	    {
	      for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
		{
		  std::bitset<3> delayed_hpattern_per_zone = 0x0;
		  
		  int right = 0;
		  int mid   = 1;
		  int left  = 2;

		  
		  delayed_hpattern_per_zone[right] = a_tracker_record.finale_data_per_zone[iside][izone][right+2]; // right + 2 -> bit position to change (bit enum in trigger_info.h)
		  delayed_hpattern_per_zone[mid]   = a_tracker_record.finale_data_per_zone[iside][izone][mid+2];
		  delayed_hpattern_per_zone[left]  = a_tracker_record.finale_data_per_zone[iside][izone][left+2];
		   
		  // APE trigger (Tracker previous / Tracker delayed coincidence)
		  if (delayed_hpattern_per_zone.any())
		    { 
		      if (delayed_hpattern_per_zone.test(left) && izone == 0 &&(a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
										|| a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
										|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
										|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
			{
			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_record_.trigger_mode = APE;
			  a_delayed_record_.decision = true;
			  _delayed_coincidence_decision_ = true;
			}

		      if (delayed_hpattern_per_zone.test(left) && izone-1 > -1 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(right+2)
										   || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
										   || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
										   || a_previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(right+2)
										   || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
										   || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
			{
			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_record_.trigger_mode = APE;
			  a_delayed_record_.decision = true;
			  _delayed_coincidence_decision_ = true;
			}    
		      
		      if (delayed_hpattern_per_zone.test(mid) && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
								  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
								  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
								  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
								  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
								  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
			{
			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_record_.trigger_mode = APE;
			  a_delayed_record_.decision = true;
			  _delayed_coincidence_decision_ = true;
			} 
		      
		      if (delayed_hpattern_per_zone.test(right) && izone == 10 &&(a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
			{
			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_record_.trigger_mode = APE;
			  a_delayed_record_.decision = true;
			  _delayed_coincidence_decision_ = true;
			}

		      if (delayed_hpattern_per_zone.test(right) && izone + 1 < 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
										      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
										      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(left+2)
										      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
										      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)
										      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(left+2)))
			{
			  a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_record_.trigger_mode = APE;
			  a_delayed_record_.decision = true;
			  _delayed_coincidence_decision_ = true;
			}
		    }
		} // end of izone
	    } // end of iside
	  
	  if (a_delayed_record_.decision)
	    {
	      bool decision_already_true_in_last_CTs = false;
	      for (unsigned int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
		{
		  coincidence_trigger_algorithm_test_time::L2_coincidence_decision already_created_L2_coinc_decision = _L2_coincidence_decison_records_[i]; 
		  int32_t clocktick_maximum_for_decision = already_created_L2_coinc_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE;
		  if (a_delayed_record_.clocktick_1600ns < clocktick_maximum_for_decision && already_created_L2_coinc_decision.L2_clocktick_decision < a_delayed_record_.clocktick_1600ns)
		    {
		      decision_already_true_in_last_CTs = true;
		    }
		} // end of icoinc
	  
	      if (decision_already_true_in_last_CTs == false)
		{
		  coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_coinc_decision;
		  a_L2_coinc_decision.L2_coincidence_decision_bool = true;
		  a_L2_coinc_decision.L2_clocktick_decision = a_delayed_record_.clocktick_1600ns;
		  a_L2_coinc_decision.trigger_mode = APE;
		  _L2_coincidence_decison_records_.push_back(a_L2_coinc_decision);
		}
	    }
	  
	  // Delayed Alpha Veto Event (DAVE) trigger
	  else
	    {	  
	      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
		{
		  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
		    {
		      std::bitset<2> delayed_near_source_per_zone = 0x0;
		      int near_source_right = 0;
		      int near_source_left  = 1;
		      delayed_near_source_per_zone[near_source_right] = a_tracker_record.finale_data_per_zone[iside][izone][near_source_right + 5];
		      delayed_near_source_per_zone[near_source_left]  = a_tracker_record.finale_data_per_zone[iside][izone][near_source_left + 5];
		      
		      if (delayed_near_source_per_zone.any())
			{
			  if (delayed_near_source_per_zone.test(near_source_left) && izone == 0 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
			    {
			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			      a_delayed_record_.trigger_mode = DAVE;
			      a_delayed_record_.decision = true;
			      _delayed_coincidence_decision_ = true;
			    }
			    

			  if (delayed_near_source_per_zone.test(near_source_left) && izone - 1 > -1 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(near_source_right+5)
													|| a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
													|| a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
													|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(near_source_right+5)
													|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
													|| a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
			    
			    {
			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			      a_delayed_record_.trigger_mode = DAVE;
			      a_delayed_record_.decision = true;
			      _delayed_coincidence_decision_ = true;
			    }
		      

			  if (delayed_near_source_per_zone.test(near_source_right) && izone == 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
												      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
												      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
												      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
			    {
			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			      a_delayed_record_.trigger_mode = DAVE;
			      a_delayed_record_.decision = true;
			      _delayed_coincidence_decision_ = true;
			    }
			  
			  if (delayed_near_source_per_zone.test(near_source_right) && izone + 1 < 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
													 || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
													 || a_previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(near_source_left+5)
													 || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
													 || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)
													 || a_previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(near_source_left+5)))
			    {
			      a_delayed_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			      a_delayed_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			      a_delayed_record_.trigger_mode = DAVE;
			      a_delayed_record_.decision = true;
			      _delayed_coincidence_decision_ = true;
			    }
			} // enf of if delayed any
		      
		    } // end of izone
		} // end of iside
	  
	      if (a_delayed_record_.decision)
		{
		  bool decision_already_true_in_last_CTs = false;
		  for (unsigned int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
		    {
		      coincidence_trigger_algorithm_test_time::L2_coincidence_decision already_created_L2_coinc_decision = _L2_coincidence_decison_records_[i]; 
		      int32_t clocktick_maximum_for_decision = already_created_L2_coinc_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE;
		      if (a_delayed_record_.clocktick_1600ns < clocktick_maximum_for_decision && already_created_L2_coinc_decision.L2_clocktick_decision < a_delayed_record_.clocktick_1600ns)
			{
			  decision_already_true_in_last_CTs = true;
			}
		    } // end of icoinc
	  
		  if (decision_already_true_in_last_CTs == false)
		    {
		      coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_coinc_decision;
		      a_L2_coinc_decision.L2_coincidence_decision_bool = true;
		      a_L2_coinc_decision.L2_clocktick_decision = a_delayed_record_.clocktick_1600ns;
		      a_L2_coinc_decision.trigger_mode = DAVE;
		      _L2_coincidence_decison_records_.push_back(a_L2_coinc_decision);
		    }
		}
	    
	    } //end of else
	  
	} // end of if counter != 0
  
      return;
    }

    void coincidence_trigger_algorithm_test_time::process(const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> & calo_records_,
							  const std::vector<tracker_trigger_algorithm_test_time::tracker_record> & tracker_records_,
							  std::vector<coincidence_trigger_algorithm_test_time::coincidence_event_record> & coincidence_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(calo_records_,
	       tracker_records_,
	       coincidence_records_);
      return;
    }

    void coincidence_trigger_algorithm_test_time::_process(const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> & calo_records_,
							   const std::vector<tracker_trigger_algorithm_test_time::tracker_record> & tracker_records_,
							   std::vector<coincidence_trigger_algorithm_test_time::coincidence_event_record> & coincidence_records_)
    {
      // reset_data();
      // _previous_event_records_.reset(new buffer_previous_event_record_type(_previous_event_circular_buffer_depth_));
      // _creating_pair_per_clocktick(_coincidence_calo_records_, tracker_records_);
     
      // std::vector<std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record,tracker_trigger_algorithm_test_time::tracker_record> >::iterator it_pair = _pair_records_.begin();
      // for (; it_pair != _pair_records_.end(); it_pair++)
      // 	{
      // 	  std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> a_pair = *it_pair;
      // 	  uint32_t pair_clocktick_1600ns = a_pair.first.clocktick_1600ns;

      // 	  coincidence_event_record a_coincidence_record;
      // 	  _process_calo_tracker_coincidence(a_pair, a_coincidence_record);

      // 	  if (a_coincidence_record.decision)
      // 	    {	  
      // 	      // Add the coincidence in the records :

      // 	      coincidence_records_.push_back(a_coincidence_record);
      // 	      coincidence_trigger_algorithm_test_time::L2_coincidence_decision last_L2_decision = _L2_coincidence_decison_records_.front();
      // 	      // Search the last L2 decision (in CT)
      // 	      for (int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
      // 		{
      // 		  coincidence_trigger_algorithm_test_time::L2_coincidence_decision a_L2_decision =_L2_coincidence_decison_records_[i];
      // 		  if (a_L2_decision.L2_clocktick_decision > last_L2_decision.L2_clocktick_decision) last_L2_decision = a_L2_decision;
      // 		}
	      	      
      // 	      // For a CARACO L2 decision, create a PER at the end of the L2 decision
      // 	      if (last_L2_decision.L2_coincidence_decision_bool && pair_clocktick_1600ns == last_L2_decision.L2_clocktick_decision + SIZE_OF_L2_COINCIDENCE_DECISION_GATE - 1 && last_L2_decision.trigger_mode == CARACO)
      // 		{
      // 		  _build_previous_event_record(a_coincidence_record);		  
      // 		}
      // 	    }
      //  	  else
      // 	    {
      // 	      if (_previous_event_records_->size() != 0)
      // 		{
      // 		  boost::circular_buffer<coincidence_trigger_algorithm_test_time::previous_event_record>::iterator per_it = _previous_event_records_->begin();

      // 		  for (; per_it != _previous_event_records_->end(); per_it++)
      // 		    {
      // 		      coincidence_trigger_algorithm_test_time::previous_event_record a_previous_event_record = *per_it;
      // 		      if (pair_clocktick_1600ns < a_previous_event_record.previous_clocktick_1600ns){}
      // 		      else
      // 			{
      // 			  a_previous_event_record.counter_1600ns =  clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK - (a_pair.first.clocktick_1600ns - a_previous_event_record.previous_clocktick_1600ns);
		      
      // 			  if (a_previous_event_record.counter_1600ns > 0)
      // 			    {
			  
      // 			      coincidence_event_record a_delayed_event_record;
      // 			      // Compare tracker record only with previous event
      // 			      _process_delayed_coincidence(a_pair,
      // 							   a_previous_event_record,
      // 							   a_delayed_event_record); 

      // 			      if (a_delayed_event_record.decision) 
      // 				{
      // 				  coincidence_records_.push_back(a_delayed_event_record);
      // 				}
      // 			    }

      // 			  *per_it = a_previous_event_record;
      // 			}
      // 		    }
      // 		}
      // 	    } // end of else
      // 	} // end of it_pair

      // std::clog << "Size of L2 coinc records = " << _L2_coincidence_decison_records_.size() << std::endl;
      // for (int i = 0; i < _L2_coincidence_decison_records_.size(); i++)
      // 	{
      // 	  _L2_coincidence_decison_records_[i].display();
      // 	}
	  
      // std::clog << "Size of Previous event records = " << _previous_event_records_->size() << std::endl;
      // for (int i = 0; i < _previous_event_records_->size(); i++)
      // 	{
      // 	  _previous_event_records_->at(i).display();
      // 	}

      // std::clog << "Size of Coincidence records = " << coincidence_records_.size() << std::endl;
      // for (int i = 0; i < coincidence_records_.size(); i++)
      // 	{	
      // 	  coincidence_records_[i].display();
      // 	}

      return;
    }
    
  } // end of namespace digitization
  
} // end of namespace snemo
