#include "falaise/config/path.h"

#include "bayeux/datatools/utils.h"

namespace {
// Resolve input string to path for path's constructor-from-string
std::string resolve_path(std::string const& input) {
  std::string result{input};
  std::string err{};
  if (!datatools::fetch_path(result, err)) {
    throw falaise::config::invalid_path_error(err);
  }
  return result;
}
}  // namespace

namespace falaise {
namespace config {
path::path(std::string const& p) : value(resolve_path(p)) {}
}  // namespace config
}  // namespace falaise
