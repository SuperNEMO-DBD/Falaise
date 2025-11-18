// Ourselves:
#include <falaise/datasets/dataset_producer_config.hpp>

// Standard library:
#include <stdexcept>
#include <iomanip>

namespace snemo {
  namespace datasets {
    
    dataset_producer_config::dataset_producer_config(const instance_id & id_,
						     dataset_producer_model_ref_type producer_model_)
      : _id_(id_)
      ,	_producer_model_(producer_model_)
    {
    }
 
    const instance_id & dataset_producer_config::id() const
    {
      return _id_;
    }

    const dataset_producer_model & dataset_producer_config::producer_model() const
    {
      return _producer_model_;
    }
  
    const boost::property_tree::ptree & dataset_producer_config::parameters() const
    {
      return _parameters_;
    }

    boost::property_tree::ptree & dataset_producer_config::parameters()
    {
      return _parameters_;
    }
 
    void dataset_producer_config::print_tree(std::ostream & out_) const
    {
      out_ << "|-- " << "ID : " << std::quoted(_id_.to_string()) << " [@" << this << ']' << '\n';
      out_ << "|-- " << "Producer model : " << std::quoted(_producer_model_.get().id().to_string()) << " [@" << &_producer_model_.get() << ']' << '\n';
      out_ << "`-- " << "Parameters : " << _parameters_.size() << '\n';
      // for (auto boost::property_tree::ptree::value_type & v: _parameters_) {
      // 	out_ << "    " << " + " << "Parameter : " << std::quoted(v.first) << '\n';
      // }
      for (auto it = _parameters_.begin();
	   it != _parameters_.end();
	   it++) {
	out_ << "    " << " + " << "Parameter : " << std::quoted(it->first) << '\n';
      }
      return;
    }

  } // namespace datasets
} // namespace snemo 
