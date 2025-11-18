#ifndef SNEMO_DATASETS_DATASET_HPP
#define SNEMO_DATASETS_DATASET_HPP

// Standard library:
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <iostream>

// This project:
#include <falaise/datasets/instance_id.hpp>
#include <falaise/datasets/datachunk.hpp>

namespace snemo {
  namespace datasets {

    struct dataset_model;
    struct dataset_producer;
    
    /// Dataset
    struct dataset  
    {
      typedef std::reference_wrapper<const dataset_model>    dataset_model_ref_type;
      typedef std::reference_wrapper<const dataset_producer> dataset_producer_ref_type;
      typedef std::reference_wrapper<const dataset>          dataset_ref_type;
      typedef std::optional<dataset_producer_ref_type>       opt_dataset_producer_ref_type;
      typedef std::optional<dataset_ref_type>                opt_dataset_ref_type;
 
      dataset() = delete;
 
      dataset(const instance_id & id_,
	      dataset_model_ref_type dataset_model_);

      dataset(const instance_id & id_,
	      dataset_model_ref_type dataset_model_,
	      dataset_producer_ref_type producer_,
	      opt_dataset_ref_type input_dataset_opt_ = std::nullopt);

      const instance_id & id() const;
  
      const dataset_model & model() const;
	
      bool has_producer() const;
      
      const dataset_producer & producer() const;

      bool has_input_dataset() const;

      const dataset & input_dataset() const;
 
      bool has_chunk(const instance_id & id_) const;
      
      void add_chunk(const datachunk & chunk_);

      void add_chunk(const instance_id & id_,
		     const std::string & path_,
		     datachunk::opt_nentries_type number_of_entries_opt_ = std::nullopt);
 
      const std::vector<datachunk> & chunks() const;
 
      bool has_number_of_entries() const;
 
      std::uint32_t number_of_entries() const;
      
      void print_tree(std::ostream & out_) const;

    private:
      
      instance_id _id_;
      dataset_model_ref_type _model_; ///< Associated dataset model to the dataset
      opt_dataset_producer_ref_type _producer_opt_; ///< Associated dataset producer (optional)
      opt_dataset_ref_type _input_dataset_opt_; ///< Associated optional input dataset
      std::vector<datachunk> _chunks_; ///< List of data chunks
  
    };
    
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_DATASET_HPP
