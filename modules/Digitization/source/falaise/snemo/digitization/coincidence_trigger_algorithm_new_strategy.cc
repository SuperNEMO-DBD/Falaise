// snemo/digitization/coincidence_trigger_algorithm_new_strategy.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm_new_strategy.h>
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    const uint32_t coincidence_trigger_algorithm_new_strategy::SHIFT_COMPUTING_CLOCKTICK_1600NS;
    const uint32_t coincidence_trigger_algorithm_new_strategy::SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS;


    coincidence_trigger_algorithm_new_strategy::coincidence_base_record::coincidence_base_record()
    {
      coincidence_base_record::reset();
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::coincidence_base_record::reset()
    {
      zoning_word[0].reset();
      zoning_word[1].reset();
      total_multiplicity_side_0.reset();
      total_multiplicity_side_1.reset();
      LTO_side_0 = false;
      LTO_side_1 = false;
      total_multiplicity_gveto.reset();
      LTO_gveto = false;
      xt_info_bitset.reset();
      single_side_coinc = false;
      total_multiplicity_threshold = false;
      decision = false;
      return;
    }
    
    const void  coincidence_trigger_algorithm_new_strategy::coincidence_base_record::display() const
    {
      std::clog << "Reducted zoning words : " << std::endl;
      std::clog << "ZW [0] : " << zoning_word[0] << std::endl;
      std::clog << "ZW [1] : " << zoning_word[1] << std::endl;
      std::cout << "XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl; 
      std::cout << xt_info_bitset << ' ';
      std::cout << LTO_gveto << ' ';
      std::cout << total_multiplicity_gveto << ' ';
      std::cout << LTO_side_1 << ' ';
      std::cout << LTO_side_0 << ' ';
      std::cout << total_multiplicity_side_1 << ' ';
      std::cout << total_multiplicity_side_0 << ' ';
      for (int iside = trigger_info::NSIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = trigger_info::NZONES-1; izone >= 0 ; izone--)
      	    {
      	      std::cout << calo_zoning_word[iside][izone];
      	    }
      	  std::cout << ' ';
      	}      
      std::cout << std::endl;
      std::cout << "Single Side coinc : " << single_side_coinc 
		<< "  |  Threshold total mult : "   << total_multiplicity_threshold << std::endl;
      std::clog << std::endl;

     
      return;
    }
    
    coincidence_trigger_algorithm_new_strategy::coincidence_calo_record::coincidence_calo_record()
    {
      coincidence_calo_record::reset();
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::coincidence_calo_record::reset()
    {
      coincidence_base_record::reset();
      clocktick_1600ns = -1;
      return;
    }
    
    const void coincidence_trigger_algorithm_new_strategy::coincidence_calo_record::display() const
    {
      std::cout << "************************************************************************************" << std::endl;
      std::cout << "*************************** Coincidence calo record ********************" << std::endl;
      std::clog << "*************************** Clocktick 1600 = " << clocktick_1600ns << " ***************************" << std::endl;
      coincidence_base_record::display();
      std::cout << "Coincidence calo record decision : [" << decision << "]" << std::endl;
      return;
    }

    coincidence_trigger_algorithm_new_strategy::coincidence_event_record::coincidence_event_record()
    {
      coincidence_event_record::reset();
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::coincidence_event_record::reset()
    {
      coincidence_base_record::reset();
      clocktick_1600ns = -1;
      is_delayed = false;
      for (int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      tracker_finale_data_per_zone[iside][izone].reset();
	    }
	}	       
      return;
    }

    const void coincidence_trigger_algorithm_new_strategy::coincidence_event_record::display() const
    {
      std::cout << "************************************************************************************" << std::endl;
      std::cout << "*************************** Coincidence event record ********************" << std::endl;
      std::clog << "*************************** Clocktick 1600 = " << clocktick_1600ns << " ***************************" << std::endl;
      coincidence_base_record::display();
      std::cout << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
      for (int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::cout << "Side = " << iside << " | ";
	  for (int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      std::cout << "[" << tracker_finale_data_per_zone[iside][izone] << "] ";
	    } // end of izone
	  std::cout << std::endl;
	}	
      std::cout << "Coincidence event record decision : [" << decision << "]" << std::endl;
      return;
    }
    
    coincidence_trigger_algorithm_new_strategy::previous_event_record::previous_event_record()
    {
      previous_event_record::reset();
      return;
    }
    
    void coincidence_trigger_algorithm_new_strategy::previous_event_record::reset()
    {
      counter_1600ns = 0;
      load_bit = true;
      for (int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      tracker_finale_data_per_zone[iside][izone].reset();
	    }
	}	       
      return;
    }
    
    const void coincidence_trigger_algorithm_new_strategy::previous_event_record::display() const
    {
      std::cout << "************************************************************************************" << std::endl;
      std::cout << "*************************** Previous event record ********************" << std::endl;
      std::clog << "*************************** Counter 1600 = " << counter_1600ns << " ***************************" << std::endl;
      coincidence_base_record::display();
      std::cout << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
      for (int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::cout << "Side = " << iside << " | ";
	  for (int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      std::cout << "[" << tracker_finale_data_per_zone[iside][izone] << "] ";
	    } // end of izone
	  std::cout << std::endl;
	}	
      std::cout << "Previous event record decision : [" << decision << "]" << std::endl;
      std::clog << std::endl;
      return;
    }

    coincidence_trigger_algorithm_new_strategy::coincidence_trigger_algorithm_new_strategy()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _calorimeter_gate_size_ = 0;
      _coincidence_calo_records_.reserve(SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS);
      _coincidence_decision_ = false;
      _previous_event_record_.reset();
      return;
    }

    coincidence_trigger_algorithm_new_strategy::~coincidence_trigger_algorithm_new_strategy()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    bool coincidence_trigger_algorithm_new_strategy::has_calorimeter_gate_size() const
    {
      return _calorimeter_gate_size_ != 0;
    }

    void coincidence_trigger_algorithm_new_strategy::set_calorimeter_gate_size(unsigned int calorimeter_gate_size_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo circular buffer depth can't be set ! ");
      _calorimeter_gate_size_ = calorimeter_gate_size_;
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized ! ");
      if (! has_calorimeter_gate_size()) {
	if (config_.has_key("calorimeter_gate_size")) {
	  int calorimeter_gate_size = config_.fetch_integer("calorimeter_gate_size");
	  DT_THROW_IF(calorimeter_gate_size <= 0, std::domain_error, "Invalid value calorimeter_gate_size !");
	  set_calorimeter_gate_size((unsigned int) calorimeter_gate_size);
	}
      }
      _initialized_ = true;
      return;
    }

    bool coincidence_trigger_algorithm_new_strategy::is_initialized() const
    {
      return _initialized_;
    }

    void coincidence_trigger_algorithm_new_strategy::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _electronic_mapping_ = 0;
      reset_data();
      _initialized_ = false;
      return;
    }
    
    void coincidence_trigger_algorithm_new_strategy::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _coincidence_calo_records_.clear();
      _coincidence_decision_ = false;
      _previous_event_record_.reset();
      return;
    }

    const std::vector<coincidence_trigger_algorithm_new_strategy::coincidence_calo_record> coincidence_trigger_algorithm_new_strategy::get_coincidence_calo_records_vector() const
    {
      return _coincidence_calo_records_;
    }

    const bool coincidence_trigger_algorithm_new_strategy::get_coincidence_decision() const
    {
      return _coincidence_decision_;
    }

    void coincidence_trigger_algorithm_new_strategy::_preparing_calo_coincidence(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_)
    {     
      std::vector<calo_trigger_algorithm::calo_summary_record>::const_iterator it = calo_records_.begin();
      bool decision_already_true = false;
      bool calo_coinc_decision = false;
      coincidence_trigger_algorithm_new_strategy::coincidence_calo_record coincidence_calo_record;
      unsigned int clocktick_25ns_decision = 0;
      unsigned int coincidence_calo_record_position = 0;
      unsigned int max_mult_side_0 = 0;
      unsigned int max_mult_side_1 = 0;
      unsigned int max_mult_gveto  = 0;
      
      for (it; it != calo_records_.end(); it++)
      	{
	  const calo_trigger_algorithm::calo_summary_record a_ctrec= *it;
	  unsigned int a_ctrec_clocktick_1600ns = 0;
	  _compute_clocktick_1600ns(a_ctrec.clocktick_25ns, a_ctrec_clocktick_1600ns);
	  a_ctrec_clocktick_1600ns += SHIFT_COMPUTING_CLOCKTICK_1600NS;
	  if (a_ctrec.calo_finale_decision == true && decision_already_true == false)
	    {
	      clocktick_25ns_decision = a_ctrec.clocktick_25ns;
	      coincidence_calo_record.clocktick_1600ns = a_ctrec_clocktick_1600ns;
	      coincidence_calo_record.zoning_word[0] = a_ctrec.zoning_word[0];
	      coincidence_calo_record.zoning_word[1] = a_ctrec.zoning_word[1];
	      coincidence_calo_record.total_multiplicity_side_0 = a_ctrec.total_multiplicity_side_0;
	      coincidence_calo_record.total_multiplicity_side_1 = a_ctrec.total_multiplicity_side_1;
	      coincidence_calo_record.LTO_side_0 = a_ctrec.LTO_side_0;
	      coincidence_calo_record.LTO_side_1 = a_ctrec.LTO_side_1;
	      coincidence_calo_record.total_multiplicity_gveto = a_ctrec.total_multiplicity_gveto;
	      coincidence_calo_record.LTO_gveto = a_ctrec.LTO_gveto;
	      coincidence_calo_record.xt_info_bitset = a_ctrec.xt_info_bitset;
	      coincidence_calo_record.single_side_coinc = a_ctrec.single_side_coinc;
	      coincidence_calo_record.total_multiplicity_threshold = a_ctrec.total_multiplicity_threshold;
	      coincidence_calo_record.decision = a_ctrec.calo_finale_decision;
	      _coincidence_calo_records_.push_back(coincidence_calo_record);
	      decision_already_true = true;
	      calo_coinc_decision = true;
	      max_mult_side_0 = a_ctrec.total_multiplicity_side_0.to_ulong();
	      max_mult_side_1 = a_ctrec.total_multiplicity_side_1.to_ulong();
	      max_mult_gveto  = a_ctrec.total_multiplicity_gveto.to_ulong();
	    }
	  
	  if (decision_already_true == true  
	      && a_ctrec_clocktick_1600ns == _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns
	      && a_ctrec.clocktick_25ns <= clocktick_25ns_decision + 3) // 3 == calo circular buffer depth
	    {
	      _coincidence_calo_records_[coincidence_calo_record_position].zoning_word[0] = a_ctrec.zoning_word[0];
	      _coincidence_calo_records_[coincidence_calo_record_position].zoning_word[1] = a_ctrec.zoning_word[1];
	      if (a_ctrec.total_multiplicity_side_0.to_ulong() > max_mult_side_0) max_mult_side_0 = a_ctrec.total_multiplicity_side_0.to_ulong();
	      if (a_ctrec.total_multiplicity_side_1.to_ulong() > max_mult_side_1) max_mult_side_1 = a_ctrec.total_multiplicity_side_1.to_ulong();
	      if (a_ctrec.total_multiplicity_gveto.to_ulong()  > max_mult_gveto)  max_mult_gveto = a_ctrec.total_multiplicity_gveto.to_ulong();
	      _coincidence_calo_records_[coincidence_calo_record_position].total_multiplicity_side_0 = max_mult_side_0;
	      _coincidence_calo_records_[coincidence_calo_record_position].total_multiplicity_side_1 = max_mult_side_1;
	      _coincidence_calo_records_[coincidence_calo_record_position].total_multiplicity_gveto = max_mult_gveto;
	    }
	  
	  else if (decision_already_true == true
		   && a_ctrec_clocktick_1600ns != _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns
		   && a_ctrec.clocktick_25ns <= clocktick_25ns_decision + 3) // 3 == calo circular buffer depth
	    {
	      coincidence_trigger_algorithm_new_strategy::coincidence_calo_record coincidence_calo_record_next_clocktick;
	      coincidence_calo_record_next_clocktick = coincidence_calo_record;
	      coincidence_calo_record_next_clocktick.total_multiplicity_side_0 = max_mult_side_0;
	      coincidence_calo_record_next_clocktick.total_multiplicity_side_1 = max_mult_side_1;
	      coincidence_calo_record_next_clocktick.total_multiplicity_gveto  = max_mult_gveto;

	      coincidence_calo_record_next_clocktick.clocktick_1600ns = coincidence_calo_record.clocktick_1600ns + 1;
	      _coincidence_calo_records_.push_back(coincidence_calo_record_next_clocktick);
	      coincidence_calo_record_position++;
	    }
	}
      
      if (decision_already_true == true)
       	{
	  for (int iclocktick = _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns + 1; iclocktick <= coincidence_calo_record.clocktick_1600ns + _calorimeter_gate_size_; iclocktick++)
	    {
      	      coincidence_trigger_algorithm_new_strategy::coincidence_calo_record a_coinc_calo_record;
      	      a_coinc_calo_record = _coincidence_calo_records_[coincidence_calo_record_position];	      
      	      a_coinc_calo_record.clocktick_1600ns = iclocktick;
      	      _coincidence_calo_records_.push_back(a_coinc_calo_record);
	    }
      	}
      
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::_compute_clocktick_1600ns(const uint32_t clocktick_25ns_, uint32_t & clocktick_1600ns_)
    {
      clocktick_1600ns_ = (clocktick_25ns_ * clock_utils::MAIN_CLOCKTICK) / clock_utils::TRIGGER_CLOCKTICK;
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::_process_calo_tracker_coincidence(coincidence_trigger_algorithm_new_strategy::coincidence_calo_record & a_calo_record_,
										       tracker_trigger_algorithm_test_new_strategy::tracker_record & a_tracker_record_,
										       coincidence_trigger_algorithm_new_strategy::coincidence_event_record & a_coincidence_record_)

    { 
      a_coincidence_record_.clocktick_1600ns = a_calo_record_.clocktick_1600ns;
      
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
	    {
	      std::bitset<3> hpattern_for_a_zone = 0x0;

	      int right = 0;
	      int mid   = 1;
	      int left  = 2;
			  
	      hpattern_for_a_zone[right] = a_tracker_record_.finale_data_per_zone[iside][izone][right+2];
	      hpattern_for_a_zone[mid] = a_tracker_record_.finale_data_per_zone[iside][izone][mid+2];
	      hpattern_for_a_zone[left] = a_tracker_record_.finale_data_per_zone[iside][izone][left+2];
	      if (hpattern_for_a_zone.any())
		{
		  if (hpattern_for_a_zone.test(mid) && a_calo_record_.zoning_word[iside].test(izone) != 0)
		    {
		      a_coincidence_record_.zoning_word[iside].set(izone, true);
		      a_coincidence_record_.decision = true;
		      _coincidence_decision_ = true;
		    }

		  if ((hpattern_for_a_zone.test(right) && a_calo_record_.zoning_word[iside].test(izone) != 0) ||
		      (izone+1 < 10 && hpattern_for_a_zone.test(right) && a_calo_record_.zoning_word[iside].test(izone+1) != 0))
		    {	  
		      a_coincidence_record_.zoning_word[iside].set(izone, true);
		      a_coincidence_record_.decision = true;
		      _coincidence_decision_ = true;
		    }
			      
		  if ((hpattern_for_a_zone.test(left) && a_calo_record_.zoning_word[iside].test(izone) != 0) ||
		      (izone-1 > -1 && hpattern_for_a_zone.test(left) && a_calo_record_.zoning_word[iside].test(izone-1) != 0))
		    {		 
		      a_coincidence_record_.zoning_word[iside].set(izone, true);
		      a_coincidence_record_.decision = true;
		      _coincidence_decision_ = true;
		    }			  
    
		} // end of hpattern.any()
	    } // end of izone
	} // end of iside
      if (a_coincidence_record_.decision) a_coincidence_record_.is_delayed = false;
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::_build_previous_event_record(coincidence_trigger_algorithm_new_strategy::coincidence_event_record & a_coincidence_record_)    
    {
      unsigned int max_mult_side_0 = _previous_event_record_.total_multiplicity_side_0.to_ulong();
      unsigned int max_mult_side_1 = _previous_event_record_.total_multiplicity_side_1.to_ulong();
      unsigned int max_mult_gveto  = _previous_event_record_.total_multiplicity_gveto.to_ulong();

      _previous_event_record_.counter_1600ns = 625;
      _previous_event_record_.load_bit = false; // Can't erase this structure (but it is possible to update the structure)

      if (a_coincidence_record_.total_multiplicity_side_0.to_ulong() > max_mult_side_0) _previous_event_record_.total_multiplicity_side_0 = a_coincidence_record_.total_multiplicity_side_0;
      if (a_coincidence_record_.total_multiplicity_side_1.to_ulong() > max_mult_side_1) _previous_event_record_.total_multiplicity_side_1 = a_coincidence_record_.total_multiplicity_side_1;

      if (a_coincidence_record_.LTO_side_0) _previous_event_record_.LTO_side_0 = true;
      if (a_coincidence_record_.LTO_side_1) _previous_event_record_.LTO_side_1 = true;

      if (a_coincidence_record_.total_multiplicity_gveto.to_ulong() > max_mult_gveto) _previous_event_record_.total_multiplicity_gveto = a_coincidence_record_.total_multiplicity_gveto;

      if (a_coincidence_record_.LTO_gveto)  _previous_event_record_.LTO_gveto  = true;

      for (int ibit = 0; ibit < calo_trigger_algorithm::XT_INFO_BITSET_SIZE; ibit ++)
	{
	  if (a_coincidence_record_.xt_info_bitset.test(ibit)) _previous_event_record_.xt_info_bitset.set(ibit);
	}
      if (a_coincidence_record_.single_side_coinc) _previous_event_record_.single_side_coinc = true;
      if (a_coincidence_record_.total_multiplicity_threshold) _previous_event_record_.total_multiplicity_threshold = true;

      for (int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  for (int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      if (a_coincidence_record_.zoning_word[iside].test(izone)) _previous_event_record_.calo_zoning_word[iside].set(izone, true);
	      for (int ibit = 0; ibit < trigger_info::DATA_FULL_BITSET_SIZE; ibit ++)
		{
		  if (a_coincidence_record_.tracker_finale_data_per_zone[iside][izone].test(ibit)) _previous_event_record_.tracker_finale_data_per_zone[iside][izone].set(ibit);
		}
	    }
	}
      
      return;
    }
    
    void coincidence_trigger_algorithm_new_strategy::_process_delayed_coincidence(coincidence_trigger_algorithm_new_strategy::coincidence_calo_record & a_calo_record_,
										  tracker_trigger_algorithm_test_new_strategy::tracker_record & a_tracker_record_, 
										  coincidence_trigger_algorithm_new_strategy::coincidence_event_record & a_delayed_record_)
    {

      // to do 

      
      
      
      
      


      // // Cut on CT, search for delayed alpha on calo records and tracker records
      // std::size_t ct_begin = tracker_records_.front().clocktick_1600ns;
      // std::size_t ct_end   = tracker_records_.back().clocktick_1600ns;
      // std::size_t tracker_size = tracker_records_.size();

      
      // std::size_t delayed_ct_begin = 0;
      // std::size_t delayed_tracker_position = 0;
      // std::size_t ct_before = ct_begin;
      
      // for (int i = 0; i < tracker_size; i++)
      // 	{
      // 	  std::size_t actual_clocktick = tracker_records_[i].clocktick_1600ns;
      // 	  if (actual_clocktick  == ct_before + 1 || actual_clocktick == ct_before) ct_before = actual_clocktick;
      // 	  else 
      // 	    {
      // 	      delayed_ct_begin = actual_clocktick;
      // 	      delayed_tracker_position = i;
      // 	      break;
      // 	    }
      // 	}
     
      // std::clog << "CT begin = " << ct_begin << std::endl;
      // std::clog	<< "CT end  = " << ct_end << std::endl;
      // std::clog << "Size    = " << tracker_size << std::endl;
      
      // if (delayed_ct_begin != 0 && _previous_event_record_.counter_1600ns != 0) // counter != 0 ->   > 1ms (625 * 1600ns) 
      // 	{
      // 	  std::clog << "Delayed CT begin = " << delayed_ct_begin << std::endl;
      // 	  std::clog << "Delayed Tracker pos = " << delayed_tracker_position << std::endl;
	  
      // 	  // Just delayed tracker records for coincidence with previous structure :
      // 	  std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record>::const_iterator it_tracker = tracker_records_.begin() + delayed_tracker_position;
	 
      // 	  for (it_tracker; it_tracker != tracker_records_.end(); it_tracker++)
      // 	    {
      // 	      delayed_event_record a_delayed_record;
      // 	      tracker_trigger_algorithm_test_new_strategy::tracker_record a_delayed_tracker_record = *it_tracker;
      // 	      std::clog << "CT tracker record = " << a_delayed_tracker_record.clocktick_1600ns << std::endl;
	      
      // 	      for (int iside = 0; iside < trigger_info::NSIDES; iside++)
      // 		{
      // 		  for (int izone = 0; izone < trigger_info::NZONES; izone++)
      // 		    {
      // 		      std::bitset<3> delayed_hpattern_per_zone = 0x0;

      // 		      int right = 0;
      // 		      int mid   = 1;
      // 		      int left  = 2;
			  
      // 		      delayed_hpattern_per_zone[right] = a_delayed_tracker_record.finale_data_per_zone[iside][izone][right+2]; // right + 2 -> bit position to change (bit enum in trigger_info.h)
      // 		      delayed_hpattern_per_zone[mid]   = a_delayed_tracker_record.finale_data_per_zone[iside][izone][mid+2];
      // 		      delayed_hpattern_per_zone[left]  = a_delayed_tracker_record.finale_data_per_zone[iside][izone][left+2];
		      
      // 		      if (delayed_hpattern_per_zone.any())
      // 			{			  
      // 			  if (delayed_hpattern_per_zone.test(left) && izone-1 > -1 && (_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(right+2)
      // 										       || _previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
      // 										       || _previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 										       || _previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(right+2)
      // 										       || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
      // 										       || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
      // 			    {
      // 			      a_delayed_record.decision = true;
      // 			      a_delayed_record.tracker_finale_data_per_zone[iside][izone] = a_delayed_tracker_record.finale_data_per_zone[iside][izone];
      // 			      //a_delayed_record.display();
      // 			    }    

      // 			  if (delayed_hpattern_per_zone.test(mid) && (_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
      // 								      || _previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 								      || _previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
      // 								      || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
      // 								      || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
      // 								      || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
      // 			    {
      // 			      a_delayed_record.decision = true;
      // 			      a_delayed_record.tracker_finale_data_per_zone[iside][izone] = a_delayed_tracker_record.finale_data_per_zone[iside][izone];
      // 			    } 

      // 			  if (delayed_hpattern_per_zone.test(right) && izone + 1 < 10 && (_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
      // 											  || _previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
      // 											  || _previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(left+2)
      // 											  || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
      // 											  || _previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)
      // 											  || _previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(left+2)))
      // 			    {
      // 			      a_delayed_record.decision = true;
      // 			      a_delayed_record.tracker_finale_data_per_zone[iside][izone] = a_delayed_tracker_record.finale_data_per_zone[iside][izone];
      // 			      // a_delayed_record.display();
      // 			    }
      // 			}
      // 		    } // end of izone
		  
      // 		} // end of iside
	      
      // 	      // Fill only if there is information
      // 	      _delayed_event_record_ = a_delayed_record;
	      
      // 	    } // end of it_tracker
      // 	} // end of delayed ct
      
      
      
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
							     const std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_,
							     std::vector<coincidence_trigger_algorithm_new_strategy::coincidence_event_record> & coincidence_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(calo_records_,
	       tracker_records_,
	       coincidence_records_);
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::_process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
							      const std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_,
							      std::vector<coincidence_trigger_algorithm_new_strategy::coincidence_event_record> & coincidence_records_)
    {
      reset_data();
      _preparing_calo_coincidence(calo_records_);
      
      std::vector<coincidence_calo_record>::const_iterator it_calo = _coincidence_calo_records_.begin();
      for (it_calo; it_calo != _coincidence_calo_records_.end(); it_calo++)
	{
	  coincidence_calo_record a_calo_record = *it_calo;
	  std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record>::const_iterator it_tracker = tracker_records_.begin();
	  for (it_tracker; it_tracker != tracker_records_.end(); it_tracker++)
	    {
	      tracker_trigger_algorithm_test_new_strategy::tracker_record a_tracker_record = *it_tracker;

	      if (a_calo_record.clocktick_1600ns == a_tracker_record.clocktick_1600ns)
		{
		  coincidence_event_record a_coincidence_record;
		  _process_calo_tracker_coincidence(a_calo_record, a_tracker_record, a_coincidence_record);

		  if (a_coincidence_record.decision)
		    {
		      coincidence_records_.push_back(a_coincidence_record);
		      _build_previous_event_record(a_coincidence_record); // fill the token load bit
		    }
		  else // problem with load bit coincidence atm
		    {
		      coincidence_event_record a_delayed_event_record;
		      _process_delayed_coincidence(a_calo_record, a_tracker_record, a_delayed_event_record); // Compare calo record and tracker record with previous event
		      if (a_delayed_event_record.decision && a_delayed_event_record.is_delayed) 
			{
			  coincidence_records_.push_back(a_delayed_event_record);
			}
		    }
		  
		} // end of clocktick egality
	    } // end of it_tracker
	} // end of it_calo
      
      std::clog << "Coinc record size = " << coincidence_records_.size() << std::endl;
      
      return;
    }
    
  } // end of namespace digitization
  
} // end of namespace snemo
