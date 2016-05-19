// snemo/digitization/trigger_algorithm_test_time.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_algorithm_test_time.h>

namespace snemo {
  
  namespace digitization {

    trigger_algorithm_test_time::L1_calo_decision::L1_calo_decision()
    {
      L1_calo_decision::reset();
      return;      
    }

    void trigger_algorithm_test_time::L1_calo_decision::reset()
    {
      L1_calo_decision_bool = false;
      L1_calo_ct_decision = -1;
    }

    const void trigger_algorithm_test_time::L1_calo_decision::display() const
    {
      std::clog << "Display L1 calo decision @ 25 ns" << std::endl;
      std::clog << "Decision clocktick 25 ns =  " << L1_calo_ct_decision << std::endl;
      std::clog << "L1 calo decision         = [" << L1_calo_decision_bool << "]" << std::endl << std::endl;
      return;
    }

    trigger_algorithm_test_time::L1_tracker_decision::L1_tracker_decision()
    {
      L1_tracker_decision::reset();
      return;      
    }

    void trigger_algorithm_test_time::L1_tracker_decision::reset()
    {
      L1_tracker_decision_bool = false;
      L1_tracker_ct_decision = -1;
    }

    const void trigger_algorithm_test_time::L1_tracker_decision::display() const
    {
      std::clog << "Display L1 tracker decision @ 1600 ns" << std::endl;
      std::clog << "Decision clocktick 1600 ns  =  " << L1_tracker_ct_decision << std::endl;
      std::clog << "L1 tracker decision         = [" << L1_tracker_decision_bool << "]" << std::endl << std::endl;
      return;
    }

    trigger_algorithm_test_time::L2_decision::L2_decision()
    {
      L2_decision::reset();
      return;      
    }

    void trigger_algorithm_test_time::L2_decision::reset()
    {
      L2_decision_bool = false;
      L2_ct_decision = -1;
      L2_trigger_mode = INVALID;
    }

    const void trigger_algorithm_test_time::L2_decision::display() const
    {
      std::clog << "Display L2 decision @ 1600 ns" << std::endl;
      std::clog << "Decision clocktick 1600 ns =  " << L2_ct_decision << std::endl;
      std::clog << "L2 Trigger mode            = [" << L2_trigger_mode << "]" << std::endl;
      std::clog << "L2 decision                = [" << L2_decision_bool << "]" << std::endl << std::endl;
      return;
    }

    trigger_algorithm_test_time::previous_event_record::previous_event_record()
    {
      previous_event_record::reset();
      return;
    }
    
    void trigger_algorithm_test_time::previous_event_record::reset()
    {
      previous_clocktick_1600ns = -1;
      counter_1600ns = 0;
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
      for (int iside = 0; iside < trigger_info::NSIDES; iside++) 
	{
	  for (int izone = 0; izone < trigger_info::NZONES; izone++) 
	    {
	      tracker_finale_data_per_zone[iside][izone].reset();
	    }
	  zoning_word[iside].reset();
	}	      
      return;
    }
    
