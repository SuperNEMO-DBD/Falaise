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

    coincidence_trigger_algorithm_new_strategy::coincidence_calo_record::coincidence_calo_record()
    {
      coincidence_calo_record::reset();
      return;
    }
    void coincidence_trigger_algorithm_new_strategy::coincidence_calo_record::reset()
    {
      clocktick_1600ns = -1;
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
      calo_finale_decision = false;
      return;
    }

    coincidence_trigger_algorithm_new_strategy::coincidence_output::coincidence_output()
    {
      coincidence_output::reset();
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::coincidence_output::reset()
    {
      clocktick_1600ns = -1;
      zoning_word[0].reset();
      zoning_word[1].reset();	       
      coincidence_finale_decision = false;
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::coincidence_output::display()
    {
      std::clog << "*************************** Clocktick 1600 = " << clocktick_1600ns << " ***************************" << std::endl << std::endl;
      std::clog << "Coincidence zoning word : " << std::endl;
      std::clog << "ZW [0] : " << zoning_word[0] << std::endl;
      std::clog << "ZW [1] : " << zoning_word[1] << std::endl;
      return;
    }
    
    coincidence_trigger_algorithm_new_strategy::previous_event_output::previous_event_output()
    {
      previous_event_output::reset();
      return;
    }
    
    void coincidence_trigger_algorithm_new_strategy::previous_event_output::reset()
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
      for (int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      finale_data_per_zone[iside][izone].reset();
	    }
	}
      return;
    }
    
    void coincidence_trigger_algorithm_new_strategy::previous_event_output::display()
    {
      //todo
      return;
    }

    coincidence_trigger_algorithm_new_strategy::coincidence_trigger_algorithm_new_strategy()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _calorimeter_gate_size_ = 0;
      _coincidence_calo_records_.reserve(SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS);
      _coincidence_decision_ = false;
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
	      coincidence_calo_record.calo_finale_decision = a_ctrec.calo_finale_decision;
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
	      

	      // if (a_ctrec.total_multiplicity_side_0.to_ulong() > coincidence_calo_record.total_multiplicity_side_0.to_ulong()) coincidence_calo_record.total_multiplicity_side_0 = a_ctrec.total_multiplicity_side_0;
	      // if (a_ctrec.total_multiplicity_side_1.to_ulong() > coincidence_calo_record.total_multiplicity_side_1.to_ulong()) coincidence_calo_record.total_multiplicity_side_1 = a_ctrec.total_multiplicity_side_1;
	      // _coincidence_calo_records_[coincidence_calo_record_position].total_multiplicity_side_0.to_ulong() = _coincidence_calo_records_[coincidence_calo_record_position].total_multiplicity_side_0.to_ulong() + a_ctrec.total_multiplicity_side_0.to_ulong(); 
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
         
    void coincidence_trigger_algorithm_new_strategy::_build_previous_prompt_event_record(coincidence_calo_record & a_calo_record_,
											 tracker_trigger_algorithm_test_new_strategy::tracker_record & a_tracker_record_)
    {
      

      return;
    }
    
    void coincidence_trigger_algorithm_new_strategy::process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
							     const std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_,
							     std::vector<coincidence_trigger_algorithm_new_strategy::coincidence_output> & coincidence_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(calo_records_,
	       tracker_records_,
	       coincidence_records_);
      return;
    }

    void coincidence_trigger_algorithm_new_strategy::_process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
							      const std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record> & tracker_records_,
							      std::vector<coincidence_trigger_algorithm_new_strategy::coincidence_output> & coincidence_records_)
    {
      reset_data();
      _preparing_calo_coincidence(calo_records_);
     
      std::vector<coincidence_calo_record>::const_iterator it_calo = _coincidence_calo_records_.begin();
      //      std::clog << "Size of coincidence calo records : " << _coincidence_calo_records_.size() << std::endl;
      for (it_calo; it_calo != _coincidence_calo_records_.end(); it_calo++)
	{
	  coincidence_calo_record a_calo_record = *it_calo;
	  std::vector<tracker_trigger_algorithm_test_new_strategy::tracker_record>::const_iterator it_tracker = tracker_records_.begin();
	  for (it_tracker; it_tracker != tracker_records_.end(); it_tracker++)
	    {
	      tracker_trigger_algorithm_test_new_strategy::tracker_record a_tracker_record = *it_tracker;

	      if (a_calo_record.clocktick_1600ns == a_tracker_record.clocktick_1600ns)
		{
		  coincidence_output a_coincidence_output;
		  a_coincidence_output.clocktick_1600ns = a_calo_record.clocktick_1600ns;

		  for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
		    {
		      for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
			{
			  std::bitset<3> hpattern_for_a_zone = 0x0;

			  int right = 0;
			  int mid   = 1;
			  int left  = 2;
			  
			  hpattern_for_a_zone[0] = a_tracker_record.finale_data_per_zone[iside][izone][2];
			  hpattern_for_a_zone[1] = a_tracker_record.finale_data_per_zone[iside][izone][3];
			  hpattern_for_a_zone[2] = a_tracker_record.finale_data_per_zone[iside][izone][4];
			  if (hpattern_for_a_zone.any())
			    {
			      if (hpattern_for_a_zone.test(mid) && a_calo_record.zoning_word[iside].test(izone) != 0)
				{
				  a_coincidence_output.zoning_word[iside].set(izone, true);
				  a_coincidence_output.coincidence_finale_decision = true;
				  _coincidence_decision_ = true;
				}

			      if ((hpattern_for_a_zone.test(right) && a_calo_record.zoning_word[iside].test(izone) != 0) ||
				  (izone+1 < 10 && hpattern_for_a_zone.test(right) && a_calo_record.zoning_word[iside].test(izone+1) != 0))
				{	  
				  a_coincidence_output.zoning_word[iside].set(izone, true);
				  a_coincidence_output.coincidence_finale_decision = true;
				  _coincidence_decision_ = true;
				}
			      if ((hpattern_for_a_zone.test(left) && a_calo_record.zoning_word[iside].test(izone) != 0) ||
				  (izone-1 > 0 && hpattern_for_a_zone.test(left) && a_calo_record.zoning_word[iside].test(izone-1) != 0))
				{		 
				  a_coincidence_output.zoning_word[iside].set(izone, true);
				  a_coincidence_output.coincidence_finale_decision = true;
				  _coincidence_decision_ = true;
				}			      
			    }
			} // end of izone
		    } // end of iside
		  _build_previous_prompt_event_record(a_calo_record, a_tracker_record);
		  coincidence_records_.push_back(a_coincidence_output);
		} // end of clocktick egality
	    } // end of it_tracker
	} // end of it_calo
      return;
    }
    
  } // end of namespace digitization
  
} // end of namespace snemo
