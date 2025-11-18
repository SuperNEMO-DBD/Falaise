// Ourselves:
#include <falaise/datasets/dataset_model.hpp>

// Standard library:
#include <stdexcept>
#include <iomanip>

namespace snemo {
  namespace datasets {
    
    dataset_model::dataset_model(const model_id & id_,
				 const std::string & description_)
      : _id_(id_)
      , _description_(description_)
    {
    }
  
    const model_id & dataset_model::id() const
    {
      return _id_;
    }

    const std::string & dataset_model::description() const
    {
      return _description_;
    }
  
    const boost::property_tree::ptree & dataset_model::details() const
    {
      return _details_;
    }

    boost::property_tree::ptree & dataset_model::details()
    {
      return _details_;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const dataset_model & dsm_)
    {
      out_ << '{' << "id=" << std::quoted(dsm_._id_.to_string()) << ";desc=" << std::quoted(dsm_._description_) << ";#details=" << dsm_._details_.size() << '}';
      return out_;
    }
    
    void dataset_model::print_tree(std::ostream & out_) const
    {
      out_ << "|-- " << "ID : " << std::quoted(_id_.to_string()) << " [@" << this << ']' << '\n';
      out_ << "|-- " << "Description : " << std::quoted(_description_) << '\n';
      out_ << "`-- " << "Details : " << _details_.size() << '\n';
      for (auto it = _details_.begin();
	   it != _details_.end();
	   it++) {
	out_ << "   " << " + " << "Key : " << std::quoted(it->first) << '\n';	
      }
      return;
    }

  } // namespace datasets
} // namespace snemo 
