#ifndef SNEMO_DATASETS_MODEL_ID_HPP
#define SNEMO_DATASETS_MODEL_ID_HPP

// Standard library:
#include <string>
// #include <optional>
#include <iostream>

namespace snemo {
  namespace datasets {

    /// Identifier of a model
    struct model_id
    {
      model_id() = default;

      model_id(const std::string & name_, const std::string & version_ = "");

      const std::string & name() const;

      bool has_version() const;
	
      const std::string & version() const;

      friend std::ostream & operator<<(std::ostream & out_, const model_id & id_);

      std::string to_string() const;

      void from_string(const std::string & token_);
  
      bool operator==(const model_id & other_) const;
 
      bool operator<(const model_id & other_) const;
     
    private:

      std::string _name_; ///< Unique name
      std::string _version_; ///< Version number/label

    };
  
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_MODEL_ID_HPP
