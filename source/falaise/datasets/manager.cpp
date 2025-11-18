// Ourselves:
#include <falaise/datasets/manager.hpp>

// Third party
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/datasets/factory.hpp>

namespace snemo {
  namespace datasets {

  
    void manager::initialize(const datatools::properties & config_)
    {
      verbosity = datatools::logger::extract_logging_configuration(config_);
      if (config_.has_flag("default_models")) {
	_init_dataset_models_();
	_init_dataset_producer_models_();
      }
      std::clog << "Embeded factory:\n";
      _fact_.print_tree(std::clog);
      return;      
    }
    
    void manager::_init_dataset_models_()
    {
      {
	boost::property_tree::ptree cbdParams;
	cbdParams.put("id", "cbd-1.0");
	cbdParams.put("description", "Raw DAQ output data model (CBD), version 1.0");
	_fact_.create("dataset_model", cbdParams);
	DT_LOG_INFORMATION(verbosity, "Declaring CBD-1.0 dataset model");
      }
      {
	boost::property_tree::ptree rtdParams;
	rtdParams.put("id", "rtd-1.0");
	rtdParams.put("description", "Raw Trigger Data model (RTD), version 1.0");
	_fact_.create("dataset_model", rtdParams);
	DT_LOG_INFORMATION(verbosity, "Declaring RTD-1.0 dataset model");
      }
      {
	boost::property_tree::ptree redParams;
	redParams.put("id", "red-1.0");
	redParams.put("description", "Raw Event Data model (RED), version 1.0");
	_fact_.create("dataset_model",redParams);
	DT_LOG_INFORMATION(verbosity, "Declaring RED-1.0 dataset model");
      }
      {
	boost::property_tree::ptree uddParams;
	uddParams.put("id", "udd-1.0");
	uddParams.put("description", "Unified Digitized Data model (UDD), version 1.0");
	_fact_.create("dataset_model", uddParams);
	DT_LOG_INFORMATION(verbosity, "Declaring UDD-1.0 dataset model");
      }

      return;
    }

    void manager::_init_dataset_producer_models_()
    {
      {
	boost::property_tree::ptree processModelParams;
	processModelParams.put("id", "daq-1.0");
	processModelParams.put("description", "DAQ process model, version 1.0");
	processModelParams.put("output_dataset_model_id", "cbd-1.0");
 	_fact_.create("dataset_producer_model", processModelParams);
	DT_LOG_INFORMATION(verbosity, "Declaring DAQ process model (version 1.0)");
      }
      {
	boost::property_tree::ptree processModelParams;
	processModelParams.put("id", "cbd2rtd-1.0");
	processModelParams.put("description", "CBD2RTD merger process model, version 1.0");
	processModelParams.put("output_dataset_model_id", "rtd-1.0");
	processModelParams.put("input_dataset_model_id", "cbd-1.0");
 	_fact_.create("dataset_producer_model", processModelParams);
	DT_LOG_INFORMATION(verbosity, "Declaring CBD2RTD process model (version 1.0)");
      }
      {
	boost::property_tree::ptree processModelParams;
	processModelParams.put("id", "eb-1.0");
	processModelParams.put("description", "Event builder process model, version 1.0");
	processModelParams.put("output_dataset_model_id", "red-1.0");
	processModelParams.put("input_dataset_model_id", "rtd-1.0");
 	_fact_.create("dataset_producer_model", processModelParams);
	DT_LOG_INFORMATION(verbosity, "Declaring event builder process model (version 1.0)");
      }
      {
	boost::property_tree::ptree processModelParams;
	processModelParams.put("id", "red2udd-1.0");
	processModelParams.put("description", "RED2UDD conversion process model, version 1.0");
	processModelParams.put("output_dataset_model_id", "udd-1.0");
	processModelParams.put("input_dataset_model_id", "red-1.0");
 	_fact_.create("dataset_producer_model", processModelParams);
	DT_LOG_INFORMATION(verbosity, "Declaring RED2UDD conversion process model (version 1.0)");
      }
      
      return;
    }

    void manager::reset()
    {
      return;
    }

    const factory & manager::fact() const
    {
      return _fact_;
    }

    factory & manager::fact()
    {
      return _fact_;
    }
    
  } // namespace datasets 
} // namespace snemo 
