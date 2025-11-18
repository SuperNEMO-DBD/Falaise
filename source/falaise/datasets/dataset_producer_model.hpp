#ifndef SNEMO_DATASETS_DATASET_PRODUCER_MODEL_HPP
#define SNEMO_DATASETS_DATASET_PRODUCER_MODEL_HPP

// Standard library:
#include <string>
#include <functional>
#include <optional>
#include <iostream>

// This project:
#include <falaise/datasets/model_id.hpp>
#include <falaise/datasets/dataset_model.hpp>

namespace snemo {
  namespace datasets {

    /// Dataset producer model
    struct dataset_producer_model  
    {
 
      typedef std::reference_wrapper<const dataset_model> dataset_model_ref_type;
      typedef std::optional<dataset_model_ref_type>       opt_dataset_model_ref_type;
 
      dataset_producer_model() = delete;

      dataset_producer_model(const model_id & id_,
			     const std::string & description_,
			     dataset_model_ref_type output_data_model_,
			     opt_dataset_model_ref_type input_data_model_ = std::nullopt);
        
      const model_id & id() const;

      const std::string & description() const;
  
      const dataset_model & output_data_model() const;
       
      bool has_input_data_model() const;
 
      const dataset_model & input_data_model() const;

      void print_tree(std::ostream & out_) const;

    private:

      model_id _id_; ///< Model ID
      std::string _description_; ///< Description of the model
      dataset_model_ref_type _output_data_model_; ///< Reference to the output dataset model
      opt_dataset_model_ref_type _input_data_model_opt_; ///< Optional reference to the input dataset model

    };
 
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_DATASET_PRODUCER_MODEL_HPP
