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
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      _tracker_algo_.set_electronic_mapping(my_electronic_mapping_);
      _calo_algo_.set_electronic_mapping(my_electronic_mapping_);
      _coinc_algo_.set_electronic_mapping(my_electronic_mapping_);
      return;
    }  

    void trigger_algorithm_test_time::set_clock_manager(const clock_utils & my_clock_manager_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, clock manager can't be set ! ");
      _clock_manager_ = & my_clock_manager_;
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
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized ! ");
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
       
      if (!is_activated_coincidence()) {
	if(config_.has_key("activate_coincidence")) {
	  bool activate_coincidence_config =  config_.fetch_boolean("activate_coincidence");
	  _activate_any_coincidences_ = activate_coincidence_config;
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
      uint32_t calorimeter_clocktick_min = calo_ctw_data_.get_clocktick_min();
      uint32_t calorimeter_clocktick_max = calo_ctw_data_.get_clocktick_max();

      uint32_t calorimeter_clocktick_min_1600ns = -1;
      uint32_t calorimeter_clocktick_max_1600ns = -1;
      _clock_manager_->compute_clocktick_25ns_to_1600ns(calorimeter_clocktick_min,
							calorimeter_clocktick_min_1600ns);
      _clock_manager_->compute_clocktick_25ns_to_1600ns(calorimeter_clocktick_max,
							calorimeter_clocktick_max_1600ns);  
      uint32_t tracker_clocktick_min_800ns = geiger_ctw_data_.get_clocktick_min();
      uint32_t tracker_clocktick_max_800ns = geiger_ctw_data_.get_clocktick_max();
      uint32_t tracker_clocktick_min_1600ns = -1;
      uint32_t tracker_clocktick_max_1600ns = -1;
      
      _clock_manager_->compute_clocktick_800ns_to_1600ns(tracker_clocktick_min_800ns,
							 tracker_clocktick_min_1600ns);
      _clock_manager_->compute_clocktick_800ns_to_1600ns(tracker_clocktick_max_800ns,
							 tracker_clocktick_max_1600ns);

      uint32_t clocktick_min = calorimeter_clocktick_min_1600ns;
      if (tracker_clocktick_min_1600ns < clocktick_min) clocktick_min = tracker_clocktick_min_1600ns;    
      uint32_t clocktick_max = tracker_clocktick_max_1600ns;
      if (calorimeter_clocktick_max_1600ns > clocktick_max) clocktick_max = calorimeter_clocktick_max_1600ns;

      std::clog << "Calo CT min 1600 = " << calorimeter_clocktick_min_1600ns << std::endl;
      std::clog << "Calo CT max 1600 = " << calorimeter_clocktick_max_1600ns << std::endl;
      std::clog << "Tracker CT min 800 = " << tracker_clocktick_min_800ns << std::endl;
      std::clog << "Tracker CT max 800 = " << tracker_clocktick_max_800ns << std::endl;
      std::clog << "Tracker CT min 1600 = " << tracker_clocktick_min_1600ns << std::endl;
      std::clog << "Tracker CT max 1600 = " << tracker_clocktick_max_1600ns << std::endl;
      std::clog << "CT min 1600 = " << clocktick_min << std::endl;
      std::clog << "CT max 1600 = " << clocktick_max << std::endl;
      
      snemo::digitization::geiger_ctw_data geiger_ctw_data_1600ns = geiger_ctw_data_;
      for (int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  const geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.get_geiger_ctws()[i].get();
	  if (a_gg_ctw.get_clocktick_800ns() % 2 == 1)
	    {
	      geiger_ctw_data_1600ns.grab_geiger_ctws().erase(geiger_ctw_data_1600ns.get_geiger_ctws().begin() + i);
	    }
	}

      for (int i = 0; i < geiger_ctw_data_1600ns.get_geiger_ctws().size(); i++)
	{
	  const geiger_ctw & a_gg_ctw = geiger_ctw_data_1600ns.get_geiger_ctws()[i].get();
	  if (a_gg_ctw.get_clocktick_800ns() % 2 == 1)
	    {
	      geiger_ctw_data_1600ns.grab_geiger_ctws().erase(geiger_ctw_data_1600ns.get_geiger_ctws().begin() + i);
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
	}
     
      // Process the calorimeter algorithm at 25 ns to create calo record at 25 ns
      _calo_algo_.process(calo_ctw_data_,
			  _calo_records_25ns_);
      
      // // Fake calo record for test : 
      // calo_trigger_algorithm_test_time::calo_summary_record a_fake_calo_record_25ns;
      // std::bitset<10> fake_bitset (std::string("0010000000"));
      // a_fake_calo_record_25ns.clocktick_25ns = 67;
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
      //else if (
      
      

      
      
      return;
    }

    

    // void trigger_algorithm_test_time::_process_calo_algo(const calo_ctw_data & calo_ctw_data_)
    // {
    //   _calo_algo_.process(calo_ctw_data_,
    // 			  _calo_records_);
    //   return;
    // }

    // void trigger_algorithm_test_time::_process_tracker_algo(const geiger_ctw_data & geiger_ctw_data_)
    // {
    //   _tracker_algo_.process(geiger_ctw_data_,
    // 			     _tracker_records_);
    //   return;
    // }
    
    // void trigger_algorithm_test_time::_process_coinc_algo()
    // {
    //   _coinc_algo_.process(_calo_records_,
    // 			   _tracker_records_,
    // 			   _coincidence_records_);
    //   return;
    // }
    
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
