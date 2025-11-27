// Ourselves:
#include <falaise/datasets/factory.hpp>

// Standard library:
#include <iomanip>
#include <typeinfo>

// Third party
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/datasets/model_id.hpp>
#include <falaise/datasets/instance_id.hpp>

namespace snemo {
  namespace datasets {
    
    bool factory::has_dataset_model(const std::string & key_) const
    {
      return _dataset_models_.count(key_);
    }
  
    bool factory::has_dataset_producer_model(const std::string & key_) const
    {
      return _dataset_producer_models_.count(key_);
    }
 
    bool factory::has_dataset_producer_config(const std::string & key_) const
    {
      return _dataset_producer_configs_.count(key_);
    }
 
    bool factory::has_dataset_producer(const std::string & key_) const
    {
      return _dataset_producers_.count(key_);
    }
 
    bool factory::has_dataset(const std::string & key_) const
    {
      return _datasets_.count(key_);
    }
 
    std::reference_wrapper<const dataset_model>
    factory::get_dataset_model_ref(const std::string & key_) const
    {
      return std::cref(*_dataset_models_.find(key_)->second.get());
    }
 
    std::reference_wrapper<const dataset_producer_model>
    factory::get_dataset_producer_model_ref(const std::string & key_) const
    {
      return std::cref(*_dataset_producer_models_.find(key_)->second.get());
    }
  
    std::reference_wrapper<const dataset_producer_config>
    factory::get_dataset_producer_config_ref(const std::string & key_) const
    {
      return std::cref(*_dataset_producer_configs_.find(key_)->second.get());
    }
   
    std::reference_wrapper<const dataset_producer>
    factory::get_dataset_producer_ref(const std::string & key_) const
    {
      return std::cref(*_dataset_producers_.find(key_)->second.get());
    }
  
    std::reference_wrapper<const dataset>
    factory::get_dataset_ref(const std::string & key_) const
    {
      return std::cref(*_datasets_.find(key_)->second.get());
    }
 
