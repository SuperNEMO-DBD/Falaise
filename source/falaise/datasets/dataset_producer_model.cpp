// Ourselves:
#include <falaise/datasets/dataset_producer_model.hpp>

// Third party:
#include <bayeux/datatools/exception.h>

// Standard library:
#include <stdexcept>
#include <iomanip>

namespace snemo {
  namespace datasets {
    
    dataset_producer_model::dataset_producer_model(const model_id & id_,
						   const std::string & description_,
						   dataset_model_ref_type output_data_model_,
						   opt_dataset_model_ref_type input_data_model_)
      : _id_(id_)
      , _description_(description_)
      , _output_data_model_(output_data_model_)
    {
      if (input_data_model_.has_value()) {
	_input_data_model_opt_ = input_data_model_.value();
      }
      return;
    }
  
    const model_id & dataset_producer_model::id() const
    {
      return _id_;
    }
  
    const std::string & dataset_producer_model::description() const
    {
      return _description_;
    }
  
    const dataset_model & dataset_producer_model::output_data_model() const
    {
      return _output_data_model_;
    }
  
    bool dataset_producer_model::has_input_data_model() const
    {
      return _input_data_model_opt_.has_value();
    }
  
    const dataset_model & dataset_producer_model::input_data_model() const
    {
      if (not has_input_data_model()) {
	DT_THROW(std::logic_error, "No input data model");
      }
      return _input_data_model_opt_.value();
    }
 
    void dataset_producer_model::print_tree(std::ostream & out_) const
    {
      out_ << "|-- " << "ID : " << std::quoted(_id_.to_string()) << " [@" << this << ']' << '\n';
      out_ << "|-- " << "Description : " << std::quoted(_description_) << '\n';
      out_ << "|-- " << "Output data model : "
	   << std::quoted(_output_data_model_.get().id().to_string()) << " [@" << &_output_data_model_.get() << ']' << '\n';
      out_ << "`-- " << "Input data model : ";
      if (not has_input_data_model()) {
	out_ << "none";
      } else {
	out_ << std::quoted(_input_data_model_opt_.value().get().id().to_string()) << " [@" << &_input_data_model_opt_.value().get() << ']' ;
      }
      out_ << '\n';
      return;
    }

  } // namespace datasets
} // namespace snemo 
