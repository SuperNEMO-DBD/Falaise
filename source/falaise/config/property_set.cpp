#include "property_set.h"

namespace falaise {
namespace config {
property_set::property_set(datatools::properties const& ps) : ps_(ps) {}

property_set::operator datatools::properties() const { return ps_; }

bool property_set::is_empty() const { return ps_.empty(); }

std::vector<std::string> property_set::get_names() const { return ps_.keys(); }

bool property_set::has_key(std::string const& key) const { return ps_.has_key(key); }

std::string property_set::to_string() const {
  std::ostringstream oss;
  ps_.tree_dump(oss);
  return oss.str();
}

bool property_set::erase(std::string const& key) {
  // Check first to avoid exception from erase() of properties
  if (ps_.has_key(key)) {
    ps_.erase(key);
    return true;
  }
  return false;
}

bool property_set::is_type_impl_(std::string const& key, int) const {
  return ps_.is_integer(key) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, double) const {
  // Request for raw double implies a dimensionless number is wanted
  return ps_.is_real(key) && (!ps_.has_explicit_unit(key)) && (!ps_.has_unit_symbol(key)) &&
         ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, bool) const {
  return ps_.is_boolean(key) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, std::string) const {
  // Request for raw string implies a non-path type string is wanted
  return ps_.is_string(key) && (!ps_.is_explicit_path(key)) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, falaise::config::path) const {
  return ps_.is_explicit_path(key) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, falaise::config::quantity) const {
  // Quantity must be real, and have explicit unit *and* unit symbol
  return ps_.is_real(key) && ps_.has_explicit_unit(key) && ps_.has_unit_symbol(key) &&
         ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, std::vector<int>) const {
  return ps_.is_integer(key) && ps_.is_vector(key);
}

bool property_set::is_type_impl_(std::string const& key, std::vector<double>) const {
  // vector of raw doubles is always dimensionless
  return ps_.is_real(key) && (!ps_.has_explicit_unit(key)) && (!ps_.has_unit_symbol(key)) &&
         ps_.is_vector(key);
}

bool property_set::is_type_impl_(std::string const& key, std::vector<bool>) const {
  return ps_.is_boolean(key) && ps_.is_vector(key);
}

bool property_set::is_type_impl_(std::string const& key, std::vector<std::string>) const {
  return ps_.is_string(key) && ps_.is_vector(key);
}

void make_property_set(const std::string& filename, property_set& ps) {
  datatools::properties tmp{};
  datatools::properties::read_config(filename, tmp);
  ps = tmp;
}
}  // namespace config
}  // namespace falaise