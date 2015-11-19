// snemo/digitization/trigger_display_manager.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_display_manager.h>

namespace snemo {
  
  namespace digitization {
    
    trigger_display_manager::trigger_display_manager()
    {
      return;
    }

    trigger_display_manager::~trigger_display_manager()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void trigger_display_manager::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void trigger_display_manager::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Trigger display manager is already initialized ! ");
     
      _initialized_ = true;
      return;
    }

    bool trigger_display_manager::is_initialized() const
    {
      return _initialized_;
    }

    void trigger_display_manager::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger display manager is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }


  
  } // end of namespace digitization

} // end of namespace snemo
