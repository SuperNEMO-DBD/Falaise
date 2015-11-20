// snemo/digitization/trigger_display_manager.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/trigger_display_manager.h>

namespace snemo {
  
  namespace digitization {
    
    trigger_display_manager::trigger_display_manager()
    {
      _initialized_              = false;
      _calo_25ns_        = false;
      _calo_1600ns_      = false;
      _tracker_1600ns_   = false;
      _coinc_1600ns_     = false;
      _decision_trigger_ = false;
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
      if (!is_calo_25ns()) {
	if (config_.has_key("calo_25ns")) {
	  bool calo_25ns_config = config_.fetch_boolean("calo_25ns");
	  _calo_25ns_ = calo_25ns_config;
	}
      }

      if (!is_calo_1600ns()) {
	if (config_.has_key("calo_1600ns")) {
	  bool calo_1600ns_config = config_.fetch_boolean("calo_1600ns");
	  _calo_1600ns_ = calo_1600ns_config;
	}
      }
      
      if (!is_tracker_1600ns()) {
	if (config_.has_key("tracker_1600ns")) {
	  bool tracker_1600ns_config = config_.fetch_boolean("tracker_1600ns");
	  _tracker_1600ns_ = tracker_1600ns_config;
	}
      }
      
      if (!is_coinc_1600ns()) {
	if (config_.has_key("coinc_1600ns")) {
	  bool coinc_1600ns_config = config_.fetch_boolean("coinc_1600ns");
	  _coinc_1600ns_ = coinc_1600ns_config;
	}
      }

      if (!is_decision_trigger()) {
	if (config_.has_key("decision_trigger")) {
	  bool decision_trigger_config = config_.fetch_boolean("decision_trigger");
	  _decision_trigger_ = decision_trigger_config;
	}
      }
      
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
      _initialized_      = false;
      _calo_25ns_        = false;
      _calo_1600ns_      = false;
      _tracker_1600ns_   = false;
      _coinc_1600ns_     = false;
      _decision_trigger_ = false;
      return;
    }

    const bool trigger_display_manager::is_calo_25ns() const
    {
      return _calo_25ns_;
    }
    const bool trigger_display_manager::is_calo_1600ns() const
    {
      return _calo_1600ns_; 
    }
    const bool trigger_display_manager::is_tracker_1600ns() const
    {
      return _tracker_1600ns_;
    }
    const bool trigger_display_manager::is_coinc_1600ns() const
    {
      return _coinc_1600ns_;
    }
    const bool trigger_display_manager::is_decision_trigger() const
    {
      return _decision_trigger_;
    }

    void trigger_display_manager::display_calo_trigger_25ns() const
    {
      DT_THROW_IF(!is_calo_25ns(), std::logic_error, "Boolean calo 25ns is not activated, it can't display ! ");
      
      return;
    }

    void trigger_display_manager::display_calo_trigger_1600ns() const
    {
      DT_THROW_IF(!is_calo_1600ns(), std::logic_error, "Boolean calo 1600ns is not activated, it can't display ! ");
      return;
    }
			
    void trigger_display_manager::display_tracker_trigger_1600ns() const
    {
      DT_THROW_IF(!is_tracker_1600ns(), std::logic_error, "Boolean tracker 1600ns is not activated, it can't display ! ");
      return;
    }

    void trigger_display_manager::display_coincidence_trigger_1600ns() const
    {
      DT_THROW_IF(!is_coinc_1600ns(), std::logic_error, "Boolean coinc 1600ns is not activated, it can't display ! ");
      return;
    }

    void trigger_display_manager::display_decision_trigger() const			
    {
      DT_THROW_IF(!is_decision_trigger(), std::logic_error, "Boolean decision trigger is not activated, it can't display ! ");
      return;
    }
  
  } // end of namespace digitization

} // end of namespace snemo