    void factory::create(const std::string & what_,
			 const boost::property_tree::ptree & parameters_)
    {
      if (what_ == "dataset_model") {
	/*****************************************************************/
	DT_LOG_DEBUG(verbosity, "Create a dataset model...");
	
	//  Unique ID:
	std::string idRepr = parameters_.get<std::string>("id");
	std::string description = parameters_.get<std::string>("description");
	model_id id;
	id.from_string(idRepr);
	std::string key = id.to_string();
	DT_THROW_IF(has_dataset_model(key), std::logic_error,
		    "Dataset model with ID=" << std::quoted(key) << " already exists");
	
	// Instantiation:
	std::shared_ptr<dataset_model> shPtr =
	  std::make_shared<dataset_model>(id, description);
	auto detailsPtree = parameters_.get_child("details", boost::property_tree::ptree());
	shPtr->details() = detailsPtree;

	// Registration:
 	_dataset_models_.insert({key, shPtr});
	_dataset_models_deps_.insert({key, {}});

	DT_LOG_DEBUG(verbosity, "new dataset model " << std::quoted(key) << " was inserted");

      } else if (what_ == "dataset_producer_model") {
	/*****************************************************************/
	DT_LOG_DEBUG(verbosity, "Create a dataset model producer...");

	//  Unique ID:
	std::string idRepr = parameters_.get<std::string>("id");
	std::string description = parameters_.get<std::string>("description");
	model_id id;
	id.from_string(idRepr);
	std::string key = id.to_string();
	DT_THROW_IF(has_dataset_model(key), std::logic_error,
		    "Dataset producer model with ID=" << std::quoted(key) << " already exists");

	// Mandatory output dataset model:
	std::string outputDatasetModelIdRepr = parameters_.get<std::string>("output_dataset_model_id");
	model_id outputDatasetModelId;
	outputDatasetModelId.from_string(outputDatasetModelIdRepr);
	DT_THROW_IF(not has_dataset_model(outputDatasetModelId.to_string()),
		    std::logic_error,
		    "Undefined output dataset model with ID=" << std::quoted(outputDatasetModelId.to_string()));
	dataset_producer_model::dataset_model_ref_type outputDatasetModelCref
	  = get_dataset_model_ref(outputDatasetModelId.to_string());

	// Optional input dataset model:
	dataset_producer_model::opt_dataset_model_ref_type inputDatasetModelCrefOpt;
	auto inputDatasetModelIdReprOpt
	  = parameters_.get_optional<std::string>("input_dataset_model_id");
	if (inputDatasetModelIdReprOpt.has_value()) {
	  model_id inputDatasetModelId;
	  inputDatasetModelId.from_string(inputDatasetModelIdReprOpt.value());
	  DT_THROW_IF(not has_dataset_model(inputDatasetModelId.to_string()),
		      std::logic_error,
		      "Undefined input dataset model with ID=" << std::quoted(inputDatasetModelId.to_string()));
	  inputDatasetModelCrefOpt = get_dataset_model_ref(inputDatasetModelId.to_string());
	}

	// Instantiation:
	std::shared_ptr<const dataset_producer_model> shPtr
	  = std::make_shared<const dataset_producer_model>(id,
							   description,
							   outputDatasetModelCref,
							   inputDatasetModelCrefOpt);

	// Registration:
 	_dataset_producer_models_.insert({key, shPtr});
	_dataset_producer_models_deps_.insert({key, {}});

	// Manage dependencies:
	_dataset_models_deps_.find(outputDatasetModelId.to_string())->second.dataset_producer_model_ids.push_back(key);
	if (inputDatasetModelIdReprOpt.has_value()) {
	  _dataset_models_deps_.find(inputDatasetModelIdReprOpt.value())->second.dataset_producer_model_ids.push_back(key);
	}
	
	DT_LOG_DEBUG(verbosity, "new dataset producer model " << std::quoted(key) << " was inserted");

      } else if (what_ == "dataset_producer_config") {
	/*****************************************************************/
	DT_LOG_DEBUG(verbosity, "Create a dataset model config...");

	// Unique ID:
	std::string idRepr = parameters_.get<std::string>("id");
	instance_id id;
	id.from_string(idRepr);
	std::string key = id.to_string();
	DT_THROW_IF(has_dataset_producer_config(key), std::logic_error,
		    "Dataset producer config with ID=" << std::quoted(key) << " already exists");

       	// Mandatory dataset producer model:
	std::string outputDatasetProducedModelIdRepr
	  = parameters_.get<std::string>("producer_model_id");
	model_id outputDatasetProducedModelId;
	outputDatasetProducedModelId.from_string(outputDatasetProducedModelIdRepr);
	DT_THROW_IF(not has_dataset_producer_model(outputDatasetProducedModelId.to_string()),
		    std::logic_error,
		    "Undefined output dataset producer model with ID="
		    << std::quoted(outputDatasetProducedModelId.to_string()));
	auto outputDatasetProducerModelCref
	  = get_dataset_producer_model_ref(outputDatasetProducedModelId.to_string());

	// Instantiation:
	std::shared_ptr<dataset_producer_config> shPtr
	  = std::make_shared<dataset_producer_config>(id, outputDatasetProducerModelCref);
	//if (XXX) {
	auto parametersPtree = parameters_.get_child("parameters", boost::property_tree::ptree());
	shPtr->parameters() = parametersPtree;
	  //}
	// Registration:
 	_dataset_producer_configs_.insert({key, shPtr});
	_dataset_producer_configs_deps_.insert({key, {}});

	// Manage dependencies:
	_dataset_producer_models_deps_.find(outputDatasetProducedModelId.to_string())->second.dataset_producer_config_ids.push_back(key);
	
	DT_LOG_DEBUG(verbosity, "new dataset producer configuration " << std::quoted(key) << " was inserted");
	
      } else if (what_ == "dataset_producer") {
	/*****************************************************************/
	DT_LOG_DEBUG(verbosity, "Create a dataset_producer...");
	if (datatools::logger::is_debug(verbosity)) {
	  for (const auto & it: parameters_) {
	    std::cerr << "[debug] + parameter = " << it.first << '\n';
	  }
	}
	
	//  Unique ID:
	std::string idRepr = parameters_.get<std::string>("id");
	instance_id id;
	id.from_string(idRepr);
	DT_LOG_DEBUG(verbosity, "Instance ID = " << id.to_string());
	std::string key = id.to_string();
	DT_THROW_IF(has_dataset_producer(key), std::logic_error,
		    "Dataset producer with ID=" << std::quoted(key) << " already exists");

	// Mandatory dataset producer model:
	std::string producerModelIdRepr
	  = parameters_.get<std::string>("producer_model_id");
	DT_LOG_DEBUG(verbosity, "producerModelIdRepr = " << std::quoted(producerModelIdRepr));
	model_id producerModelId;
	producerModelId.from_string(producerModelIdRepr);
	DT_THROW_IF(not has_dataset_producer_model(producerModelId.to_string()),
		    std::logic_error,
		    "Undefined output dataset producer model with ID="
		    << std::quoted(producerModelId.to_string()));
	auto producerModelCref
	  = get_dataset_producer_model_ref(producerModelId.to_string());
	DT_LOG_DEBUG(verbosity,
		     "producer model ID = "
		     << std::quoted(producerModelCref.get().id().to_string()));

	// Optional dataset producer config:
	dataset_producer::opt_producer_config_ref_type configCrefOpt;
	auto configIdReprOpt /*boost::optional<std::string>*/
	  = parameters_.get_optional<std::string>("producer_configuration_id");
	DT_LOG_DEBUG(verbosity,
		     typeid(configIdReprOpt).name() <<  " : has value = " << std::boolalpha
		     << configIdReprOpt.has_value());
	if (configIdReprOpt.has_value()) {
	  DT_LOG_DEBUG(verbosity,"value = " << configIdReprOpt.value());
	  instance_id configId;
	  configId.from_string(configIdReprOpt.value());
	  DT_THROW_IF(not has_dataset_producer_config(configId.to_string()),
		      std::logic_error,
		      "Undefined configuration with ID="
		      << std::quoted(configId.to_string()));
	  configCrefOpt = get_dataset_producer_config_ref(configId.to_string());
	  DT_LOG_DEBUG(verbosity,
		       "configCrefOpt = "
		       << std::quoted(configCrefOpt.value().get().id().to_string()));
	}
	
	// Instantiation:
	std::shared_ptr<dataset_producer> shPtr;
	shPtr = std::make_shared<dataset_producer>(id,
						   producerModelCref,
						   configCrefOpt);

	// Registration:
 	_dataset_producers_.insert({key, shPtr});
	_dataset_producers_deps_.insert({key, {}});

	// Manage dependencies:
	_dataset_producer_models_deps_.find(producerModelId.to_string())->second.dataset_producer_ids.push_back(key);
	if (configIdReprOpt.has_value()) {
	  _dataset_producer_configs_deps_.find(configIdReprOpt.value())->second.dataset_producer_ids.push_back(key);
	}
	
	DT_LOG_DEBUG(verbosity, "new dataset producer " << std::quoted(key) << " was inserted");

      } else if (what_ == "dataset") {
	/*****************************************************************/
	DT_LOG_DEBUG(verbosity, "Create a dataset...");
	if (datatools::logger::is_debug(verbosity)) {
	  for (const auto & it: parameters_) {
	    std::cerr << "[debug] + parameter = " << it.first << '\n';
	  }
	}

	//  Unique ID:
	std::string idRepr = parameters_.get<std::string>("id");
	instance_id id;
	id.from_string(idRepr);
	std::string key = id.to_string();
	DT_THROW_IF(has_dataset(key), std::logic_error,
		    "Dataset with ID=" << std::quoted(key) << " already exists");
	DT_LOG_DEBUG(verbosity, "Instance ID = " << id.to_string());

	// Mandatory dataset model:
	auto datasetModelIdRepr = parameters_.get<std::string>("dataset_model_id");
	model_id datasetModelId;
	datasetModelId.from_string(datasetModelIdRepr);
	DT_LOG_DEBUG(verbosity, "datasetModelId ID = " << std::quoted(datasetModelId.to_string()));
	DT_THROW_IF(not has_dataset_model(datasetModelId.to_string()),
		    std::logic_error,
		    "Undefined dataset model with ID=" << std::quoted(datasetModelId.to_string()));
	dataset::dataset_model_ref_type datasetModelCref = get_dataset_model_ref(datasetModelId.to_string());

	// Optional dataset producer and input dataset:
	dataset::opt_dataset_producer_ref_type datasetProducerCrefOpt;
	dataset::opt_dataset_ref_type datasetCrefOpt;
	auto datasetProducerIdReprOpt
	  = parameters_.get_optional<std::string>("dataset_producer_id");
	if (datasetProducerIdReprOpt.has_value()) {
	  instance_id datasetProducerId;
	  datasetProducerId.from_string(datasetProducerIdReprOpt.value());
	  DT_LOG_DEBUG(verbosity, "datasetProducerId ID = " << std::quoted(datasetProducerId.to_string()));
	  DT_THROW_IF(not has_dataset_producer(datasetProducerId.to_string()),
		      std::logic_error,
		      "Undefined dataset producer with ID=" << std::quoted(datasetProducerId.to_string()));
	  datasetProducerCrefOpt = get_dataset_producer_ref(datasetProducerId.to_string());

	  // Optional input dataset:
	  auto datasetIdReprOpt = parameters_.get_optional<std::string>("input_dataset_id");
	  DT_LOG_DEBUG(verbosity,
		       typeid(datasetIdReprOpt).name() <<  " : has value = " << std::boolalpha
		       << datasetIdReprOpt.has_value());
	  if (datasetIdReprOpt.has_value()) {
	    instance_id datasetId;
	    datasetId.from_string(datasetIdReprOpt.value());
	    DT_THROW_IF(not has_dataset(datasetId.to_string()),
			std::logic_error,
			"Undefined dataset with ID="
			<< std::quoted(datasetId.to_string()));
	    datasetCrefOpt = get_dataset_ref(datasetId.to_string());
	  } else {
	    DT_LOG_DEBUG(verbosity, "No input dataset");
	  }
	}

	// Instantation:
	std::shared_ptr<dataset> shPtr;
	if (datasetProducerCrefOpt.has_value()) {
	  shPtr = std::make_shared<dataset>(id,
					    datasetModelCref,
					    datasetProducerCrefOpt.value(),
					    datasetCrefOpt);
	} else {
	  shPtr = std::make_shared<dataset>(id, datasetModelCref);
	}
	std::uint32_t datasetNbChunks = parameters_.get<std::uint32_t>("nchunks", 0u);
	DT_LOG_DEBUG(verbosity, "Number of chunks = " << datasetNbChunks);
	if (datasetNbChunks > 0) {
	  for (uint32_t iChunk = 0u; iChunk < datasetNbChunks; iChunk++) {
	    std::string chunkPrefix("chunks[");
	    chunkPrefix += std::to_string(iChunk);
	    chunkPrefix += "]"; 
	    std::string chunkIdKey = chunkPrefix + ".id";
	    std::string chunkPathKey = chunkPrefix + ".path";
	    std::string chunkNbEntriesKey = chunkPrefix + ".nentries";
	    DT_LOG_DEBUG(verbosity, "Searching chunk ID key = " << std::quoted(chunkIdKey));
	    std::string chunkIdValue = parameters_.get<std::string>(chunkIdKey);
	    instance_id chunkId;
	    chunkId.from_string(chunkIdValue);
	    std::string chunkPathValue = parameters_.get<std::string>(chunkPathKey);
	    auto chunkNbEntriesValueOptB = parameters_.get_optional<std::uint32_t>(chunkNbEntriesKey);
	    datachunk::opt_nentries_type chunkNbEntriesValueOpt;
	    if (chunkNbEntriesValueOptB) {
	      chunkNbEntriesValueOpt = chunkNbEntriesValueOptB.value();
	    }
	    shPtr->add_chunk(chunkId, chunkPathValue, chunkNbEntriesValueOpt);
	  }
	}
	
	// Registration:
 	_datasets_.insert({key, shPtr});
	_datasets_deps_.insert({key, {}});

	// Manage dependencies:
	DT_LOG_DEBUG(verbosity, "Manage dependencies...");
	_dataset_models_deps_.find(datasetModelId.to_string())->second.dataset_ids.push_back(key);
	DT_LOG_DEBUG(verbosity, "Added dependant in dataset model " << std::quoted(datasetModelId.to_string()));
	if (datasetCrefOpt.has_value()) {
	  _datasets_deps_.find(datasetCrefOpt.value().get().id().to_string())->second.dataset_ids.push_back(key);
	}
	DT_LOG_DEBUG(verbosity, "dataset producers' deps map size = " << _dataset_producers_deps_.size());
	if (datasetProducerIdReprOpt.has_value()) {
	  DT_LOG_DEBUG(verbosity, "item to be updated " << std::quoted(datasetProducerIdReprOpt.value()));
	  _dataset_producers_deps_.find(datasetProducerIdReprOpt.value())->second.dataset_ids.push_back(key);
	  DT_LOG_DEBUG(verbosity, "Added dependant in dataset producer " << std::quoted(datasetProducerIdReprOpt.value()));
	}
	
	DT_LOG_DEBUG(verbosity, "new dataset " << std::quoted(key) << " was inserted");
      } else {
	DT_THROW(std::logic_error, "Unsupported type of object " << std::quoted(what_));
      }

      return;
    }

