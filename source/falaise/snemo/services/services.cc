// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/services.cc */

// Ourselves:
#include <falaise/snemo/services/services.h>

namespace snemo {

const std::string& service_info::metadataServiceName() {
  static std::string lbl("metadata");
  return lbl;
}

const std::string& service_info::geometryServiceName() {
  static std::string lbl("geometry");
  return lbl;
}

const std::string& service_info::cutServiceName() {
  static std::string lbl("cuts");
  return lbl;
}

const std::string& service_info::rcServiceName() {
  static std::string lbl("RC");
  return lbl;
}

}  // end of namespace snemo
