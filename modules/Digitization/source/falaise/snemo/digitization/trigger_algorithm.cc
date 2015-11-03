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
      _tracker_algo_.set_electronic_mapping(my_electronic_mapping_);
      _calo_algo_.set_electronic_mapping(my_electronic_mapping_);
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

      datatools::properties calo_config;
      config_.export_and_rename_starting_with(calo_config, "calo.", "");
      _calo_algo_.initialize(calo_config);

      datatools::properties tracker_config;
      config_.export_and_rename_starting_with(tracker_config, "tracker.", "");
      _tracker_algo_.initialize(tracker_config);

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

    const std::vector<tracker_trigger_algorithm::tracker_record>trigger_algorithm::get_tracker_records_vector() const
    {
      return _tracker_records_;
    }    
    
    const std::vector<calo_trigger_algorithm::calo_summary_record> trigger_algorithm::get_calo_records_vector() const  
    {
      return _calo_records_;
    }

    void trigger_algorithm::process(const geiger_ctw_data & geiger_ctw_data_,
				    const calo_ctw_data & calo_ctw_data_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Trigger algorithm is not initialized, it can't process ! ");
      _process(geiger_ctw_data_,
	       calo_ctw_data_);
      return;
    }   

    void trigger_algorithm::_process_calo_algo(const calo_ctw_data & calo_ctw_data_,
					       std::vector<calo_trigger_algorithm::calo_summary_record> calo_records_)
    {
      _calo_algo_.process(calo_ctw_data_);
      return;
    }

    void trigger_algorithm::_process_tracker_algo(const geiger_ctw_data & geiger_ctw_data_,
						  std::vector<tracker_trigger_algorithm::tracker_record> tracker_records_)
    {
      _tracker_algo_.process(geiger_ctw_data_);
      return;
    }
    
    void trigger_algorithm::_process_coinc_algo(std::vector<tracker_trigger_algorithm::tracker_record> tracker_records_,
						std::vector<calo_trigger_algorithm::calo_summary_record> calo_records_)
    {
      return;
    }
    
    void trigger_algorithm::_process(const geiger_ctw_data & geiger_ctw_data_,
				     const calo_ctw_data & calo_ctw_data_)
    {
      if (calo_ctw_data_.has_calo_ctw())
	{
	  _process_calo_algo(calo_ctw_data_, _calo_records_);
	}

      if (geiger_ctw_data_.has_geiger_ctw())
	{
	  _process_tracker_algo(geiger_ctw_data_, _tracker_records_);
	}
       return;
    }
  } // end of namespace digitization

} // end of namespace snemo
