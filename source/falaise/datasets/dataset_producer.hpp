#ifndef SNEMO_DATASETS_DATASET_PRODUCER_HPP
#define SNEMO_DATASETS_DATASET_PRODUCER_HPP

// Standard library:
#include <string>
#include <functional>
#include <optional>
#include <iostream>

// This project:
#include <falaise/datasets/instance_id.hpp>
#include <falaise/datasets/dataset_producer_config.hpp>

namespace snemo {
  namespace datasets {

    /// Forward declaration
    struct dataset;
    struct dataset_producer_model;

    /// Dataset producer 
    struct dataset_producer 
    {

      typedef std::reference_wrapper<const dataset_producer_model>  producer_model_ref_type;
      typedef std::reference_wrapper<const dataset_producer_config> producer_config_ref_type;
      typedef std::reference_wrapper<const dataset>                 dataset_ref_type;
      typedef std::optional<producer_config_ref_type>               opt_producer_config_ref_type;
      // typedef std::optional<dataset_ref_type>                       opt_dataset_ref_type;
      
      dataset_producer() = delete;
   
      dataset_producer(const instance_id & id_,
		       producer_model_ref_type producer_model_,
		       opt_producer_config_ref_type configuration_opt_ = std::nullopt);
    
      // dataset_producer(const instance_id & id_,
      // 		       producer_model_ref_type producer_model_,
      // 		       //dataset_ref_type input_dataset_,
      // 		       opt_producer_config_ref_type configuration_opt_ = std::nullopt);

      const instance_id & id() const;
 
      const dataset_producer_model & producer_model() const;
  
      bool has_configuration() const;
   
      const dataset_producer_config & configuration() const;

      // bool has_input_dataset() const;

      // const dataset & input_dataset() const;
  
      void print_tree(std::ostream & out_) const;

    private:

      instance_id _id_;
      producer_model_ref_type _producer_model_; ///< Producer model
      opt_producer_config_ref_type _configuration_opt_; ///< Associated optional configuration for data producer
      //opt_dataset_ref_type _input_dataset_opt_; ///< Associated optional input dataset
      
    };
    
  } // namespace datasets
} // namespace snemo 

#endif // SNEMO_DATASETS_DATASET_PRODUCER_HPP
