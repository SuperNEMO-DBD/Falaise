// property_reader.cc

// Ourselves
#include "falaise/config/property_reader.h"

namespace falaise {

namespace config {

namespace type_check_visitor {

namespace detail {

bool visit_impl(const datatools::properties& p, const std::string& key, int /*unused*/) {
  return p.is_integer(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, double /*unused*/) {
  return p.is_real(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, bool /*unused*/) {
  return p.is_boolean(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key,
                const std::string& /*unused*/) {
  return p.is_string(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key,
                const std::vector<int>& /*unused*/) {
  return p.is_integer(key) && p.is_vector(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key,
                const std::vector<double>& /*unused*/) {
  return p.is_real(key) && p.is_vector(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key,
                const std::vector<bool>& /*unused*/) {
  return p.is_boolean(key) && p.is_vector(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key,
                const std::vector<std::string>& /*unused*/) {
  return p.is_string(key) && p.is_vector(key);
}

}  // namespace detail
}  // namespace type_check_visitor
}  // namespace config
}  // namespace falaise
