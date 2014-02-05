// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/data_model.cc */

// Ourselves:
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

    namespace datamodel {

      // Event record:
      const std::string data_info::EVENT_RECORD_LABEL    = "ER";

      // Data banks :
      const std::string data_info::EVENT_HEADER_LABEL    = "EH";
      const std::string data_info::SIMULATED_DATA_LABEL  = "SD";
      const std::string data_info::RAW_DATA_LABEL        = "RD";
      const std::string data_info::UNIFIED_DATA_LABEL    = "UD";
      const std::string data_info::CALIBRATED_DATA_LABEL = "CD";
      const std::string data_info::TRACKER_CLUSTERING_DATA_LABEL = "TCD";
      const std::string data_info::TRACKER_TRAJECTORY_DATA_LABEL = "TTD";

    } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/data_model.cc
