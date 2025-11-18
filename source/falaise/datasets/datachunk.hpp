#ifndef SNEMO_DATASETS_DATACHUNK_HPP
#define SNEMO_DATASETS_DATACHUNK_HPP

// Standard library:
#include <string>
#include <cstdint>
#include <optional>

// This project:
#include <falaise/datasets/instance_id.hpp>

namespace snemo {
  namespace datasets {
 
    struct datachunk
    {
      typedef std::optional<std::uint32_t> opt_nentries_type;
 
      datachunk() = default;
  
      datachunk(const instance_id & id_,
		const std::string & path_,
		opt_nentries_type number_of_entries_opt_ = std::nullopt);

      const instance_id & id() const;

      const std::string & path() const;

      bool has_number_of_entries() const;

      std::uint32_t number_of_entries() const;
  
    private:

      instance_id _id_;
      std::string _path_;
      opt_nentries_type _number_of_entries_opt_;
  
    };
  
  } // namespace datasets 
} // namespace snemo 

#endif // SNEMO_DATASETS_DATACHUNK_HPP
