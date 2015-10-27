// snemo/digitization/trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_algorithm.h>

namespace snemo {
  
  namespace digitization {

    trigger_algorithm::trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      return;
    }

    trigger_algorithm::~trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void trigger_algorithm::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    void trigger_algorithm::initialize()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized ! ");
      DT_THROW_IF(_electronic_mapping_ == 0, std::logic_error, "Missing electronic mapping ! " );

      _initialized_ = true;
      return;
    }

    bool trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      return;
    }

    void trigger_algorithm::process(const calo_trigger_algorithm::trigger_summary_record & trigger_sumary_record_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't process ! ");
      
      return;
    }

    void trigger_algorithm::_process(const calo_trigger_algorithm::trigger_summary_record & trigger_sumary_record_)
    {
      return;
    }
  } // end of namespace digitization

} // end of namespace snemo
