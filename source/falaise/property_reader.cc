// property_reader.cc

// Ourselves
#include "falaise/property_reader.h"

namespace falaise {

namespace properties {

namespace type_check_visitor {

namespace detail {

bool visit_impl(const datatools::properties& p, const std::string& key, int) {
  return p.is_integer(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, double) {
  return p.is_real(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, bool) {
  return p.is_boolean(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, std::string) {
  return p.is_string(key) && p.is_scalar(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, std::vector<int>) {
  return p.is_integer(key) && p.is_vector(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, std::vector<double>) {
  return p.is_real(key) && p.is_vector(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, std::vector<bool>) {
  return p.is_boolean(key) && p.is_vector(key);
}

bool visit_impl(const datatools::properties& p, const std::string& key, std::vector<std::string>) {
  return p.is_string(key) && p.is_vector(key);
}

}  // namespace detail
}  // namespace type_check_visitor
}  // namespace properties
}  // namespace falaise
