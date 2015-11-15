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
    
    void coincidence_trigger_algorithm::coincidence_calo_record::compute_clocktick_1600ns()
    {
      clocktick_1600ns = (calo_summary_record.clocktick_25ns * clock_utils::MAIN_CLOCKTICK) / clock_utils::TRIGGER_CLOCKTICK;
      return;
    }

    coincidence_trigger_algorithm::coincidence_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;

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

    void coincidence_trigger_algorithm::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void coincidence_trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized ! ");
      
      // interprate config key for coincidences
      // if (! has_circular_buffer_depth()) {
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
      std::clog << "size of vector : " << calo_records_.size() << std::endl;
     
      std::vector<calo_trigger_algorithm::calo_summary_record>::const_iterator it=calo_records_.begin();
      bool decision_already_true = false;
      uint32_t clocktick_decision = 6666666;
      coincidence_calo_record coincidence_calo_record;

      for (it; it != calo_records_.end(); it++)
      	{
	  const calo_trigger_algorithm::calo_summary_record a_ctrec= *it;
	  if (a_ctrec.calo_finale_decision == true && decision_already_true == false)
	    {
	      coincidence_calo_record.calo_summary_record = a_ctrec;
	      clocktick_decision = coincidence_calo_record.calo_summary_record.clocktick_25ns;
	      decision_already_true = true;
	    }
	  
	  if (decision_already_true == true && a_ctrec.clocktick_25ns == (clocktick_decision + 3))
	    {
	      coincidence_calo_record.calo_summary_record.zoning_word[0] = a_ctrec.zoning_word[0];
	      coincidence_calo_record.calo_summary_record.zoning_word[1] = a_ctrec.zoning_word[1];
	      coincidence_calo_record.calo_summary_record.display();
	      coincidence_calo_record.compute_clocktick_1600ns();
	      _coincidence_calo_records_.push_back(coincidence_calo_record);
 	    }
	}

      
      // Ajout des 10 élément avec CT1600 ++ a chauqefois 

      std::clog << "Size of coincidence calo_records : " << _coincidence_calo_records_.size() << std::endl;
      std::clog << "Clocktick 1600 for element [0] : "   << _coincidence_calo_records_[0].clocktick_1600ns << std::endl;
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
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
