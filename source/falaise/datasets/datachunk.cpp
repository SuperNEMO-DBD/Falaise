// Ourselves:
#include <falaise/datasets/datachunk.hpp>

// Standard library:
#include <stdexcept>

// This project:

namespace snemo {
  namespace datasets {
   
    datachunk::datachunk(const instance_id & id_,
			 const std::string & path_,
			 opt_nentries_type number_of_entries_opt_)
      : _id_(id_)
      , _path_(path_)
      , _number_of_entries_opt_(number_of_entries_opt_)
    {
      return;
    }
 
    const instance_id & datachunk::id() const
    {
      return _id_;
    }

    const std::string & datachunk::path() const
    {
      return _path_;
    }
 
    bool datachunk::has_number_of_entries() const
    {
      return _number_of_entries_opt_.has_value();
    }
  
    std::uint32_t datachunk::number_of_entries() const
    {
      return _number_of_entries_opt_.value();
    }
  
  } // namespace datasets
} // namespace snemo 
