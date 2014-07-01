// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/data_model.cc

// Ourselves:
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

  namespace datamodel {

    // Event record:
    const std::string data_info::EVENT_RECORD_LABEL = "ER";

    const std::string & data_info::default_event_record_label()
    {
      static std::string lbl("ER");
      return lbl;
    }

    // Data banks :
    const std::string data_info::EVENT_HEADER_LABEL = "EH";

    const std::string & data_info::default_event_header_label()
    {
      static std::string lbl("EH");
      return lbl;
    }

    const std::string data_info::SIMULATED_DATA_LABEL = "SD";

    const std::string & data_info::default_simulated_data_label()
    {
      static std::string lbl("SD");
      return lbl;
    }

    const std::string data_info::RAW_DATA_LABEL = "RD";

    const std::string & data_info::default_raw_data_label()
    {
      static std::string lbl("RD");
      return lbl;
    }

    const std::string data_info::UNIFIED_DATA_LABEL    = "UD";

    const std::string & data_info::default_unified_data_label()
    {
      static std::string lbl("UD");
      return lbl;
    }

    const std::string data_info::CALIBRATED_DATA_LABEL = "CD";

    const std::string & data_info::default_calibrated_data_label()
    {
      static std::string lbl("CD");
      return lbl;
    }

    const std::string data_info::TRACKER_CLUSTERING_DATA_LABEL = "TCD";

    const std::string & data_info::default_tracker_clustering_data_label()
    {
      static std::string lbl("TCD");
      return lbl;
    }

    const std::string data_info::TRACKER_TRAJECTORY_DATA_LABEL = "TTD";

    const std::string & data_info::default_tracker_trajectory_data_label()
    {
      static std::string lbl("TTD");
      return lbl;
    }

    const std::string & data_info::default_particle_track_data_label()
    {
      static std::string lbl("PTD");
      return lbl;
    }

  } // end of namespace datamodel

} // end of namespace snemo
