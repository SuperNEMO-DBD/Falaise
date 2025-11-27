#ifndef SNEMO_DATASETS_DATASET_MODEL_HPP
#define SNEMO_DATASETS_DATASET_MODEL_HPP

// Standard library:
#include <string>
#include <iostream>

// Third party:
#include <boost/property_tree/ptree.hpp>

// This project:
#include <falaise/datasets/model_id.hpp>

namespace snemo {
  namespace datasets {

    /// Dataset model
    struct dataset_model  
    {
      dataset_model() = default; 

      dataset_model(const model_id & id_, const std::string & description_);
  
      const model_id & id() const;

      const std::string & description() const;
  
      const boost::property_tree::ptree & details() const;

      boost::property_tree::ptree & details();

      friend std::ostream & operator<<(std::ostream & out_, const dataset_model & dsm_);
      
      void print_tree(std::ostream & out_) const;

    private:

      model_id _id_; ///< Model ID
      std::string _description_; ///< Description of the model
      boost::property_tree::ptree _details_; ///< List of details describing the dataset model
      
    };
 
  } // namespace datasets
} // namespace snemo 

#endif // SNEMO_DATASETS_DATASET_MODEL_HPP
