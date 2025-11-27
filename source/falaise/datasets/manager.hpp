#ifndef SNEMO_DATASETS_MANAGER_HPP
#define SNEMO_DATASETS_MANAGER_HPP


// Third party:
//#include <boost/property_tree/ptree.hpp>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/datasets/factory.hpp>

namespace snemo {
  namespace datasets {
 
    class manager
    {
    public:
      
      manager() = default;

      void initialize(const datatools::properties & config_);

      void reset();

      const factory & fact() const;

      factory & fact();

    public:
      
      datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;

    private:

      void _init_dataset_models_();

      void _init_dataset_producer_models_();
      
    private:

      factory _fact_;
      
    };
 
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_MANAGER_HPP
