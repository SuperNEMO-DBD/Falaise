// snemo/digitization/trigger_algorithm_test_time.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_algorithm_test_time.h>
#include <snemo/digitization/mapping.h>


namespace snemo {
  
  namespace digitization {

    trigger_algorithm_test_time::trigger_algorithm_test_time()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _coincidence_calorimeter_gate_size_ = 0;
      _L2_decision_coincidence_gate_size_ = 0;
      _previous_event_circular_buffer_depth_ = 0;
      _activate_calorimeter_only_ = false;
      _activate_calo_tracker_time_coincidence_ = false;
      _activate_caraco_ = false;
      _activate_take_all_delayed_ = false;
      _activate_ape_dave_coincidence_ = false;
      _activate_ape_coincidence_only_ = false;
      _activate_any_coincidences_ = false;
      _calo_records_25ns_.clear();
      _coincidence_calo_records_1600ns_.clear();
      _tracker_records_.clear();
      _geiger_matrix_records_.clear();
      _pair_records_.clear();
      _coincidence_records_.clear();
      _previous_event_records_.reset();
      _L1_calo_decision_records_.clear();
      _L2_decision_records_.clear();
      _finale_trigger_decision_ = false;
      return;
    }

    trigger_algorithm_test_time::~trigger_algorithm_test_time()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void trigger_algorithm_test_time::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _coincidence_calorimeter_gate_size_ = 0;
      _L2_decision_coincidence_gate_size_ = 0;
      _previous_event_circular_buffer_depth_ = 0;
      _activate_calorimeter_only_ = false;
      _activate_calo_tracker_time_coincidence_ = false;
      _activate_caraco_ = false;
      _activate_take_all_delayed_ = false;
      _activate_ape_dave_coincidence_ = false;
      _activate_ape_coincidence_only_ = false;
      _activate_any_coincidences_ = false;
      _tracker_algo_.reset();
      _calo_algo_.reset();
      _coinc_algo_.reset();
      _calo_records_25ns_.clear();
      _coincidence_calo_records_1600ns_.clear();
      _tracker_records_.clear();
      _geiger_matrix_records_.clear();
      _pair_records_.clear();
      _coincidence_records_.clear();
      _previous_event_records_.reset();
      _L1_calo_decision_records_.clear();
      _L2_decision_records_.clear();
      _finale_trigger_decision_ = false;
      return;
    }  
    
    void trigger_algorithm_test_time::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't be reset ! ");
      _calo_records_25ns_.clear();
      _coincidence_calo_records_1600ns_.clear();
      _tracker_records_.clear();
      _geiger_matrix_records_.clear();
      _pair_records_.clear();
      _coincidence_records_.clear();
      _previous_event_records_->clear();
      _L1_calo_decision_records_.clear();
      _L2_decision_records_.clear();
      _finale_trigger_decision_ = false;
      return;
    }
    
    void trigger_algorithm_test_time::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      _tracker_algo_.set_electronic_mapping(my_electronic_mapping_);
      return;
    }  

    void trigger_algorithm_test_time::set_clock_manager(const clock_utils & my_clock_manager_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized, clock manager can't be set ! ");
      _clock_manager_ = & my_clock_manager_;
      return;
    }  

    bool trigger_algorithm_test_time::has_calorimeter_gate_size() const
    {
      return _coincidence_calorimeter_gate_size_ != 0;
    }

    void trigger_algorithm_test_time::set_calorimeter_gate_size(unsigned int calorimeter_gate_size_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized, coincidence calorimeter gate size can't be set ! ");
      _coincidence_calorimeter_gate_size_ = calorimeter_gate_size_;
      return;
    }

    bool trigger_algorithm_test_time::has_L2_decision_coincidence_gate_size() const
    {
      return _L2_decision_coincidence_gate_size_ != 0;
    }

    void trigger_algorithm_test_time::set_L2_decision_coincidence_gate_size(unsigned int L2_decision_coincidence_gate_size_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized, coincidence calorimeter gate size can't be set ! ");
      _L2_decision_coincidence_gate_size_ = L2_decision_coincidence_gate_size_;
      return;
    }

    bool trigger_algorithm_test_time::has_previous_event_buffer_depth() const
    {
      return _previous_event_circular_buffer_depth_ != 0;
    }

    void trigger_algorithm_test_time::set_previous_event_buffer_depth(unsigned int previous_event_circular_buffer_depth_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized, previous event buffer depth can't be set ! ");
      _previous_event_circular_buffer_depth_ = previous_event_circular_buffer_depth_;
      return;
    }

    bool trigger_algorithm_test_time::is_activated_coincidence() const
    {
      return _activate_any_coincidences_;
    }

    void trigger_algorithm_test_time::initialize()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void trigger_algorithm_test_time::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized ! ");
      DT_THROW_IF(_electronic_mapping_ == 0, std::logic_error, "Missing electronic mapping ! " );
      DT_THROW_IF(_clock_manager_ == 0, std::logic_error, "Missing clock manager ! " );

      datatools::properties calo_config;
      config_.export_and_rename_starting_with(calo_config, "calo.", "");
      _calo_algo_.initialize(calo_config);

      datatools::properties tracker_config;
      config_.export_and_rename_starting_with(tracker_config, "tracker.", "");
      _tracker_algo_.initialize(tracker_config);

      datatools::properties coinc_config;
      config_.export_and_rename_starting_with(coinc_config, "coincidence.", "");
      _coinc_algo_.initialize(coinc_config);
       
      if (!has_calorimeter_gate_size()) {
	if (config_.has_key("coincidence_calorimeter_gate_size")) {
	  int coincidence_calorimeter_gate_size = config_.fetch_integer("coincidence_calorimeter_gate_size");
	  DT_THROW_IF(coincidence_calorimeter_gate_size <= 0, std::domain_error, "Invalid value of coincidence_calorimeter_gate_size !");
	  set_calorimeter_gate_size((unsigned int) coincidence_calorimeter_gate_size);
	}
      }

      if (!has_L2_decision_coincidence_gate_size()) {
	if (config_.has_key("L2_decision_coincidence_gate_size")) {
	  int L2_decision_coincidence_gate_size = config_.fetch_integer("L2_decision_coincidence_gate_size");
	  DT_THROW_IF(L2_decision_coincidence_gate_size <= 0, std::domain_error, "Invalid value of L2_decision_coincidence_gate_size !");
	  set_L2_decision_coincidence_gate_size((unsigned int) L2_decision_coincidence_gate_size);
	}
      }
    
      if (!has_previous_event_buffer_depth()) {
	if (config_.has_key("previous_event_buffer_depth")) {
	  int previous_event_buffer_depth = config_.fetch_integer("previous_event_buffer_depth");
	  DT_THROW_IF(previous_event_buffer_depth <= 0, std::domain_error, "Invalid negative previous event buffer depth!");
	  set_previous_event_buffer_depth((unsigned int) previous_event_buffer_depth);
	}
      }

      // Check config dependencies (calo only is not compatible with coincidence config for example

      if (!is_activated_coincidence()) {
	if(config_.has_key("activate_calorimeter_only")) {
	  bool activate_calorimeter_only_config =  config_.fetch_boolean("activate_calorimeter_only");
	  _activate_calorimeter_only_ = activate_calorimeter_only_config;
	}
      }

      if (!is_activated_coincidence()) {
	if(config_.has_key("activate_any_coincidences")) {
	  bool activate_any_coincidences_config =  config_.fetch_boolean("activate_any_coincidences");
	  _activate_any_coincidences_ = activate_any_coincidences_config;
	}
      }
        
      _initialized_ = true;
      return;
    }

    bool trigger_algorithm_test_time::is_initialized() const
    {
      return _initialized_;
    }
  
    
    const std::vector<trigger_structures::calo_summary_record> trigger_algorithm_test_time::get_calo_records_25ns_vector() const  
    {
      return _calo_records_25ns_;
    }	

    const std::vector<trigger_structures::coincidence_calo_record> trigger_algorithm_test_time::get_coincidence_calo_records_1600ns_vector() const
    {
      return _coincidence_calo_records_1600ns_;
    }
    
    const std::vector<trigger_structures::tracker_record>trigger_algorithm_test_time::get_tracker_records_vector() const
    {
      return _tracker_records_;
    }  

    const std::vector<trigger_structures::coincidence_event_record> trigger_algorithm_test_time::get_coincidence_records_vector() const
    {
      return _coincidence_records_;
    }

    const std::vector<trigger_structures::L1_calo_decision> trigger_algorithm_test_time::get_L1_calo_decision_records_vector() const
    {
      return _L1_calo_decision_records_;
    }

    const std::vector<trigger_structures::L2_decision> trigger_algorithm_test_time::get_L2_decision_records_vector() const
    {
      return _L2_decision_records_;
    }
        
    bool trigger_algorithm_test_time::get_finale_decision() const
    {
      return _finale_trigger_decision_;
    }   

    void  trigger_algorithm_test_time::_rescale_calo_records_at_1600ns(const std::vector<trigger_structures::calo_summary_record> & calo_records_25_ns_,
								       std::vector<trigger_structures::coincidence_calo_record> & coincidence_calo_records_1600ns_)
    {
      std::vector<trigger_structures::calo_summary_record>::const_iterator it = calo_records_25_ns_.begin();
      for (; it != calo_records_25_ns_.end(); it++)
      	{
      	  const trigger_structures::calo_summary_record a_ctrec= *it;

      	  if (a_ctrec.calo_finale_decision == true)
      	    {
	      uint32_t ctrec_clocktick_1600ns = 0;
      	      _clock_manager_->compute_clocktick_25ns_to_1600ns(a_ctrec.clocktick_25ns, ctrec_clocktick_1600ns);

      	      if (coincidence_calo_records_1600ns_.size() == 0)
      		{
      		  // No coincidence calo records, creation of the first and the 10 following EMPTY 
      		  trigger_structures::coincidence_calo_record first_coincidence_calo_record;
      		  first_coincidence_calo_record.clocktick_1600ns = ctrec_clocktick_1600ns;
      		  coincidence_calo_records_1600ns_.push_back(first_coincidence_calo_record);
		    
      		  if (coincidence_calo_records_1600ns_.size() != 0)
      		    {
      		      for (unsigned int iclocktick = first_coincidence_calo_record.clocktick_1600ns + 1; iclocktick < first_coincidence_calo_record.clocktick_1600ns + _coincidence_calorimeter_gate_size_; iclocktick ++)
      			{
      			  trigger_structures::coincidence_calo_record on_gate_coincidence_calo_record;
      			  on_gate_coincidence_calo_record = first_coincidence_calo_record;
      			  on_gate_coincidence_calo_record.clocktick_1600ns = iclocktick;
      			  coincidence_calo_records_1600ns_.push_back(on_gate_coincidence_calo_record);		      
      			}
      		    }
      		}
	  
      	      bool coinc_calo_record_find = false;

      	      // Search if a coincidence calo record for clocktick ctrec 25 -> 1600 is already existing
      	      for (unsigned int i = 0; i < coincidence_calo_records_1600ns_.size(); i++)
      		{
      		  if (ctrec_clocktick_1600ns == coincidence_calo_records_1600ns_[i].clocktick_1600ns)
      		    {
      		      // Updating existing coincidence calo record
      		      coinc_calo_record_find = true;
      		      _update_coinc_calo_record(a_ctrec, coincidence_calo_records_1600ns_[i]);

      		      // Updating the following coincidence calo record
      		      // Have to check if they are in the gate or outside
      		      if (i != coincidence_calo_records_1600ns_.size() - 1)
      			{
      			  for (unsigned int j = i+1; j < coincidence_calo_records_1600ns_.size(); j++)
      			    {
      			      if (coincidence_calo_records_1600ns_[j].clocktick_1600ns <= coincidence_calo_records_1600ns_[i].clocktick_1600ns + _coincidence_calorimeter_gate_size_) 
      				{
				  uint32_t clocktick_1600_before_modification = coincidence_calo_records_1600ns_[j].clocktick_1600ns;
      				  coincidence_calo_records_1600ns_[j] = coincidence_calo_records_1600ns_[i];
      				  coincidence_calo_records_1600ns_[j].clocktick_1600ns = clocktick_1600_before_modification;
      				}
      			    } // end of for j
      			}

      		      // Have to grow back the gate and create new coincidence calo record
      		      unsigned int actual_index = i;
		      unsigned int index_min = std::floor(actual_index /  _coincidence_calorimeter_gate_size_) * _coincidence_calorimeter_gate_size_;
      		      unsigned int index_max = index_min + _coincidence_calorimeter_gate_size_ - 1;
		      uint32_t clocktick_to_create = ctrec_clocktick_1600ns + _coincidence_calorimeter_gate_size_;	     
		      uint32_t clocktick_max_1600 = coincidence_calo_records_1600ns_[index_max].clocktick_1600ns;
		      
      		      for (unsigned int j = clocktick_max_1600 + 1; j <= clocktick_to_create; j++)
      			{
      			  bool no_same_clocktick = true;
      			  for (unsigned int k = actual_index + 1 ; k < coincidence_calo_records_1600ns_.size(); k++)
      			    {   
      			      // Check if the CT i between max and wanted to create exist in the coinc calo record list :
      			      if (j == coincidence_calo_records_1600ns_[k].clocktick_1600ns)
      				{
      				  no_same_clocktick = false;
      				}
      			    } // end of for k
			  
      			  if (no_same_clocktick == true)
      			    {
      			      trigger_structures::coincidence_calo_record grow_back_coincidence_calo_record = coincidence_calo_records_1600ns_[index_max];
      			      grow_back_coincidence_calo_record.clocktick_1600ns = j;
      			      coincidence_calo_records_1600ns_.push_back(grow_back_coincidence_calo_record);
      			    }
      			} // end of for j
		      		      
      		    }
      		} // end of for i 
	      
      	      // If not, create the first and the 10 following empty then updated
      	      if (coinc_calo_record_find == false)
      		{
      		  trigger_structures::coincidence_calo_record new_coincidence_calo_record;
      		  new_coincidence_calo_record.clocktick_1600ns = ctrec_clocktick_1600ns;
      		  coincidence_calo_records_1600ns_.push_back(new_coincidence_calo_record);
      		  for (unsigned int iclocktick = new_coincidence_calo_record.clocktick_1600ns + 1; iclocktick < new_coincidence_calo_record.clocktick_1600ns + _coincidence_calorimeter_gate_size_; iclocktick ++)
      		    {
      		      trigger_structures::coincidence_calo_record new_on_gate_coincidence_calo_record;
      		      new_on_gate_coincidence_calo_record = new_coincidence_calo_record;
      		      new_on_gate_coincidence_calo_record.clocktick_1600ns = iclocktick;
      		      coincidence_calo_records_1600ns_.push_back(new_on_gate_coincidence_calo_record);		      
      		    }
      		} 
      	    } // end of for it calo records
       	}
      
      return;
    }

    void trigger_algorithm_test_time::_update_coinc_calo_record(const trigger_structures::calo_summary_record & a_calo_summary_record_25ns_, 
								trigger_structures::coincidence_calo_record & a_coinc_calo_record_1600ns_)
    {
      // Update information but keep the clocktick of the 1600 structure
      
      // Initial values of coinc calo record 1600 ns : 
      unsigned int multiplicity_side_0 = a_coinc_calo_record_1600ns_.total_multiplicity_side_0.to_ulong();
      unsigned int multiplicity_side_1 = a_coinc_calo_record_1600ns_.total_multiplicity_side_1.to_ulong();
      unsigned int multiplicity_gveto  = a_coinc_calo_record_1600ns_.total_multiplicity_gveto.to_ulong();
      bool lto_side_0 = a_coinc_calo_record_1600ns_.LTO_side_0;     
      bool lto_side_1 = a_coinc_calo_record_1600ns_.LTO_side_1;
      bool lto_gveto  = a_coinc_calo_record_1600ns_.LTO_gveto;
      std::bitset<trigger_info::CALO_ZONING_PER_SIDE_BITSET_SIZE> zoning_word_side_0 = a_coinc_calo_record_1600ns_.calo_zoning_word[0];
      std::bitset<trigger_info::CALO_ZONING_PER_SIDE_BITSET_SIZE> zoning_word_side_1 = a_coinc_calo_record_1600ns_.calo_zoning_word[1];
      std::bitset<trigger_info::CALO_XT_INFO_BITSET_SIZE> xt_info = a_coinc_calo_record_1600ns_.xt_info_bitset ;
      bool single_side =  a_coinc_calo_record_1600ns_.single_side_coinc;
      bool total_multiplicity_threshold = a_coinc_calo_record_1600ns_.total_multiplicity_threshold;
      bool calo_decision = a_coinc_calo_record_1600ns_.decision;
     
      unsigned int clocktick_1600_from_25 = 0;
      _clock_manager_->compute_clocktick_25ns_to_1600ns(a_calo_summary_record_25ns_.clocktick_25ns,
							clocktick_1600_from_25);

      if (clocktick_1600_from_25 <= a_coinc_calo_record_1600ns_.clocktick_1600ns + _coincidence_calorimeter_gate_size_)
	{ 
	  a_coinc_calo_record_1600ns_.calo_zoning_word[0] = a_calo_summary_record_25ns_.zoning_word[0];
	  a_coinc_calo_record_1600ns_.calo_zoning_word[1] = a_calo_summary_record_25ns_.zoning_word[1];	

	  for (unsigned int i = 0; i < trigger_info::CALO_ZONING_PER_SIDE_BITSET_SIZE; i++)
	    {
	      if (a_coinc_calo_record_1600ns_.calo_zoning_word[0].test(i) == true)
		{
		  zoning_word_side_0.set(i, true);
		}
	      if (a_coinc_calo_record_1600ns_.calo_zoning_word[1].test(i) == true)
		{
		  zoning_word_side_1.set(i, true);
		}
	    }
	  a_coinc_calo_record_1600ns_.calo_zoning_word[0] = zoning_word_side_0;
	  a_coinc_calo_record_1600ns_.calo_zoning_word[1] = zoning_word_side_1;
	  a_coinc_calo_record_1600ns_.xt_info_bitset = xt_info;

	  unsigned int number_of_zone_touch_side_0 = a_coinc_calo_record_1600ns_.calo_zoning_word[0].count();
	  unsigned int number_of_zone_touch_side_1 = a_coinc_calo_record_1600ns_.calo_zoning_word[1].count();
	  
	  if (multiplicity_side_0 < a_calo_summary_record_25ns_.total_multiplicity_side_0.to_ulong())
	    {
	      a_coinc_calo_record_1600ns_.total_multiplicity_side_0 = a_calo_summary_record_25ns_.total_multiplicity_side_0.to_ulong();
	    }
	  if (number_of_zone_touch_side_0 > multiplicity_side_0)
	    {
	      a_coinc_calo_record_1600ns_.total_multiplicity_side_0 = number_of_zone_touch_side_0;
	    }
	  
	  if (multiplicity_side_1 < a_calo_summary_record_25ns_.total_multiplicity_side_1.to_ulong())
	    {
	      a_coinc_calo_record_1600ns_.total_multiplicity_side_1 = a_calo_summary_record_25ns_.total_multiplicity_side_1.to_ulong();
	    }
	  if (number_of_zone_touch_side_1 > multiplicity_side_1)
	    {
	      a_coinc_calo_record_1600ns_.total_multiplicity_side_1 = number_of_zone_touch_side_1;
	    }

	  if (multiplicity_gveto < a_calo_summary_record_25ns_.total_multiplicity_gveto.to_ulong())
	    {
	      a_coinc_calo_record_1600ns_.total_multiplicity_gveto = a_calo_summary_record_25ns_.total_multiplicity_gveto.to_ulong();
	    }

	  a_coinc_calo_record_1600ns_.LTO_side_0 = a_coinc_calo_record_1600ns_.LTO_side_0 + a_calo_summary_record_25ns_.LTO_side_0;
	  a_coinc_calo_record_1600ns_.LTO_side_1 = a_coinc_calo_record_1600ns_.LTO_side_1 + a_calo_summary_record_25ns_.LTO_side_1;
	  a_coinc_calo_record_1600ns_.LTO_gveto = a_coinc_calo_record_1600ns_.LTO_gveto + a_calo_summary_record_25ns_.LTO_gveto;
		      
	  if (a_coinc_calo_record_1600ns_.LTO_side_0 || lto_side_0) a_coinc_calo_record_1600ns_.LTO_side_0 = true;
	  if (a_coinc_calo_record_1600ns_.LTO_side_1 || lto_side_1) a_coinc_calo_record_1600ns_.LTO_side_1 = true;
	  if (a_coinc_calo_record_1600ns_.LTO_gveto  || lto_gveto) a_coinc_calo_record_1600ns_.LTO_gveto = true;

	  a_coinc_calo_record_1600ns_.xt_info_bitset = a_calo_summary_record_25ns_.xt_info_bitset;
	  a_coinc_calo_record_1600ns_.single_side_coinc = a_calo_summary_record_25ns_.single_side_coinc;
	  
	  if (!a_calo_summary_record_25ns_.single_side_coinc || !single_side)
	    {
	      a_coinc_calo_record_1600ns_.single_side_coinc = false;
	    }
	  else if (a_coinc_calo_record_1600ns_.calo_zoning_word[0].any() && a_coinc_calo_record_1600ns_.calo_zoning_word[1].any()) a_coinc_calo_record_1600ns_.single_side_coinc = false;
	  else a_coinc_calo_record_1600ns_.single_side_coinc = true;

	  a_coinc_calo_record_1600ns_.total_multiplicity_threshold =  a_calo_summary_record_25ns_.total_multiplicity_threshold;
	  if (a_coinc_calo_record_1600ns_.total_multiplicity_threshold || total_multiplicity_threshold) a_coinc_calo_record_1600ns_.total_multiplicity_threshold = true;
	  a_coinc_calo_record_1600ns_.decision = a_calo_summary_record_25ns_.calo_finale_decision;
	  if (a_coinc_calo_record_1600ns_.decision || calo_decision)  a_coinc_calo_record_1600ns_.decision = true;
	}
      
      return;
    }

    void trigger_algorithm_test_time::_build_previous_event_record()
    {
      // A PER is created or updated only if a coincidence between tracker and calorimeter is ok (so CALO_TRACKER_TIME_COINC or CARACO)
      
      trigger_structures::L2_decision the_L2_decision = _L2_decision_records_.back();
      uint32_t L2_decision_clocktick = the_L2_decision.L2_ct_decision;
      DT_THROW_IF(the_L2_decision.L2_trigger_mode != trigger_structures::L2_trigger_mode::CARACO 
		  && the_L2_decision.L2_trigger_mode != trigger_structures::L2_trigger_mode::CALO_TRACKER_TIME_COINC,
		  std::logic_error, "Previous event record can't be build because L2 decision is not equal to CARACO or CALO_TRACKER_TIME_COINC !");

      // Construct the PER on the coincidence records pushed back (to have the maximum of calo / tracker information (concatenate)) : 
      trigger_structures::previous_event_record a_previous_event_record;
      for (unsigned int i = 0; i < _coincidence_records_.size(); i++)
	{
	  trigger_structures::coincidence_event_record a_coincidence_record = _coincidence_records_[i];
	  // Check if the coincidence record is in the L2 gate and with the good trigger mode to construct the PER :
	  if (a_coincidence_record.clocktick_1600ns >= L2_decision_clocktick
	      && a_coincidence_record.clocktick_1600ns < (L2_decision_clocktick + _L2_decision_coincidence_gate_size_)
	      && (a_coincidence_record.trigger_mode == trigger_structures::L2_trigger_mode::CARACO || 
		  a_coincidence_record.trigger_mode == trigger_structures::L2_trigger_mode::CALO_TRACKER_TIME_COINC))
	    {
	      // Update the PER with all information from coincidence records in the gate
	      a_previous_event_record.previous_clocktick_1600ns = a_coincidence_record.clocktick_1600ns;
	      a_previous_event_record.counter_1600ns = clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK;
	      a_previous_event_record.trigger_mode = the_L2_decision.L2_trigger_mode;
	      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
		{
		  for (unsigned int ibit = 0; ibit < trigger_info::CALO_ZONING_PER_SIDE_BITSET_SIZE; ibit++)
		    {
		      if (a_previous_event_record.calo_zoning_word[iside].test(ibit) || a_coincidence_record.calo_zoning_word[iside].test(ibit))
			{
			  a_previous_event_record.calo_zoning_word[iside].set(ibit);
			}
		    } // end of ibit
		} // end of iside
		  
		  // Initial values in the PER :
	      unsigned int multiplicity_side_0 = a_previous_event_record.total_multiplicity_side_0.to_ulong();
	      unsigned int multiplicity_side_1 = a_previous_event_record.total_multiplicity_side_1.to_ulong();
	      unsigned int multiplicity_gveto  = a_previous_event_record.total_multiplicity_gveto.to_ulong();
	      bool lto_side_0 = a_previous_event_record.LTO_side_0;     
	      bool lto_side_1 = a_previous_event_record.LTO_side_1;
	      bool lto_gveto  = a_previous_event_record.LTO_gveto;
	      unsigned int number_of_zone_touch_side_0 = a_previous_event_record.calo_zoning_word[0].count();
	      unsigned int number_of_zone_touch_side_1 = a_previous_event_record.calo_zoning_word[1].count();   
	      bool single_side = a_previous_event_record.single_side_coinc;
	      bool total_multiplicity_threshold = a_previous_event_record.total_multiplicity_threshold;
	      bool decision = a_previous_event_record.decision;
	      a_previous_event_record.xt_info_bitset = a_coincidence_record.xt_info_bitset;
		  
	      if (multiplicity_side_0 < a_coincidence_record.total_multiplicity_side_0.to_ulong()) 
		{
		  a_previous_event_record.total_multiplicity_side_0 = a_coincidence_record.total_multiplicity_side_0.to_ulong();
		}
		  
	      if (number_of_zone_touch_side_0 > multiplicity_side_0)
		{
		  a_previous_event_record.total_multiplicity_side_0 = number_of_zone_touch_side_0;
		}
	  
	      if (multiplicity_side_1 < a_coincidence_record.total_multiplicity_side_1.to_ulong())
		{
		  a_previous_event_record.total_multiplicity_side_1 = a_coincidence_record.total_multiplicity_side_1.to_ulong();
		}

	      if (number_of_zone_touch_side_1 > multiplicity_side_1)
		{
		  a_previous_event_record.total_multiplicity_side_1 = number_of_zone_touch_side_1;
		}

	      if (multiplicity_gveto < a_coincidence_record.total_multiplicity_gveto.to_ulong())
		{
		  a_previous_event_record.total_multiplicity_gveto = a_coincidence_record.total_multiplicity_gveto.to_ulong();
		}
	      if (lto_side_0 || a_coincidence_record.LTO_side_0) a_previous_event_record.LTO_side_0 = true;
	      if (lto_side_1 || a_coincidence_record.LTO_side_1) a_previous_event_record.LTO_side_1 = true;
	      if (lto_gveto || a_coincidence_record.LTO_gveto) a_previous_event_record.LTO_gveto = true;
	      if (single_side || a_coincidence_record.single_side_coinc) a_previous_event_record.single_side_coinc = true;
	      else if (!single_side || !a_coincidence_record.single_side_coinc) a_previous_event_record.single_side_coinc = false;
	      if (total_multiplicity_threshold || a_coincidence_record.total_multiplicity_threshold) a_previous_event_record.total_multiplicity_threshold = true;
	      if (decision || a_coincidence_record.decision) a_previous_event_record.decision = true;
		  
	      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
		{
		  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
		    {
		      for (unsigned int ibit = 0; ibit < trigger_info::DATA_FULL_BITSET_SIZE; ibit++)
			{
			  if (a_previous_event_record.tracker_finale_data_per_zone[iside][izone].test(ibit) || a_coincidence_record.tracker_finale_data_per_zone[iside][izone].test(ibit))
			    {
			      a_previous_event_record.tracker_finale_data_per_zone[iside][izone].set(ibit);
			    }
			} // end of ibit
		      if (a_coincidence_record.coincidence_zoning_word[iside].test(izone) || a_previous_event_record.coincidence_zoning_word[iside].test(izone))
			{
			  a_previous_event_record.coincidence_zoning_word[iside].set(izone);
			}
		      if (a_coincidence_record.tracker_zoning_word_pattern[iside].test(izone) || a_previous_event_record.tracker_zoning_word_pattern[iside].test(izone))
			{
			  a_previous_event_record.tracker_zoning_word_pattern[iside].set(izone);
			}
		      if (a_coincidence_record.tracker_zoning_word_near_source[iside].test(izone) || a_previous_event_record.tracker_zoning_word_near_source[iside].test(izone))
			{
			  a_previous_event_record.tracker_zoning_word_near_source[iside].set(izone);
			}

		    } // end of izone
		} // end of iside
	    }	
	} // end of for coinc record size
      _previous_event_records_->push_back(a_previous_event_record);
	  
      return;
    }
	
    void trigger_algorithm_test_time::process(const calo_ctw_data & calo_ctw_data_,
    					      const geiger_ctw_data & geiger_ctw_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't process ! ");
      _process(calo_ctw_data_,
    	       geiger_ctw_data_);
      return;
    }  

    void trigger_algorithm_test_time::_process(const calo_ctw_data & calo_ctw_data_,
    					       const geiger_ctw_data & geiger_ctw_data_)
    {
      _previous_event_records_.reset(new buffer_previous_event_record_type(_previous_event_circular_buffer_depth_));
      snemo::digitization::geiger_ctw_data geiger_ctw_data_1600ns = geiger_ctw_data_;
      
      // Remove 1 of 2 gg ctw data due to data transfer limitation (CB to TB)
      for (unsigned int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  const geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.get_geiger_ctws()[i].get();
	  if (a_gg_ctw.get_clocktick_800ns() % 2 == 1)
	    {
	      geiger_ctw_data_1600ns.grab_geiger_ctws().erase(geiger_ctw_data_1600ns.grab_geiger_ctws().begin() + i);
	    }
	}

      for (unsigned int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  const geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.get_geiger_ctws()[i].get();
	  if (a_gg_ctw.get_clocktick_800ns() % 2 == 1)
	    {
	      geiger_ctw_data_1600ns.grab_geiger_ctws().erase(geiger_ctw_data_1600ns.grab_geiger_ctws().begin() + i);
	    }
	}

      // Warning Clocktick of gg ctw is not at 800 ns but at 1600 ns (only even CT 800 are kept (1 of 2))
      for (unsigned int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.grab_geiger_ctws()[i].grab();
	  uint32_t gg_ctw_clocktick_1600ns = clock_utils::INVALID_CLOCKTICK;
	  _clock_manager_->compute_clocktick_800ns_to_1600ns(a_gg_ctw.get_clocktick_800ns(),
							     gg_ctw_clocktick_1600ns);
	  a_gg_ctw.set_clocktick_800ns(gg_ctw_clocktick_1600ns);
	  // a_gg_ctw.tree_dump(std::clog, "INFO : ", "My gg ctw "); 
	}
     
      // Process the calorimeter algorithm at 25 ns to create calo record at 25 ns
      _calo_algo_.process(calo_ctw_data_,
			  _calo_records_25ns_);

      uint32_t last_calo_ct_25ns = clock_utils::INVALID_CLOCKTICK;
      // Create calo L1 decision(s) :
      for (unsigned int i = 0; i < _calo_records_25ns_.size(); i++)
	{
	  trigger_structures::calo_summary_record a_calo_record_25ns = _calo_records_25ns_[i];

	  uint32_t calo_record_ct_25ns = a_calo_record_25ns.clocktick_25ns;
	  
	  if ((last_calo_ct_25ns == clock_utils::INVALID_CLOCKTICK || calo_record_ct_25ns != last_calo_ct_25ns + 1) &&  a_calo_record_25ns.calo_finale_decision == true)
	    {
	      trigger_structures::L1_calo_decision a_L1_calo_decision_25ns;
	      a_L1_calo_decision_25ns.L1_calo_decision_bool = true;
	      a_L1_calo_decision_25ns.L1_calo_ct_decision = calo_record_ct_25ns;
	      _L1_calo_decision_records_.push_back(a_L1_calo_decision_25ns);
	    }
	  last_calo_ct_25ns = calo_record_ct_25ns;
	}
            
      // Configuration is calorimeter only (no coincidences at all) :
      // In this case of configuration L1 = L2
      if (_activate_calorimeter_only_ && !_activate_any_coincidences_)
	{
	  if (_L1_calo_decision_records_.size() != 0)
	    {
	      for (unsigned int i = 0; i < _L1_calo_decision_records_.size(); i++)
		{
		  if (_L1_calo_decision_records_[i].L1_calo_decision_bool)
		    {
		      trigger_structures::L2_decision a_L2_decision;
		      a_L2_decision.L2_decision_bool = true;
		      a_L2_decision.L2_ct_decision = _L1_calo_decision_records_[i].L1_calo_ct_decision;
		      a_L2_decision.L2_trigger_mode = trigger_structures::CALO_ONLY;
		      _L2_decision_records_.push_back(a_L2_decision);
		    }
		}
	    }
	}
      
      // Any coincidences are activated and calo only is not activated, rescaling calorimeter at 1600 ns in any case
      else if (_activate_any_coincidences_ && !_activate_calorimeter_only_)
	{
	  _rescale_calo_records_at_1600ns(_calo_records_25ns_,
	  				  _coincidence_calo_records_1600ns_);
	  for (unsigned int ict = 0; ict <_coincidence_calo_records_1600ns_.size(); ict++)
	    {
	      // _coincidence_calo_records_1600ns_[ict].display();
	    }
	  
	  // Calculate the ct 1600 minimum and ct 1600 maximum : 
	  uint32_t calorimeter_ct_min_1600 = clock_utils::INVALID_CLOCKTICK;
	  uint32_t calorimeter_ct_max_1600 = clock_utils::INVALID_CLOCKTICK;
	  uint32_t tracker_ct_min_1600     = clock_utils::INVALID_CLOCKTICK;
	  uint32_t tracker_ct_max_1600     = clock_utils::INVALID_CLOCKTICK;

	  if (_coincidence_calo_records_1600ns_.size() != 0)
	    {
	      calorimeter_ct_min_1600 = _coincidence_calo_records_1600ns_.front().clocktick_1600ns;
	      calorimeter_ct_max_1600 = _coincidence_calo_records_1600ns_.back().clocktick_1600ns;
	    }
	  
	  if (geiger_ctw_data_1600ns.get_geiger_ctws().size() != 0)
	    {
	      tracker_ct_min_1600 = geiger_ctw_data_1600ns.get_clocktick_min();
	      tracker_ct_max_1600 = geiger_ctw_data_1600ns.get_clocktick_max();	      
	    }

	  uint32_t clocktick_min = clock_utils::INVALID_CLOCKTICK;
	  if (calorimeter_ct_min_1600 != clock_utils::INVALID_CLOCKTICK) clocktick_min = calorimeter_ct_min_1600;
	  else if (tracker_ct_min_1600 != clock_utils::INVALID_CLOCKTICK) clocktick_min = tracker_ct_min_1600;

	  if ((tracker_ct_min_1600 != clock_utils::INVALID_CLOCKTICK && calorimeter_ct_min_1600 != clock_utils::INVALID_CLOCKTICK) && tracker_ct_min_1600 < clocktick_min) clocktick_min = tracker_ct_min_1600;
    
	  uint32_t clocktick_max = clock_utils::INVALID_CLOCKTICK;
	  if (tracker_ct_max_1600 != clock_utils::INVALID_CLOCKTICK) clocktick_max = tracker_ct_max_1600;
	  else if (calorimeter_ct_max_1600 != clock_utils::INVALID_CLOCKTICK) clocktick_max = calorimeter_ct_max_1600;
	  if ((tracker_ct_max_1600 != clock_utils::INVALID_CLOCKTICK && calorimeter_ct_max_1600 != clock_utils::INVALID_CLOCKTICK) && calorimeter_ct_max_1600 > clocktick_max) clocktick_max = calorimeter_ct_max_1600;

	  // std::clog << "Calo CT min 1600    = " << calorimeter_ct_min_1600 << std::endl;
	  // std::clog << "Calo CT max 1600    = " << calorimeter_ct_max_1600 << std::endl;
	  // std::clog << "Tracker CT min 1600 = " << tracker_ct_min_1600 << std::endl;
	  // std::clog << "Tracker CT max 1600 = " << tracker_ct_max_1600 << std::endl;
	  // std::clog << "CT min 1600         = " << clocktick_min << std::endl;
	  // std::clog << "CT max 1600         = " << clocktick_max << std::endl;
	  
	  if (clocktick_min != clock_utils::INVALID_CLOCKTICK && clocktick_max != clock_utils::INVALID_CLOCKTICK)
	    {
	      // Maybe time optimisation to do here, is it mandatory to go for each clocktick ?
	      // Maybe prepare tracker record outside this loop but it breaks the time implementation (close to the electronics)
	      
	      std::pair <bool, unsigned int> per_to_delete;
	      per_to_delete.first = false;
	      per_to_delete.second = -1;
	      for (uint32_t ict1600 = clocktick_min; ict1600 <= clocktick_max; ict1600++)
		{
		  //std::clog << "************* CT1600 : " << ict1600 << " ****************" <<std::endl;
		  
		  // Decrease PERs counters if exist and delete if a counter is set to 0.
		  if (!_previous_event_records_->empty())
		    {
		      // _previous_event_records_->back().counter_1600ns = clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK - ict1600 + _previous_event_records_->back().previous_clocktick_1600ns;
		      // std::clog << "CT PER = " << _previous_event_records_->back().previous_clocktick_1600ns << " counter = " <<
		      // 	_previous_event_records_->back().counter_1600ns << std::endl;

		      auto it_circ = _previous_event_records_->begin();
		      unsigned int pers_counter = 0;
		      if (per_to_delete.first)
			{
			  if (_previous_event_records_->size() > 1) 
			    {
			      _previous_event_records_->erase(_previous_event_records_->begin() + per_to_delete.second);
			    }
			  else if (_previous_event_records_->size() == 1)
			    {
			      _previous_event_records_->clear(); 
			    }
			  per_to_delete.first = false;
			  per_to_delete.second = -1;
			}

		      // Problem here : counter 1600 on it_circ gives a strange value 
		      for (; it_circ != _previous_event_records_->end(); it_circ++)
			{
			  DT_THROW_IF(it_circ->counter_1600ns > clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK, std::logic_error, "Previous Event Record counter out of bounds [0;clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK (625)] !");			  
			  if (it_circ->counter_1600ns <= clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK)
			    {
			      int counter_result = clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK - ict1600 + it_circ->previous_clocktick_1600ns;
			      if (counter_result <= 0) counter_result = 0;
			      it_circ->counter_1600ns = counter_result;
			      // std::clog << "CT PER = " << it_circ->previous_clocktick_1600ns << " counter = " << it_circ->counter_1600ns << std::endl;
			    }
			  if (it_circ->counter_1600ns == 0)
			    {
			      // PER has to be erased in the circular buffer because the counter equal to 0.
			      // Flag to remember this per has to be delete:
			      per_to_delete.first = true;
			      per_to_delete.second = pers_counter;
			    }
			}
		      pers_counter++;
		    }
		  
		  // std::clog << "Size of PERs = " << _previous_event_records_->size() << " Empty : " << _previous_event_records_->empty() << std::endl;
		  		  
		  trigger_structures::tracker_record a_tracker_record;
		  a_tracker_record.clocktick_1600ns = ict1600;

  
		  if (geiger_ctw_data_1600ns.get_geiger_ctws().size() != 0)
		    {
		      geiger_ctw_data::geiger_ctw_collection_type geiger_ctw_list_per_clocktick_1600;
		      geiger_ctw_data_1600ns.get_list_of_geiger_ctw_per_clocktick(ict1600, geiger_ctw_list_per_clocktick_1600);
		      _tracker_algo_.process(geiger_ctw_list_per_clocktick_1600,
					     a_tracker_record);

		      if (!a_tracker_record.is_empty()) _tracker_records_.push_back(a_tracker_record);
		  
		      trigger_structures::geiger_matrix a_geiger_matrix = _tracker_algo_.get_geiger_matrix_for_a_clocktick();
		      a_geiger_matrix.clocktick_1600ns = ict1600;
		      if (!a_geiger_matrix.is_empty()) _geiger_matrix_records_.push_back(a_geiger_matrix);
		    }

		  trigger_structures::coincidence_calo_record a_coinc_calo_record_for_pair;
		  a_coinc_calo_record_for_pair.clocktick_1600ns = ict1600;
		  std::vector<trigger_structures::coincidence_calo_record>::const_iterator it_calo = _coincidence_calo_records_1600ns_.begin();
		  for (; it_calo != _coincidence_calo_records_1600ns_.end(); it_calo++)
		    { 
		      trigger_structures::coincidence_calo_record a_coinc_calo_record = *it_calo;
		      if (a_coinc_calo_record.clocktick_1600ns == ict1600)
			{
			  a_coinc_calo_record_for_pair = a_coinc_calo_record;
			}
		    } 

		  if (!a_coinc_calo_record_for_pair.is_empty() || !a_tracker_record.is_empty())
		    {
		      std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> pair_for_a_clocktick;
		      pair_for_a_clocktick.first  = a_coinc_calo_record_for_pair;
		      pair_for_a_clocktick.second = a_tracker_record;
		      _pair_records_.push_back(pair_for_a_clocktick);
		      
		      // Process calo tracker coincidence for a clocktick in coinc_algo
		      // The coincidence process can be process(pair_for_a_ct)
		      // { CARACO(), APE(), DAVE() }
		      trigger_structures::coincidence_event_record a_coincidence_event_record;
		      trigger_structures::L2_decision a_L2_decision;

		      bool L2_decision_already_created = false;
		      // Maybe check if a L2_decision already exist before coinc processing ?
		  
		      if (!_L2_decision_records_.empty() 
			  && (int)_L2_decision_records_.back().L2_ct_decision >= (int)((ict1600 - _L2_decision_coincidence_gate_size_))
			  && _L2_decision_records_.back().L2_ct_decision < ict1600
			  && _L2_decision_records_.back().L2_decision_bool)
			{
			  L2_decision_already_created = true;
			}

		      // CARACO or APE or DAVE in coinc algo :
		      _coinc_algo_.process(pair_for_a_clocktick,
					   a_coincidence_event_record,
					   a_L2_decision,
					   _previous_event_records_);

		      if (a_coincidence_event_record.clocktick_1600ns != clock_utils::INVALID_CLOCKTICK
			  && a_coincidence_event_record.clocktick_1600ns == ict1600
			  && a_coincidence_event_record.decision == true
			  && a_coincidence_event_record.trigger_mode != trigger_structures::L2_trigger_mode::INVALID)
			{
			  _coincidence_records_.push_back(a_coincidence_event_record);		      
			}

		      // L2 decision to push back (not at each clocktick). Only if there is no other L2 decision in the 
		      // L2 decision gate [CTi-5; CTi]
		      if (a_L2_decision.L2_decision_bool == true 
			  && a_L2_decision.L2_ct_decision != clock_utils::INVALID_CLOCKTICK
			  && a_L2_decision.L2_trigger_mode != trigger_structures::L2_trigger_mode::INVALID
			  && L2_decision_already_created == false)
			{
			  _L2_decision_records_.push_back(a_L2_decision);
			}

		    } // end of if calo is empty || tracker is empty

		  // Build a PER only at the end of the L2 decision gate :
		  if (!_L2_decision_records_.empty())
		    {
		      // std::clog << "L2 ct + L2 decision = " << _L2_decision_records_.back().L2_ct_decision + _L2_decision_coincidence_gate_size_ << " ict = " << ict1600 << std::endl;

		      trigger_structures::L2_decision the_L2_decision = _L2_decision_records_.back();

		      if (ict1600 == (_L2_decision_records_.back().L2_ct_decision + _L2_decision_coincidence_gate_size_)
			  && (the_L2_decision.L2_trigger_mode == trigger_structures::L2_trigger_mode::CARACO 
			      || the_L2_decision.L2_trigger_mode == trigger_structures::L2_trigger_mode::CALO_TRACKER_TIME_COINC))
			{
			  _build_previous_event_record();
			}
		    }

		} // end of ict1600

	    } // end of if ct min 

	} // end of else if any_coinc
     

      // for (unsigned int i = 0; i < _coincidence_calo_records_1600ns_.size(); i++)
      // 	{
      // 	  _coincidence_calo_records_1600ns_[i].display();
      // 	}

      // for (unsigned int i = 0; i < _tracker_records_.size(); i++)
      // 	{
      // 	  _tracker_records_[i].display();
      // 	  _geiger_matrix_records_[i].display();
      // 	}

      // for (unsigned int i = 0; i < _coincidence_records_.size(); i++)
      // 	{
      // 	  _coincidence_records_[i].display();
      // 	}  
      
      // for (unsigned int i = 0; i < _L2_decision_records_.size(); i++)
      // 	{
      // 	  _L2_decision_records_[i].display(); 
      // 	}

      // boost::circular_buffer<trigger_structures::previous_event_record>::iterator it_circ = _previous_event_records_->begin();
      // for (; it_circ != _previous_event_records_->end(); it_circ++)
      // 	{
      // 	  it_circ -> display();
      // 	}  
      
      
      // std::clog << "********* Size of Finale structures for one event *********" << std::endl;
      // std::clog << "Calo collection size @ 25 ns            : " << _calo_records_25ns_.size() << std::endl;
      // std::clog << "Calo collection size @ 1600 ns          : " << _coincidence_calo_records_1600ns_.size() << std::endl;
      // std::clog << "Tracker collection size @ 1600 ns       : " << _tracker_records_.size() << std::endl;
      // std::clog << "Geiger matrix collection size @ 1600 ns : " << _geiger_matrix_records_.size() << std::endl;
      // std::clog << "Pair records collection size @ 1600 ns  : " << _pair_records_.size() << std::endl;
      // std::clog << "Coincidence collection size @ 1600 ns   : " << _coincidence_records_.size() << std::endl;
      // std::clog << "Previous event collection size          : " << _previous_event_records_->size() << std::endl;
      // std::clog << "L1 calo collection size @ 25 ns         : " << _L1_calo_decision_records_.size() << std::endl;
      // std::clog << "L2 decision collection size @ 1600 ns   : " << _L2_decision_records_.size() << std::endl;
      
      return;
    }

 
  } // end of namespace digitization

} // end of namespace snemo
