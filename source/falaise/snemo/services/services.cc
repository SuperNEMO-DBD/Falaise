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

const std::string& service_info::dbServiceName() {
  static std::string lbl("db");
  return lbl;
}

const std::string& service_info::trackerCellStatusServiceName() {
  static std::string lbl("trackerCellStatus");
  return lbl;
}

const std::string& service_info::calorimeterOmStatusServiceName() {
  static std::string lbl("calorimeterOmStatus");
  return lbl;
}

}  // end of namespace snemo
