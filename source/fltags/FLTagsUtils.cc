// Ourselves
#include "FLTagsUtils.h"

// Third party:
// - Bayeux:
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/urn_query_service.h"

// This Project:
#include "falaise/detail/falaise_sys.h"

namespace FLTags {

  std::string default_dot_filename()
  {
    return "fltags.dot";
  }

  std::string default_list_filename()
  {
    return "fltags.lis";
  }

}  // end of namespace FLTags
