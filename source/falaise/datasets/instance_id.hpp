#ifndef SNEMO_DATASETS_INSTANCE_ID_HPP
#define SNEMO_DATASETS_INSTANCE_ID_HPP

// Standard library:
#include <string>
#include <cstdint>
#include <iostream>

namespace snemo {
  namespace datasets {

    /// Identifier of an instance
    struct instance_id
    {
      instance_id() = default;

      instance_id(const std::string & name_, const std::string & version_ = "");

      instance_id(const std::string & name_,
		  const std::int32_t maj_,
		  const std::int32_t min_ = -1,
		  const std::int32_t rev_ = -1);

      const std::string & name() const;

      bool has_version() const;
	
      const std::string & version() const;

      friend std::ostream & operator<<(std::ostream & out_, const instance_id & id_);
 
      std::string to_string() const;

      void from_string(const std::string & token_);

      bool operator==(const instance_id & other_) const;
 
      bool operator<(const instance_id & other_) const;
      
    private:

      std::string _name_; ///< Name prefix
      std::string _version_; ///< Version number/label

    };
  
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_INSTANCE_ID_HPP
