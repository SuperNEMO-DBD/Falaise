// Ourselves:
#include <falaise/datasets/model_id.hpp>

// Third party
#include <bayeux/datatools/exception.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <cctype>

namespace snemo {
  namespace datasets {
    
    model_id::model_id(const std::string & name_, const std::string & version_)
    {
      DT_THROW_IF(name_.empty(), std::logic_error, "Empty name");
      DT_THROW_IF(name_.find_first_of("\t\n -") != std::string::npos,
		  std::logic_error, "Name contains forbidden char(s)");
      DT_THROW_IF(std::isdigit(name_[0]),
		  std::logic_error, "Name starts with a digit");
      _name_ = name_;
      _version_ = version_;
      return;
    }

    const std::string & model_id::name() const
    {
      return _name_;
    }

    bool model_id::has_version() const
    {
      return _version_.size();
    }
  
    const std::string & model_id::version() const
    {
      return _version_;
    }

    // friend
    std::ostream & operator<<(std::ostream & out_, const model_id & id_)
    {
      out_ << id_.name();
      if (id_.has_version()) {
	out_ << "-" << id_.version();
      }
      return out_;
    }

    std::string model_id::to_string() const
    {
      std::ostringstream sid;
      sid << *this;
      return sid.str();
    }
 
    void model_id::from_string(const std::string & token_)
    {
      std::string name;
      std::string version;
      auto pos = token_.find_first_of("-");
      if (pos == std::string::npos) {
	name = token_;
      } else {
	name = token_.substr(0, pos);
	version = token_.substr(pos+1);
      }
      *this = model_id(name, version);
      return;
    }

    bool model_id::operator==(const model_id & other_) const
    {
      if (_name_ != other_._name_) {
	return false;
      }
      if (_version_ != other_._version_) {
	return false;
      }
      return true;
    }

    bool model_id::operator<(const model_id & other_) const
    {
      if (_name_ < other_._name_) {
	return true;
      }
      if (_name_ > other_._name_) {
	return false;
      }
      if (_version_ < other_._version_) {
	return true;
      }
      return false;
    }

  } // namespace datasets 
} // namespace snemo 