    const void trigger_algorithm_test_time::previous_event_record::display() const
    {
      std::clog << "************************************************************************************" << std::endl;
      std::clog << "*************************** Previous event record ********************" << std::endl;
      std::clog << "*************************** Previous clocktick 1600 = " << previous_clocktick_1600ns << " ********************" << std::endl;
      std::clog << "*************************** Counter 1600 = " << counter_1600ns << " ***************************" << std::endl;
      std::clog << "XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl; 
      std::clog << xt_info_bitset << ' ';
      std::clog << LTO_gveto << ' ';
      std::clog << total_multiplicity_gveto << ' ';
      std::clog << LTO_side_1 << ' ';
      std::clog << LTO_side_0 << ' ';
      std::clog << total_multiplicity_side_1 << ' ';
      std::clog << total_multiplicity_side_0 << ' ';
      for (int iside = trigger_info::NSIDES-1; iside >= 0; iside--)
      	{
      	  for (int izone = trigger_info::NZONES-1; izone >= 0 ; izone--)
      	    {
      	      std::clog << calo_zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}      
      std::clog << std::endl;
      std::clog << "Single Side coinc : " << single_side_coinc 
		<< "  |  Threshold total mult : "   << total_multiplicity_threshold << std::endl;  
      std::clog << "Bitset : [NSZL NSZR L M R O I] " << std::endl;
      for (int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  std::clog << "Side = " << iside << " | ";
	  for (int izone = 0; izone < trigger_info::NZONES; izone++)
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

    trigger_algorithm_test_time::trigger_algorithm_test_time()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _coincidence_calorimeter_gate_size_ = 0;
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
      _L1_tracker_decision_records_.clear();
      _L2_decision_records_.clear();
      _finale_trigger_decision_ = false;
      _delayed_finale_trigger_decision_ = false;
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
      _L1_tracker_decision_records_.clear();
      _L2_decision_records_.clear();
      _finale_trigger_decision_ = false;
      _delayed_finale_trigger_decision_ = false;
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
      _previous_event_records_.reset();
      _L1_calo_decision_records_.clear();
      _L1_tracker_decision_records_.clear();
      _L2_decision_records_.clear();
      _finale_trigger_decision_ = false;
      _delayed_finale_trigger_decision_ = false; 
      return;
    }
    
    void trigger_algorithm_test_time::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      _tracker_algo_.set_electronic_mapping(my_electronic_mapping_);
      _calo_algo_.set_electronic_mapping(my_electronic_mapping_);
      _coinc_algo_.set_electronic_mapping(my_electronic_mapping_);
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
  
    
    const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> trigger_algorithm_test_time::get_calo_records_25ns_vector() const  
    {
      return _calo_records_25ns_;
    }	

    const std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> trigger_algorithm_test_time::get_coincidence_calo_records_1600ns_vector() const
    {
      return _coincidence_calo_records_1600ns_;
    }
    
    const std::vector<tracker_trigger_algorithm_test_time::tracker_record>trigger_algorithm_test_time::get_tracker_records_vector() const
    {
      return _tracker_records_;
    }  

    const std::vector<coincidence_trigger_algorithm_test_time::coincidence_event_record> trigger_algorithm_test_time::get_coincidence_records_vector() const
    {
      return _coincidence_records_;
    }
        
    const bool trigger_algorithm_test_time::get_finale_decision() const
    {
      return _finale_trigger_decision_;
    }   

    const bool trigger_algorithm_test_time::get_delayed_finale_decision() const
    {
      return _delayed_finale_trigger_decision_;
    }
    
    void  trigger_algorithm_test_time::_rescale_calo_records_at_1600ns(const std::vector<calo_trigger_algorithm_test_time::calo_summary_record> & calo_records_25_ns_,
								       std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record> & coincidence_calo_records_1600ns_)
    {
      std::vector<calo_trigger_algorithm_test_time::calo_summary_record>::const_iterator it = calo_records_25_ns_.begin();
      for (it; it != calo_records_25_ns_.end(); it++)
      	{
      	  const calo_trigger_algorithm_test_time::calo_summary_record a_ctrec= *it;

      	  if (a_ctrec.calo_finale_decision == true)
      	    {
      	      unsigned int ctrec_clocktick_1600ns = 0;
      	      _clock_manager_->compute_clocktick_25ns_to_1600ns(a_ctrec.clocktick_25ns, ctrec_clocktick_1600ns);

      	      if (coincidence_calo_records_1600ns_.size() == 0)
      		{
      		  // No coincidence calo records, creation of the first and the 10 following EMPTY 
      		  coincidence_trigger_algorithm_test_time::coincidence_calo_record first_coincidence_calo_record;
      		  first_coincidence_calo_record.clocktick_1600ns = ctrec_clocktick_1600ns;
      		  coincidence_calo_records_1600ns_.push_back(first_coincidence_calo_record);
		    
      		  if (coincidence_calo_records_1600ns_.size() != 0)
      		    {
      		      for (int iclocktick = first_coincidence_calo_record.clocktick_1600ns + 1; iclocktick < first_coincidence_calo_record.clocktick_1600ns + _coincidence_calorimeter_gate_size_; iclocktick ++)
      			{
      			  coincidence_trigger_algorithm_test_time::coincidence_calo_record on_gate_coincidence_calo_record;
      			  on_gate_coincidence_calo_record = first_coincidence_calo_record;
      			  on_gate_coincidence_calo_record.clocktick_1600ns = iclocktick;
      			  coincidence_calo_records_1600ns_.push_back(on_gate_coincidence_calo_record);		      
      			}
      		    }
      		}
	  
      	      bool coinc_calo_record_find = false;

      	      // Search if a coincidence calo record for clocktick ctrec 25 -> 1600 is already existing
      	      for (int i = 0; i < coincidence_calo_records_1600ns_.size(); i++)
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
      			  for (int j = i+1; j < coincidence_calo_records_1600ns_.size(); j++)
      			    {
      			      if (coincidence_calo_records_1600ns_[j].clocktick_1600ns <= coincidence_calo_records_1600ns_[i].clocktick_1600ns + _coincidence_calorimeter_gate_size_) 
      				{
      				  unsigned int clocktick_1600_before_modification = coincidence_calo_records_1600ns_[j].clocktick_1600ns;
      				  coincidence_calo_records_1600ns_[j] = coincidence_calo_records_1600ns_[i];
      				  coincidence_calo_records_1600ns_[j].clocktick_1600ns = clocktick_1600_before_modification;
      				}
      			    } // end of for j
      			}

      		      // Have to grow back the gate and create new coincidence calo record
      		      unsigned int actual_index = i;
      		      unsigned int moduled_index = i % _coincidence_calorimeter_gate_size_;
      		      unsigned int index_min = std::floor(actual_index /  _coincidence_calorimeter_gate_size_) * _coincidence_calorimeter_gate_size_;
      		      unsigned int index_max = index_min + _coincidence_calorimeter_gate_size_ - 1;
      		      unsigned int clocktick_to_create = ctrec_clocktick_1600ns + _coincidence_calorimeter_gate_size_;	     
      		      unsigned int clocktick_max_1600 = coincidence_calo_records_1600ns_[index_max].clocktick_1600ns;
		      
      		      for (int j = clocktick_max_1600 + 1; j <= clocktick_to_create; j++)
      			{
      			  bool no_same_clocktick = true;
      			  for (int k = actual_index + 1 ; k < coincidence_calo_records_1600ns_.size(); k++)
      			    {   
      			      // Check if the CT i between max and wanted to create exist in the coinc calo record list :
      			      if (j == coincidence_calo_records_1600ns_[k].clocktick_1600ns)
      				{
      				  no_same_clocktick = false;
      				}
      			    } // end of for k
			  
      			  if (no_same_clocktick == true)
      			    {
      			      coincidence_trigger_algorithm_test_time::coincidence_calo_record grow_back_coincidence_calo_record = coincidence_calo_records_1600ns_[index_max];
      			      grow_back_coincidence_calo_record.clocktick_1600ns = j;
      			      coincidence_calo_records_1600ns_.push_back(grow_back_coincidence_calo_record);
      			    }
      			} // end of for j
		      		      
      		    }
      		} // end of for i 
	      
      	      // If not, create the first and the 10 following empty then updated
      	      if (coinc_calo_record_find == false)
      		{
      		  coincidence_trigger_algorithm_test_time::coincidence_calo_record new_coincidence_calo_record;
      		  new_coincidence_calo_record.clocktick_1600ns = ctrec_clocktick_1600ns;
      		  coincidence_calo_records_1600ns_.push_back(new_coincidence_calo_record);
      		  for (int iclocktick = new_coincidence_calo_record.clocktick_1600ns + 1; iclocktick < new_coincidence_calo_record.clocktick_1600ns + _coincidence_calorimeter_gate_size_; iclocktick ++)
      		    {
      		      coincidence_trigger_algorithm_test_time::coincidence_calo_record new_on_gate_coincidence_calo_record;
      		      new_on_gate_coincidence_calo_record = new_coincidence_calo_record;
      		      new_on_gate_coincidence_calo_record.clocktick_1600ns = iclocktick;
      		      coincidence_calo_records_1600ns_.push_back(new_on_gate_coincidence_calo_record);		      
      		    }
      		} 
      	    } // end of for it calo records
       	}
      
