#ifndef SNEMO_DATASETS_DATASET_PRODUCER_CONFIG_HPP
#define SNEMO_DATASETS_DATASET_PRODUCER_CONFIG_HPP

// Standard library:
#include <string>
#include <functional>
#include <iostream>

// Third party:
#include <boost/property_tree/ptree.hpp>

// This project:
#include <falaise/datasets/instance_id.hpp>
#include <falaise/datasets/dataset_producer_model.hpp>

namespace snemo {
  namespace datasets {

    /// Configuration of a dataset producer
    struct dataset_producer_config
    {
 
      typedef std::reference_wrapper<const dataset_producer_model> dataset_producer_model_ref_type;
      
      dataset_producer_config() = delete;
  
      dataset_producer_config(const instance_id & id_,
			      dataset_producer_model_ref_type producer_model_);

      const instance_id & id() const;

      const dataset_producer_model & producer_model() const;
  
      const boost::property_tree::ptree & parameters() const;

      boost::property_tree::ptree & parameters();

      void print_tree(std::ostream & out_) const;

    private:
  
      instance_id _id_;
      dataset_producer_model_ref_type _producer_model_; ///< Associated producer model
      boost::property_tree::ptree _parameters_; ///< List of configuration parameters

    };

  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_DATASET_PRODUCER_CONFIG_HPP
