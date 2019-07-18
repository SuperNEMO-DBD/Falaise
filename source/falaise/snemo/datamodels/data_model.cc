// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/data_model.cc

// Ourselves:
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

namespace datamodel {

const std::string& data_info::default_event_record_label() {
  static std::string lbl("ER");
  return lbl;
}

const std::string& data_info::default_event_header_label() {
  static std::string lbl("EH");
  return lbl;
}

const std::string& data_info::default_simulated_data_label() {
  static std::string lbl("SD");
  return lbl;
}

const std::string& data_info::default_simulated_signal_data_label() {
  static std::string lbl("SSD");
  return lbl;
}

const std::string& data_info::default_simulated_digitized_data_label() {
  static std::string lbl("SDD");
  return lbl;
}

const std::string& data_info::default_raw_data_label() {
  static std::string lbl("RD");
  return lbl;
}

const std::string& data_info::default_unified_digitized_data_label() {
  static std::string lbl("UDD");
  return lbl;
}

const std::string& data_info::default_calibrated_data_label() {
  static std::string lbl("CD");
  return lbl;
}

const std::string& data_info::default_tracker_clustering_data_label() {
  static std::string lbl("TCD");
  return lbl;
}

const std::string& data_info::default_tracker_trajectory_data_label() {
  static std::string lbl("TTD");
  return lbl;
}

const std::string& data_info::default_particle_track_data_label() {
  static std::string lbl("PTD");
  return lbl;
}

}  // end of namespace datamodel

}  // end of namespace snemo