    void factory::dependency_record::print(std::ostream & out_, const std::string & indent_) const
    {
      auto totDeps = 0u;
      if (dataset_model_ids.size()) {
	out_ << indent_ << "|-- Dataset models: " << dataset_model_ids.size() << '\n';
	totDeps += dataset_model_ids.size();
      }
      if (dataset_producer_model_ids.size()) {
	out_ << indent_ << "|-- Dataset producer models: " << dataset_producer_model_ids.size() << '\n'; 
	totDeps += dataset_producer_model_ids.size();
      }
      if (dataset_producer_config_ids.size()) {
	out_ << indent_ << "|-- Dataset producer configs: " << dataset_producer_config_ids.size() << '\n'; 
	totDeps += dataset_producer_config_ids.size();
      }
      if (dataset_producer_ids.size()) {
	out_ << indent_ << "|-- Dataset producer ids: " << dataset_producer_ids.size() << '\n'; 
	totDeps += dataset_producer_ids.size();
      }
      if (dataset_ids.size()) {
	out_ << indent_ << "|-- Dataset ids: " << dataset_ids.size() << '\n'; 
	totDeps += dataset_ids.size();
      }
      out_ << indent_ << "`-- Total: " << totDeps << '\n'; 
      return;
    }

    std::uint32_t factory::dependency_record::number_of_dependers() const
    {
      return dataset_model_ids.size()
	+ dataset_producer_model_ids.size()
	+ dataset_producer_config_ids.size()
	+ dataset_producer_ids.size()
	+ dataset_ids.size();
    }
    