      return;
    }

    void trigger_algorithm_test_time::_update_coinc_calo_record(const calo_trigger_algorithm_test_time::calo_summary_record & a_calo_summary_record_25ns_, 
								coincidence_trigger_algorithm_test_time::coincidence_calo_record & a_coinc_calo_record_1600ns_)
    {
      // Update information but keep the clocktick of the 1600 structure
      
      // Initial values of coinc calo record 1600 ns : 
      unsigned int multiplicity_side_0 = a_coinc_calo_record_1600ns_.total_multiplicity_side_0.to_ulong();
      unsigned int multiplicity_side_1 = a_coinc_calo_record_1600ns_.total_multiplicity_side_1.to_ulong();
      unsigned int multiplicity_gveto  = a_coinc_calo_record_1600ns_.total_multiplicity_gveto.to_ulong();
      bool lto_side_0 = a_coinc_calo_record_1600ns_.LTO_side_0;     
      bool lto_side_1 = a_coinc_calo_record_1600ns_.LTO_side_1;
      bool lto_gveto  = a_coinc_calo_record_1600ns_.LTO_gveto;
      std::bitset<calo_trigger_algorithm_test_time::ZONING_PER_SIDE_BITSET_SIZE> zoning_word_side_0 = a_coinc_calo_record_1600ns_.calo_zoning_word[0];
      std::bitset<calo_trigger_algorithm_test_time::ZONING_PER_SIDE_BITSET_SIZE> zoning_word_side_1 = a_coinc_calo_record_1600ns_.calo_zoning_word[1];
      std::bitset<calo_trigger_algorithm_test_time::XT_INFO_BITSET_SIZE> xt_info = a_coinc_calo_record_1600ns_.xt_info_bitset ;
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

	  for (int i = 0; i < calo_trigger_algorithm_test_time::ZONING_PER_SIDE_BITSET_SIZE; i++)
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

	  int number_of_zone_touch_side_0 = a_coinc_calo_record_1600ns_.calo_zoning_word[0].count();
	  int number_of_zone_touch_side_1 = a_coinc_calo_record_1600ns_.calo_zoning_word[1].count();
	  
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
      
      snemo::digitization::geiger_ctw_data geiger_ctw_data_1600ns = geiger_ctw_data_;
      
      for (int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  const geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.get_geiger_ctws()[i].get();
	  if (a_gg_ctw.get_clocktick_800ns() % 2 == 1)
	    {
	      geiger_ctw_data_1600ns.grab_geiger_ctws().erase(geiger_ctw_data_1600ns.grab_geiger_ctws().begin() + i);
	    }
	}

      for (int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  const geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.get_geiger_ctws()[i].get();
	  if (a_gg_ctw.get_clocktick_800ns() % 2 == 1)
	    {
	      geiger_ctw_data_1600ns.grab_geiger_ctws().erase(geiger_ctw_data_1600ns.grab_geiger_ctws().begin() + i);
	    }
	}

      // Warning Clocktick of gg ctw is not at 800 ns but at 1600 ns (only even CT 800 are kept (1 off 2))
      for (int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.grab_geiger_ctws()[i].grab();
	  uint32_t gg_ctw_clocktick_1600ns = -1;
	  _clock_manager_->compute_clocktick_800ns_to_1600ns(a_gg_ctw.get_clocktick_800ns(),
							     gg_ctw_clocktick_1600ns);
	  a_gg_ctw.set_clocktick_800ns(gg_ctw_clocktick_1600ns);
	  // a_gg_ctw.tree_dump(std::clog, "INFO : ", "My gg ctw "); 
	}
     
      // Process the calorimeter algorithm at 25 ns to create calo record at 25 ns
      _calo_algo_.process(calo_ctw_data_,
			  _calo_records_25ns_);
      
      // // Fake calo record for test : 
      // calo_trigger_algorithm_test_time::calo_summary_record a_fake_calo_record_25ns;
      // std::bitset<10> fake_bitset (std::string("0010000000"));
      // a_fake_calo_record_25ns.clocktick_25ns = 220;
      // a_fake_calo_record_25ns.zoning_word[0] = fake_bitset;
      // a_fake_calo_record_25ns.total_multiplicity_side_0 = 1;
      // a_fake_calo_record_25ns.single_side_coinc = true;
      // a_fake_calo_record_25ns.total_multiplicity_threshold = true;
      // a_fake_calo_record_25ns.calo_finale_decision = true;
      // _calo_records_25ns_.push_back(a_fake_calo_record_25ns);
      
      uint32_t last_calo_ct_25ns = -1;

      // Create calo L1 decision(s) :
      for (int i = 0; i < _calo_records_25ns_.size(); i++)
	{
	  calo_trigger_algorithm_test_time::calo_summary_record a_calo_record_25ns = _calo_records_25ns_[i];
	  a_calo_record_25ns.display();

	  uint32_t calo_record_ct_25ns = a_calo_record_25ns.clocktick_25ns;
	  
	  if ((last_calo_ct_25ns == -1 || calo_record_ct_25ns != last_calo_ct_25ns + 1) &&  a_calo_record_25ns.calo_finale_decision == true)
	    {
	      L1_calo_decision a_L1_calo_decision_25ns;
	      a_L1_calo_decision_25ns.L1_calo_decision_bool = true;
	      a_L1_calo_decision_25ns.L1_calo_ct_decision = calo_record_ct_25ns;
	      _L1_calo_decision_records_.push_back(a_L1_calo_decision_25ns);
	    }
	  last_calo_ct_25ns = calo_record_ct_25ns;
	}
      
      for (int i = 0; i < _L1_calo_decision_records_.size(); i++)
	{
	  _L1_calo_decision_records_[i].display(); 
	}
      
      // Configuration is calorimeter only (no coincidences at all) :
      // In this case of configuration L1 = L2
      if (_activate_calorimeter_only_ && !_activate_any_coincidences_)
	{
	  if (_L1_calo_decision_records_.size() != 0)
	    {
	      for (int i = 0; i < _L1_calo_decision_records_.size(); i++)
		{
		  if (_L1_calo_decision_records_[i].L1_calo_decision_bool)
		    {
		      L2_decision a_L2_decision;
		      a_L2_decision.L2_decision_bool = true;
		      a_L2_decision.L2_ct_decision = _L1_calo_decision_records_[i].L1_calo_ct_decision;
		      a_L2_decision.L2_trigger_mode = CALO_ONLY;
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
	  
	  // Calculate the ct 1600 minimum and ct 1600 maximum : 

	  // Gestion d'erreur si GG CTW DATA vides : 
	  uint32_t calorimeter_ct_min_1600 = _coincidence_calo_records_1600ns_.front().clocktick_1600ns;
	  uint32_t calorimeter_ct_max_1600 = _coincidence_calo_records_1600ns_.back().clocktick_1600ns;
	  uint32_t tracker_ct_min_1600 = geiger_ctw_data_1600ns.get_clocktick_min();
	  uint32_t tracker_ct_max_1600 = geiger_ctw_data_1600ns.get_clocktick_max();
	  
	  uint32_t clocktick_min = calorimeter_ct_min_1600;
	  if (tracker_ct_min_1600 < clocktick_min) clocktick_min = tracker_ct_min_1600;    
	  uint32_t clocktick_max = tracker_ct_max_1600;
	  if (calorimeter_ct_max_1600 > clocktick_max) clocktick_max = calorimeter_ct_max_1600;

	  std::clog << "Calo CT min 1600 = " << calorimeter_ct_min_1600 << std::endl;
	  std::clog << "Calo CT max 1600 = " << calorimeter_ct_max_1600 << std::endl;
	  std::clog << "Tracker CT min 1600 = " << tracker_ct_min_1600 << std::endl;
	  std::clog << "Tracker CT max 1600 = " << tracker_ct_max_1600 << std::endl;
	  std::clog << "CT min 1600 = " << clocktick_min << std::endl;
	  std::clog << "CT max 1600 = " << clocktick_max << std::endl;
	  
	  for (int ict1600 = clocktick_min; ict1600 <= clocktick_max; ict1600++)
	    {
	      geiger_ctw_data::geiger_ctw_collection_type geiger_ctw_list_per_clocktick_1600;
	      geiger_ctw_data_1600ns.get_list_of_geiger_ctw_per_clocktick(ict1600, geiger_ctw_list_per_clocktick_1600);
	      
	      tracker_trigger_algorithm_test_time::tracker_record a_tracker_record;
	      a_tracker_record.clocktick_1600ns = ict1600;
	      _tracker_algo_.process(geiger_ctw_list_per_clocktick_1600,
				     a_tracker_record);
	      if (!a_tracker_record.is_empty()) _tracker_records_.push_back(a_tracker_record);
	      // a_tracker_record.display();

	      tracker_trigger_algorithm_test_time::geiger_matrix a_geiger_matrix = _tracker_algo_.get_geiger_matrix_for_a_clocktick();
	      a_geiger_matrix.clocktick_1600ns = ict1600;
	      // a_geiger_matrix.display();
	      if (!a_geiger_matrix.is_empty()) _geiger_matrix_records_.push_back(a_geiger_matrix);
	     

	      std::pair<coincidence_trigger_algorithm_test_time::coincidence_calo_record, tracker_trigger_algorithm_test_time::tracker_record> pair_for_a_clocktick;

	      std::vector<coincidence_trigger_algorithm_test_time::coincidence_calo_record>::const_iterator it_calo = _coincidence_calo_records_1600ns_.begin();
	      for (it_calo; it_calo != _coincidence_calo_records_1600ns_.end(); it_calo++)
		{ 
		  coincidence_trigger_algorithm_test_time::coincidence_calo_record a_coinc_calo_record = *it_calo;
		  if (a_coinc_calo_record.clocktick_1600ns == ict1600)
		    {
		      pair_for_a_clocktick.first = a_coinc_calo_record;
		    }
		} 
	      pair_for_a_clocktick.second = a_tracker_record;
	      
	      _pair_records_.push_back(pair_for_a_clocktick);
	      
	    }



	}

      for (int i = 0; i < _L2_decision_records_.size(); i++)
	{
	  _L2_decision_records_[i].display(); 
	}
      
      for (int i = 0; i < _coincidence_calo_records_1600ns_.size(); i++)
	{
	  _coincidence_calo_records_1600ns_[i].display();
	}
      
      
      std::clog << "********* Size of Finale structures for one event *********" << std::endl;
      std::clog << "Calo collection size @ 25 ns            : " << _calo_records_25ns_.size() << std::endl;
      std::clog << "Calo collection size @ 1600 ns          : " << _coincidence_calo_records_1600ns_.size() << std::endl;
      std::clog << "Tracker collection size @ 1600 ns       : " << _tracker_records_.size() << std::endl;
      std::clog << "Geiger matrix collection size @ 1600 ns : " << _geiger_matrix_records_.size() << std::endl;
      std::clog << "Pair records collection size @ 1600 ns  : " << _pair_records_.size() << std::endl;
      std::clog << "Coincidence collection size @ 1600 ns   : " << _coincidence_records_.size() << std::endl;
      //std::clog << "Previous event collection size          : " << _previous_event_records_->size() << std::endl;
      std::clog << "L1 calo collection size @ 25 ns         : " << _L1_calo_decision_records_.size() << std::endl;
      std::clog << "L1 tracker collection size @ 1600 ns    : " << _L1_tracker_decision_records_.size() << std::endl;
      std::clog << "L2 decision collection size @ 1600 ns   : " << _L2_decision_records_.size() << std::endl;
      
      return;
    }

    // void trigger_algorithm_test_time::_process(const calo_ctw_data & calo_ctw_data_,
    // 					       const geiger_ctw_data & geiger_ctw_data_)
    // {
    //   reset_data();
    //   if (calo_ctw_data_.has_calo_ctw())
    // 	{
    // 	  _process_calo_algo(calo_ctw_data_);
    // 	}
      
    //   if (geiger_ctw_data_.has_geiger_ctw())
    //   	{
    //   	  _process_tracker_algo(geiger_ctw_data_);
    //   	}
      
    //   if (is_activated_coincidence())
    // 	{
    // 	  _process_coinc_algo();
    // 	}    
            
    //   bool calo_decision = false;
    //   calo_decision = _calo_algo_.get_calo_decision();
    //   bool tracker_decision = false;
    //   tracker_decision = _tracker_algo_.get_tracker_decision();
    //   bool caraco_decision = false;
    //   caraco_decision = _coinc_algo_.get_caraco_decision();
    //   bool delayed_coincidence_decision = false;
    //   delayed_coincidence_decision = _coinc_algo_.get_delayed_coincidence_decision();

    //   // To improve depending of trigger configuration

    //   if (_activate_any_coincidences_)
    // 	{
    // 	  if (caraco_decision == true) _finale_trigger_decision_ = true;
    // 	  if (delayed_coincidence_decision == true) _delayed_finale_trigger_decision_ = true;
    // 	}
 
    //   else _finale_trigger_decision_ = false;
      
    //   //      if (coincidence_decision && _activate_any_coincidences_) _finale_trigger_decision_ = true;
    //   //else if (!_activate_any_coincidences_ && calo_decision && tracker_decision) _finale_trigger_decision_ = true;

    //   return;
    // }

  } // end of namespace digitization

} // end of namespace snemo
