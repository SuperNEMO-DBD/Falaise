// snemo/digitization/coincidence_trigger_algorithm_test_time.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm_test_time.h>
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {

    const uint32_t coincidence_trigger_algorithm_test_time::SIZE_OF_L2_COINCIDENCE_DECISION_GATE;

    coincidence_trigger_algorithm_test_time::coincidence_trigger_algorithm_test_time()
    {
      _initialized_ = false;
      _coincidence_decision_ = trigger_structures::L2_trigger_mode::INVALID;
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
      _coincidence_decision_ = trigger_structures::L2_trigger_mode::INVALID;
      return;
    }
    
    void coincidence_trigger_algorithm_test_time::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _coincidence_decision_ = trigger_structures::L2_trigger_mode::INVALID;
      return;
    }

    bool coincidence_trigger_algorithm_test_time::get_coincidence_decision() const
    {
      return _coincidence_decision_;
    }
    
    void coincidence_trigger_algorithm_test_time::_process_calo_tracker_coincidence(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> a_pair_for_a_clocktick_,
										    trigger_structures::coincidence_event_record & a_coincidence_record_,
										    trigger_structures::L2_decision & a_L2_decision_record_)

    { 
      trigger_structures::coincidence_calo_record a_calo_record = a_pair_for_a_clocktick_.first;
      trigger_structures::tracker_record a_tracker_record = a_pair_for_a_clocktick_.second;

      DT_THROW_IF(a_calo_record.clocktick_1600ns != a_tracker_record.clocktick_1600ns, std::logic_error, "Calo tracker coincidence can't process, clockticks of a calo record [" << a_calo_record.clocktick_1600ns << "] and a tracker record [" << a_tracker_record.clocktick_1600ns << "] are not the same ! ");

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
      		      a_coincidence_record_.coincidence_zoning_word[iside].set(izone, true);
      		      a_coincidence_record_.decision = true;
      		      a_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::CARACO;
      		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::CARACO;
      		    }

      		  if ((hpattern_for_a_zone.test(right) && a_calo_record.calo_zoning_word[iside].test(izone) != 0) ||
      		      (izone+1 < 10 && hpattern_for_a_zone.test(right) && a_calo_record.calo_zoning_word[iside].test(izone+1) != 0))
      		    {	  
      		      a_coincidence_record_.coincidence_zoning_word[iside].set(izone, true);
      		      a_coincidence_record_.decision = true;
      		      a_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::CARACO;
      		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::CARACO;
      		    }
			      
      		  if ((hpattern_for_a_zone.test(left) && a_calo_record.calo_zoning_word[iside].test(izone) != 0) ||
      		      (izone-1 > -1 && hpattern_for_a_zone.test(left) && a_calo_record.calo_zoning_word[iside].test(izone-1) != 0))
      		    {		 
      		      a_coincidence_record_.coincidence_zoning_word[iside].set(izone, true);
      		      a_coincidence_record_.decision = true;
      		      a_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::CARACO;
      		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::CARACO;
      		    }			  
    
      		} // end of hpattern.any()
      	    } // end of izone
      	} // end of iside
      
      if (a_coincidence_record_.decision = true && _coincidence_decision_ == trigger_structures::L2_trigger_mode::CARACO) 
      	{
	  a_coincidence_record_.clocktick_1600ns = a_calo_record.clocktick_1600ns;
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

	      a_coincidence_record_.tracker_zoning_word_pattern[iside] = a_tracker_record.zoning_word_pattern[iside];
	      a_coincidence_record_.tracker_zoning_word_near_source[iside] = a_tracker_record.zoning_word_near_source[iside];

	    } // end of iside

	  a_L2_decision_record_.L2_decision_bool = true;
	  a_L2_decision_record_.L2_ct_decision = a_calo_record.clocktick_1600ns;
	  a_L2_decision_record_.L2_trigger_mode = trigger_structures::L2_trigger_mode::CARACO;
	}

      return;
    }
    
    void coincidence_trigger_algorithm_test_time::_process_delayed_coincidence(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> a_pair_for_a_clocktick_,
									       trigger_structures::coincidence_event_record & a_delayed_coincidence_record_,
									       trigger_structures::L2_decision & a_L2_decision_record_,
									       const trigger_structures::previous_event_record & a_previous_event_record_)
    {
      trigger_structures::tracker_record a_tracker_record = a_pair_for_a_clocktick_.second;
	  
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      std::bitset<3> delayed_hpattern_per_zone = 0x0;
		  
	      int right = 0;
	      int mid   = 1;
	      int left  = 2;
	      
	      delayed_hpattern_per_zone[right] = a_tracker_record.finale_data_per_zone[iside][izone][right+2];
	      delayed_hpattern_per_zone[mid]   = a_tracker_record.finale_data_per_zone[iside][izone][mid+2];
	      delayed_hpattern_per_zone[left]  = a_tracker_record.finale_data_per_zone[iside][izone][left+2];
		   
	      // APE trigger (Tracker previous / Tracker delayed coincidence)
	      if (delayed_hpattern_per_zone.any())
		{ 
		  if (delayed_hpattern_per_zone.test(left) && izone == 0 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
									     || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
									     || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
									     || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
		    {
		      a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
		      a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
		      a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::APE;
		      a_delayed_coincidence_record_.decision = true;
		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::APE;
		    }

		  if (delayed_hpattern_per_zone.test(left) && izone-1 > -1 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(right+2)
									       || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
									       || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
									       || a_previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(right+2)
									       || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
									       || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)))
		    {
		      a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
		      a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
		      a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::APE;
		      a_delayed_coincidence_record_.decision = true;
		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::APE;
		    }    
		      
		  if (delayed_hpattern_per_zone.test(mid) && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(left+2)
							      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
							      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
							      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(left+2)
							      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
							      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
		    {
		      a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
		      a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
		      a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::APE;
		      a_delayed_coincidence_record_.decision = true;
		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::APE;
		    } 
		      
		  if (delayed_hpattern_per_zone.test(right) && izone == 10 &&(a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
									      || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
									      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
									      || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)))
		    {
		      a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
		      a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
		      a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::APE;
		      a_delayed_coincidence_record_.decision = true;
		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::APE;
		    }

		  if (delayed_hpattern_per_zone.test(right) && izone + 1 < 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(mid+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(right+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(left+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(mid+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(right+2)
										  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(left+2)))
		    {
		      a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
		      a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
		      a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::APE;
		      a_delayed_coincidence_record_.decision = true;
		      _coincidence_decision_ = trigger_structures::L2_trigger_mode::APE;
		    }
		}
	    } // end of izone
	} // end of iside
	  
      if (a_delayed_coincidence_record_.decision && _coincidence_decision_ == trigger_structures::L2_trigger_mode::APE)
	{
	  for (int iside = 0; iside < trigger_info::NSIDES; iside++)
	    {
	      a_delayed_coincidence_record_.tracker_zoning_word_pattern[iside] = a_tracker_record.zoning_word_pattern[iside];
	      a_delayed_coincidence_record_.tracker_zoning_word_near_source[iside] = a_tracker_record.zoning_word_near_source[iside];
	    }
	  a_delayed_coincidence_record_.single_side_coinc = a_tracker_record.single_side_coinc;
	  a_L2_decision_record_.L2_decision_bool = true;
	  a_L2_decision_record_.L2_ct_decision = a_tracker_record.clocktick_1600ns;
	  a_L2_decision_record_.L2_trigger_mode = trigger_structures::L2_trigger_mode::APE;
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
			  a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::DAVE;
			  a_delayed_coincidence_record_.decision = true;
			  _coincidence_decision_ = trigger_structures::L2_trigger_mode::DAVE;
			}
			    

		      if (delayed_near_source_per_zone.test(near_source_left) && izone - 1 > -1 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone-1].test(near_source_right+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone-1].test(near_source_right+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
												    || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
			    
			{
			  a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::DAVE;
			  a_delayed_coincidence_record_.decision = true;
			  _coincidence_decision_ = trigger_structures::L2_trigger_mode::DAVE;
			}
		      

		      if (delayed_near_source_per_zone.test(near_source_right) && izone == 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
												  || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
												  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
												  || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)))
			{
			  a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::DAVE;
			  a_delayed_coincidence_record_.decision = true;
			  _coincidence_decision_ = trigger_structures::L2_trigger_mode::DAVE;
			}
			  
		      if (delayed_near_source_per_zone.test(near_source_right) && izone + 1 < 10 && (a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_left+5)
												     || a_previous_event_record_.tracker_finale_data_per_zone[0][izone].test(near_source_right+5)
												     || a_previous_event_record_.tracker_finale_data_per_zone[0][izone+1].test(near_source_left+5)
												     || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_left+5)
												     || a_previous_event_record_.tracker_finale_data_per_zone[1][izone].test(near_source_right+5)
												     || a_previous_event_record_.tracker_finale_data_per_zone[1][izone+1].test(near_source_left+5)))
			{
			  a_delayed_coincidence_record_.clocktick_1600ns = a_tracker_record.clocktick_1600ns;
			  a_delayed_coincidence_record_.tracker_finale_data_per_zone[iside][izone] = a_tracker_record.finale_data_per_zone[iside][izone];
			  a_delayed_coincidence_record_.trigger_mode = trigger_structures::L2_trigger_mode::DAVE;
			  a_delayed_coincidence_record_.decision = true;
			  _coincidence_decision_ = trigger_structures::L2_trigger_mode::DAVE;
			}
		    } // enf of if delayed any
		      
		} // end of izone
	    } // end of iside
	  
	  if (a_delayed_coincidence_record_.decision && _coincidence_decision_ == trigger_structures::L2_trigger_mode::DAVE)
	    {
	      for (int iside = 0; iside < trigger_info::NSIDES; iside++)
		{
		  a_delayed_coincidence_record_.tracker_zoning_word_near_source[iside] = a_tracker_record.zoning_word_near_source[iside];
		}
	      a_delayed_coincidence_record_.single_side_coinc = a_tracker_record.single_side_coinc;
	      a_L2_decision_record_.L2_decision_bool = true;
	      a_L2_decision_record_.L2_ct_decision = a_tracker_record.clocktick_1600ns;
	      a_L2_decision_record_.L2_trigger_mode = trigger_structures::L2_trigger_mode::DAVE;
	    }
	    
	}//end of else
	    
      return;
    }

    void coincidence_trigger_algorithm_test_time::process(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> pair_for_a_clocktick_,
							  trigger_structures::coincidence_event_record & a_coincidence_record_,
							  trigger_structures::L2_decision & a_L2_decision_record_,
							  const boost::scoped_ptr<boost::circular_buffer<trigger_structures::previous_event_record> > & previous_event_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(pair_for_a_clocktick_,
	       a_coincidence_record_,
	       a_L2_decision_record_,
	       previous_event_records_);
      return;
    }

    void coincidence_trigger_algorithm_test_time::_process(const std::pair<trigger_structures::coincidence_calo_record, trigger_structures::tracker_record> pair_for_a_clocktick_,
							   trigger_structures::coincidence_event_record & a_coincidence_record_,
							   trigger_structures::L2_decision & a_L2_decision_record_,
							   const boost::scoped_ptr<boost::circular_buffer<trigger_structures::previous_event_record> > & previous_event_records_)
    {
      reset_data();
      // Process CARACO :
      _process_calo_tracker_coincidence(pair_for_a_clocktick_,
					a_coincidence_record_,
					a_L2_decision_record_);
      
      // If no CARACO, search for an APE or DAVE delayed coincidence :
      if (previous_event_records_->size() != 0)
	{
	  auto it_circ = previous_event_records_->begin();
	  for (; it_circ != previous_event_records_->end(); it_circ++)
	    {
	      const trigger_structures::previous_event_record a_previous_event_record = *it_circ;
	      if (a_previous_event_record.counter_1600ns <= clock_utils::PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK
		  && a_previous_event_record.counter_1600ns > 0)
		{
		  _process_delayed_coincidence(pair_for_a_clocktick_,
					       a_coincidence_record_,
					       a_L2_decision_record_,
					       a_previous_event_record);
		} // end of if counter
	    } // end of for it_circ
	} // end of if size != 0
      
      return;
    }
    
  } // end of namespace digitization
  
} // end of namespace snemo
