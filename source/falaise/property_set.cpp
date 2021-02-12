#include "property_set.h"

#include <fstream>

namespace falaise {
property_set::property_set(datatools::properties const& ps) : ps_(ps) {}

property_set::operator datatools::properties() const { return ps_; }

bool property_set::is_empty() const { return ps_.empty(); }

std::vector<std::string> property_set::get_names() const { return ps_.keys(); }

bool property_set::has_key(std::string const& key) const { return ps_.has_key(key); }

bool property_set::is_key_to_property(std::string const& key) const {
  return has_key(key) && !is_key_to_sequence(key);
}

bool property_set::is_key_to_sequence(std::string const& key) const {
  if (has_key(key)) {
    return ps_.is_vector(key);
  }
  return false;
}

bool property_set::is_key_to_property_set(std::string const& key) const {
  // Has to be a key and not a sequence/property
  if (is_key_to_property(key) || is_key_to_sequence(key)) {
    return false;
  }

  // Otherwise, must have > 0 keys starting with "prefix."
  auto keyCount = ps_.keys_starting_with(key + ".");
  return !keyCount.empty();
}

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

bool property_set::is_type_impl_(std::string const& key, int /*unused*/) const {
  return ps_.is_integer(key) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, double /*unused*/) const {
  // Request for raw double implies a dimensionless number is wanted
  return ps_.is_real(key) && (!ps_.has_explicit_unit(key)) && (!ps_.has_unit_symbol(key)) &&
         ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, bool /*unused*/) const {
  return ps_.is_boolean(key) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, const std::string& /*unused*/) const {
  // Request for raw string implies a non-path type string is wanted
  return ps_.is_string(key) && (!ps_.is_explicit_path(key)) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, const falaise::path& /*unused*/) const {
  return ps_.is_explicit_path(key) && ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key,
                                 const falaise::quantity& /*unused*/) const {
  // Quantity must be real, and have explicit unit *and* unit symbol
  return ps_.is_real(key) && ps_.has_explicit_unit(key) && ps_.has_unit_symbol(key) &&
         ps_.is_scalar(key);
}

bool property_set::is_type_impl_(std::string const& key, const std::vector<int>& /*unused*/) const {
  return ps_.is_integer(key) && ps_.is_vector(key);
}

bool property_set::is_type_impl_(std::string const& key,
                                 const std::vector<double>& /*unused*/) const {
  // vector of raw doubles is always dimensionless
  return ps_.is_real(key) && (!ps_.has_explicit_unit(key)) && (!ps_.has_unit_symbol(key)) &&
         ps_.is_vector(key);
}

bool property_set::is_type_impl_(std::string const& key,
                                 const std::vector<bool>& /*unused*/) const {
  return ps_.is_boolean(key) && ps_.is_vector(key);
}

bool property_set::is_type_impl_(std::string const& key,
                                 const std::vector<std::string>& /*unused*/) const {
  return ps_.is_string(key) && ps_.is_vector(key);
}

void make_property_set(const std::string& filename, property_set& ps) {
  std::ifstream fs{filename.c_str()};
  make_property_set(fs, ps);
}

void make_property_set(std::istream& is, property_set& ps) {
  datatools::properties tmp{};
  datatools::properties::config reader{};
  reader.read(is, tmp);
  ps = tmp;
}
}  // namespace falaise
