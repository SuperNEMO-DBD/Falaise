// snemo/digitization/trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm.h>

namespace snemo {
  
  namespace digitization {

    coincidence_trigger_algorithm::coincidence_calo_record::coincidence_calo_record()
    {
      clocktick_1600ns = 0;
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

    void coincidence_trigger_algorithm::preparing_calo_coincidence(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_)
    {
      std::clog << "size of vector : " << calo_records_.size() << std::endl;
      coincidence_calo_record my_coinc_calo_record;
      
      return;
    }
    
    void coincidence_trigger_algorithm::process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
						const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      
      return;
    }

    void coincidence_trigger_algorithm::_process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
						 const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_)
    {

      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
