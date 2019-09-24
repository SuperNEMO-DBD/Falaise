// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/data_model.cc

// Ourselves:
#include <falaise/snemo/datamodels/data_model.h>

namespace snedm {

const std::string& labels::event_record() {
  static std::string lbl("ER");
  return lbl;
}

const std::string& labels::event_header() {
  static std::string lbl("EH");
  return lbl;
}

const std::string& labels::simulated_data() {
  static std::string lbl("SD");
  return lbl;
}

const std::string& labels::calibrated_data() {
  static std::string lbl("CD");
  return lbl;
}

const std::string& labels::tracker_clustering_data() {
  static std::string lbl("TCD");
  return lbl;
}

const std::string& labels::tracker_trajectory_data() {
  static std::string lbl("TTD");
  return lbl;
}

const std::string& labels::particle_track_data() {
  static std::string lbl("PTD");
  return lbl;
}

}  // namespace snedm
