#include "falaise/path.h"

#include "bayeux/datatools/utils.h"

namespace {
// Resolve input string to path for path's constructor-from-string
std::string resolve_path(std::string const& input) {
  std::string result{input};
  // Datatools cannot handle empty paths, but we allow them
  if (result.empty()) {
    return result;
  }
  std::string err{};
  if (!datatools::fetch_path(result, err)) {
    throw falaise::invalid_path_error(err);
  }
  return result;
}
}  // namespace

namespace falaise {
path::path(std::string const& p) : value(resolve_path(p)) {}
}  // namespace falaise
