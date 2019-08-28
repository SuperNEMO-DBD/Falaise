// tags.cc

// Ourselves
#include "falaise/tags.h"

namespace falaise {

namespace tags {

const std::string& experiment_category() {
  static const std::string _c("experiment");
  return _c;
}

const std::string& experimental_setup_category() {
  static const std::string _c("expsetup");
  return _c;
}

const std::string& geometry_setup_category() {
  static const std::string _c("geomsetup");
  return _c;
}

const std::string& simulation_setup_category() {
  static const std::string _c("simsetup");
  return _c;
}

const std::string& reconstruction_setup_category() {
  static const std::string _c("recsetup");
  return _c;
}

const std::string& variant_service_category() {
  static const std::string _c("variants");
  return _c;
}

const std::string& services_category() {
  static const std::string _c("services");
  return _c;
}

const std::string& generic_configuration_category() {
  static const std::string _c("configuration");
  return _c;
}

}  // namespace tags

}  // namespace falaise
