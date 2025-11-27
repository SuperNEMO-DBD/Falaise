#ifndef SNEMO_DATASETS_FACTORY_HPP
#define SNEMO_DATASETS_FACTORY_HPP

// Standard library:
#include <string>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

// Third party:
#include <boost/property_tree/ptree.hpp>
#include <bayeux/datatools/logger.h>

// This project:
#include <falaise/datasets/model_id.hpp>
#include <falaise/datasets/dataset_model.hpp>
#include <falaise/datasets/dataset_producer_model.hpp>
#include <falaise/datasets/dataset_producer_config.hpp>
#include <falaise/datasets/dataset_producer.hpp>
#include <falaise/datasets/dataset.hpp>

namespace snemo {
  namespace datasets {

    struct factory
    {
      factory() = default;

      void create(const std::string & what_,
		  const boost::property_tree::ptree & parameters_);

      bool has_dataset_model(const std::string & key_) const;
 
      bool has_dataset_producer_model(const std::string & key_) const;

      bool has_dataset_producer_config(const std::string & key_) const;

      bool has_dataset_producer(const std::string & key_) const;

      bool has_dataset(const std::string & key_) const;

      std::reference_wrapper<const dataset_model> get_dataset_model_ref(const std::string & key_) const;
 
      std::reference_wrapper<const dataset_producer_model> get_dataset_producer_model_ref(const std::string & key_) const;
  
      std::reference_wrapper<const dataset_producer_config> get_dataset_producer_config_ref(const std::string & key_) const;
   
      std::reference_wrapper<const dataset_producer> get_dataset_producer_ref(const std::string & key_) const;
  
      std::reference_wrapper<const dataset> get_dataset_ref(const std::string & key_) const;
      
      void print_tree(std::ostream & out_) const;

      void is_removable_dataset(const std::string & key_);
      
      void remove_dataset(const std::string & key_);

    public:
      
      datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;

      struct dependency_record
      {
	dependency_record() = default;
	std::uint32_t number_of_dependers() const;
 	void print(std::ostream & out_, const std::string & indent_ = "") const;
	std::vector<std::string> dataset_model_ids;
	std::vector<std::string> dataset_producer_model_ids;
	std::vector<std::string> dataset_producer_config_ids;
	std::vector<std::string> dataset_producer_ids;
	std::vector<std::string> dataset_ids;
      };
      
    private:

      std::map<std::string, std::shared_ptr<const dataset_model>> _dataset_models_;
      std::map<std::string, dependency_record> _dataset_models_deps_;
      
      std::map<std::string, std::shared_ptr<const dataset_producer_model>> _dataset_producer_models_;
      std::map<std::string, dependency_record> _dataset_producer_models_deps_;
       
      std::map<std::string, std::shared_ptr<const dataset_producer_config>> _dataset_producer_configs_;
      std::map<std::string, dependency_record> _dataset_producer_configs_deps_;
      
      std::map<std::string, std::shared_ptr<const dataset_producer>> _dataset_producers_;
      std::map<std::string, dependency_record> _dataset_producers_deps_;
      
      std::map<std::string, std::shared_ptr<const dataset>> _datasets_;
      std::map<std::string, dependency_record> _datasets_deps_;
    
    };
  
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_FACTORY_HPP
