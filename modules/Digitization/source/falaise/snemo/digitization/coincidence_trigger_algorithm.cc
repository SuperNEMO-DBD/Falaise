// snemo/digitization/trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm.h>
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    coincidence_trigger_algorithm::coincidence_calo_record::coincidence_calo_record()
    {
      clocktick_1600ns = -1;
      return;
    }
    
    void coincidence_trigger_algorithm::coincidence_calo_record::fetch_active_zones()
    {
      // for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      // 	{
      // 	  for (int i = 0; i < calo_trigger_algorithm::ZONING_PER_SIDE_BITSET_SIZE; i++)
      // 	    {
      // 	      if (calo_summary_record.zoning_word[iside].test(i) == true) 
      // 		{
      // 		  active_zones[iside].insert(i);
      // 		  if (i <= 8 && i >= 1) 
      // 		    {
      // 		      active_zones[iside].insert(i+1);
      // 		      active_zones[iside].insert(i-1);
      // 		    }
      // 		  else if (i == 0) active_zones[iside].insert(i+1);
      // 		  else if (i == 9) active_zones[iside].insert(i-1);
      // 		}
      // 	    }
      // 	}
      return;
    }

    coincidence_trigger_algorithm::coincidence_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _calorimeter_gate_size_ = 0;
      _coincidence_calo_records_.reserve(5);
      return;
    }

    coincidence_trigger_algorithm::~coincidence_trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void coincidence_trigger_algorithm::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    bool coincidence_trigger_algorithm::has_calorimeter_gate_size() const
    {
      return _calorimeter_gate_size_ != 0;
    }

    void coincidence_trigger_algorithm::set_calorimeter_gate_size(unsigned int calorimeter_gate_size_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo circular buffer depth can't be set ! ");
      _calorimeter_gate_size_ = calorimeter_gate_size_;
      return;
    }

    void coincidence_trigger_algorithm::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void coincidence_trigger_algorithm::initialize(const datatools::properties & config_)
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

    bool coincidence_trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void coincidence_trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      return;
    }
    
    void coincidence_trigger_algorithm::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _coincidence_calo_records_.clear();
      return;
    }

    void coincidence_trigger_algorithm::_preparing_calo_coincidence(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_)
    {     
      std::vector<calo_trigger_algorithm::calo_summary_record>::const_iterator it = calo_records_.begin();
      bool decision_already_true = false;
      coincidence_trigger_algorithm::coincidence_calo_record coincidence_calo_record;
      unsigned int clocktick_25ns_decision;
      unsigned int coincidence_calo_record_position = 0;
      for (it; it != calo_records_.end(); it++)
      	{
	  const calo_trigger_algorithm::calo_summary_record a_ctrec= *it;
	  unsigned int a_ctrec_clocktick_1600ns;
	  _compute_clocktick_1600ns(a_ctrec.clocktick_25ns, a_ctrec_clocktick_1600ns);
	  std::clog << "A ctrec clocktick 1600 = " <<  a_ctrec_clocktick_1600ns << std::endl;
	  
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
	      decision_already_true = true;
	      _coincidence_calo_records_.push_back(coincidence_calo_record);
	    }
	  
	  if (decision_already_true == true && 
	      a_ctrec_clocktick_1600ns == _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns &&
	      a_ctrec.clocktick_25ns <= clocktick_25ns_decision + 3) // 3 == calo circular buffer depth
	    {
	      std::clog << "Updated " << std::endl;
	      _coincidence_calo_records_[coincidence_calo_record_position].zoning_word[0] = a_ctrec.zoning_word[0];
	      _coincidence_calo_records_[coincidence_calo_record_position].zoning_word[1] = a_ctrec.zoning_word[1];
	      std::clog << "ZW 0 : " << _coincidence_calo_records_[0].zoning_word[0] << std::endl;
	      std::clog << "ZW 1 : " << _coincidence_calo_records_[0].zoning_word[1] << std::endl;
	    }
	  
	  else if (decision_already_true == true && 
		   a_ctrec_clocktick_1600ns != _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns)
	    {
	      std::clog << "Pushed back next CT" << std::endl;
	      coincidence_trigger_algorithm::coincidence_calo_record coincidence_calo_record_next_clocktick;
	      coincidence_calo_record_next_clocktick = coincidence_calo_record;
	      coincidence_calo_record_next_clocktick.clocktick_1600ns = coincidence_calo_record.clocktick_1600ns + 1;
	      _coincidence_calo_records_.push_back(coincidence_calo_record_next_clocktick);
	      coincidence_calo_record_position++;
	    }
	}
      
      if (decision_already_true == true)
       	{
	  for (int iclocktick = coincidence_calo_record.clocktick_1600ns + 1; iclocktick <= coincidence_calo_record.clocktick_1600ns + _calorimeter_gate_size_; iclocktick++)
	    {
      	      coincidence_trigger_algorithm::coincidence_calo_record a_coinc_calo_record;
      	      a_coinc_calo_record = _coincidence_calo_records_.back();	      
      	      a_coinc_calo_record.clocktick_1600ns = iclocktick;
      	      _coincidence_calo_records_.push_back(a_coinc_calo_record);
	    }
      	}
      std::clog << "Size of coincidence calo_records : " << _coincidence_calo_records_.size() << std::endl;
      
      return;
    }

    void coincidence_trigger_algorithm::_compute_clocktick_1600ns(const uint32_t clocktick_25ns_, uint32_t & clocktick_1600ns_)
    {
      clocktick_1600ns_ = (clocktick_25ns_ * clock_utils::MAIN_CLOCKTICK) / clock_utils::TRIGGER_CLOCKTICK;
      std::clog << "CT 1600 = " << clocktick_1600ns_ << " CT 25 = " << clocktick_25ns_ << std::endl;
      return;
    }
         
    void coincidence_trigger_algorithm::process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
						const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(calo_records_,
	       tracker_records_);
      return;
    }

    void coincidence_trigger_algorithm::_process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
						 const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_)
    {
      reset_data();
      _preparing_calo_coincidence(calo_records_);
      
      std::vector<tracker_trigger_algorithm::tracker_record>::const_iterator it = tracker_records_.begin();
      for (it; it != tracker_records_.end(); it++)
	{
	  tracker_trigger_algorithm::tracker_record a_tracker_record = *it;
	  
	  //  std::clog << " a tracker record CT 1600 = " << a_tracker_record.clocktick_1600ns << std::endl;
	}
      
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  //std::clog << "iside = " << iside << ' ';
	  // for (std::set<int>::iterator it= _coincidence_calo_records_.active_zones[iside].begin(); it!= _coincidence_calo_records_.active_zones[iside].end(); it++)
	  //   {
	  //     std::clog << ' ' << *it;
	  //   }
	  std::clog << std::endl;
	}

      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
