//! \file falaise/snemo/processing/types.h
//! \brief Common types for pipeline processing
#ifndef FALAISE_SNEMO_PROCESSING_TYPES_H
#define FALAISE_SNEMO_PROCESSING_TYPES_H

#include <stdexcept>

#include <bayeux/dpp/base_module.h>

namespace falaise {
namespace processing {
//! Enumeration for module to indicate processing success/failure/other
using status = dpp::base_module::process_status;

//! Exception thrown if configuration overwrites reserved keys
class reserved_key_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Exception thrown if module fails configuration
class configuration_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Exception thrown if a module cannot be found
class missing_module_error : public std::logic_error {
  using std::logic_error::logic_error;
};
}  // namespace processing
}  // namespace falaise

#endif  // FALAISE_SNEMO_PROCESSING_TYPES_H