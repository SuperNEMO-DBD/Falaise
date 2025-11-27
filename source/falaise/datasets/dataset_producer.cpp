// Ourselves:
#include <falaise/datasets/dataset_producer.hpp>

// Standard library:
#include <stdexcept>
#include <iomanip>
#include <iostream>

// Third party
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/datasets/model_id.hpp>
#include <falaise/datasets/dataset.hpp>
#include <falaise/datasets/dataset_producer_model.hpp>
#include <falaise/datasets/dataset.hpp>

namespace snemo {
  namespace datasets {
       
    dataset_producer::dataset_producer(const instance_id & id_,
				       producer_model_ref_type producer_model_,
				       opt_producer_config_ref_type configuration_opt_)
      : _id_(id_)
      , _producer_model_(producer_model_)
      , _configuration_opt_(configuration_opt_)
    {
      // if (_producer_model_.get().has_input_data_model()) {
      // 	DT_THROW(std::logic_error,
      // 		 "Dataset producer model " << std::quoted(_producer_model_.get().id().to_string())
      // 		 << " requests some input data with model "
      // 		 << std::quoted(_producer_model_.get().input_data_model().id().to_string()));
      // }
      return;
    }

    /*
    dataset_producer::dataset_producer(const instance_id & id_,
				       producer_model_ref_type producer_model_,
				       dataset_ref_type input_dataset_,
				       opt_producer_config_ref_type configuration_opt_)
      : _id_(id_)
      , _producer_model_(producer_model_)
      , _configuration_opt_(configuration_opt_)
    {
      if (not _producer_model_.get().has_input_data_model()) {
	DT_THROW(std::logic_error,
		 "Attempt to pass input data with model " 
		 << std::quoted(input_dataset_.get().model().id().to_string())
		 << " to the dataset producer model " << std::quoted(_producer_model_.get().id().to_string()) 
		 << " which accepts none");
      }
      if (&input_dataset_.get().model() != &_producer_model_.get().input_data_model()) {
	auto idRepr = producer_model_.get().input_data_model().id().to_string();
	DT_THROW(std::logic_error,
		 "Input dataset with model " << std::quoted(input_dataset_.get().model().id().to_string()) << " [@" << &input_dataset_.get().model()
		 << "] is not of the expected model " << std::quoted(idRepr) << " [@" << &producer_model_.get().input_data_model() << ']');
       }
      _input_dataset_opt_ = input_dataset_;
      return;
    }
    */
    
    const instance_id & dataset_producer::id() const
    {
      return _id_;
    }

    const dataset_producer_model & dataset_producer::producer_model() const
    {
      return _producer_model_;
    }
  
    bool dataset_producer::has_configuration() const
    {
      return _configuration_opt_.has_value();
    }
  
    const dataset_producer_config & dataset_producer::configuration() const
    {
      return _configuration_opt_.value();
    }

    // bool dataset_producer::has_input_dataset() const
    // {
    //   return _input_dataset_opt_.has_value();
    // }

    // const dataset & dataset_producer::input_dataset() const
    // {
    //   return _input_dataset_opt_.value();
    // }
   
    void dataset_producer::print_tree(std::ostream & out_) const
    {
      out_ << "|-- " << "ID : " << std::quoted(_id_.to_string()) << " [@" << this << ']' << '\n';
      out_ << "|-- " << "Producer model : "
	   << std::quoted(_producer_model_.get().id().to_string()) << " [@" << &_producer_model_.get() << ']' << '\n';
      out_ << "`-- " << "Producer configuration : ";
      if (not has_configuration()) {
	out_ << "none";
      } else {
	out_ << std::quoted(_configuration_opt_.value().get().id().to_string()) << " [@" << &_configuration_opt_.value().get() << ']' ;
      }      
      out_ << '\n';
      // out_ << "`-- " << "Input dataset : ";
      // if (not has_input_dataset()) {
      // 	out_ << "none";
      // } else {
      // 	out_ << std::quoted(_input_dataset_opt_.value().get().id().to_string()) << " [@" << &_input_dataset_opt_.value().get() << ']' ;
      // }      
      // out_ << '\n';
      return;
    }

  } // namespace datasets 
} // namespace snemo 