    void factory::print_tree(std::ostream & out_) const
    {
      out_ << "|-- Dataset models: " << _dataset_models_.size() << '\n';
      for (const auto & item : _dataset_models_) {
	out_ << "|   + Dataset model " << std::quoted(item.first)
	     << " [" << _dataset_models_deps_.find(item.first)->second.number_of_dependers() << " dependers]"
	     << '\n';
      }
      out_ << "|   |-- Dependencies:\n";  
      for (const auto & item : _dataset_models_deps_) {
	out_ << "|   |   + Dataset model " << std::quoted(item.first)
	     << '\n';
	item.second.print(out_, "|   |   ");
      }
      out_ << "|   + end\n";  
      
      out_ << "|-- Dataset producer models: " << _dataset_producer_models_.size()
	   << '\n';
      for (const auto & item : _dataset_producer_models_) {
	out_ << "|   + Dataset producer model " << std::quoted(item.first)
	     << " [" << _dataset_producer_models_deps_.find(item.first)->second.number_of_dependers() << " dependers]"
	     << '\n';
      }
      out_ << "|   |-- Dependencies:\n";  
      for (const auto & item : _dataset_producer_models_deps_) {
	out_ << "|   |   + Dataset producer model " << std::quoted(item.first)
	     << '\n';
	item.second.print(out_, "|   |   ");
      }
      out_ << "|   + end\n";  
      
      out_ << "|-- Dataset producer configs: " << _dataset_producer_configs_.size() << '\n';
      for (const auto & item : _dataset_producer_configs_) {
	out_ << "|   + Dataset producer config " << std::quoted(item.first) 
	     << " [" << _dataset_producer_configs_deps_.find(item.first)->second.number_of_dependers() << " dependers]"
	     << '\n';
      }
      out_ << "|   |-- Dependencies:\n";  
      for (const auto & item : _dataset_producer_configs_deps_) {
	out_ << "|   |   + Dataset producer config " << std::quoted(item.first)
	     << '\n';
	item.second.print(out_, "|   |   ");
      }
      out_ << "|   + end\n";  
    
      out_ << "|-- Dataset producers: " << _dataset_producers_.size() << '\n';
      for (const auto & item : _dataset_producers_) {
	out_ << "|   + Dataset producer " << std::quoted(item.first)
	     << " [" << _dataset_producers_deps_.find(item.first)->second.number_of_dependers() << " dependers]"
	     << '\n';
      }
      out_ << "|   |-- Dependencies:\n";  
      for (const auto & item : _dataset_producers_deps_) {
	out_ << "|   |   + Dataset producer " << std::quoted(item.first)
	     << '\n';
	item.second.print(out_, "|   |   ");
      }
      out_ << "|   + end\n";  
    
      out_ << "|-- Datasets: " << _datasets_.size() << '\n';
      for (const auto & item : _datasets_) {
	out_ << "|   + Dataset " << std::quoted(item.first)
	     << " [" << _datasets_deps_.find(item.first)->second.number_of_dependers() << " dependers]"
	     << '\n';
      }
      out_ << "|   |-- Dependencies:\n";  
      for (const auto & item : _datasets_deps_) {
	out_ << "|   |   + Dataset " << std::quoted(item.first)
	     << '\n';
	item.second.print(out_, "|   |   ");
      }
      out_ << "|   + end\n";  

      out_ << "`-- end." << '\n';
      return;
    }

  } // namespace datasets 
} // namespace snemo 
