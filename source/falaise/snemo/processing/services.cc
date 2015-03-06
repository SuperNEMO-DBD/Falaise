// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/services.cc */

// Ourselves:
#include <falaise/snemo/processing/services.h>

namespace snemo {

  namespace processing {

    const std::string & service_info::default_metadata_service_label()
    {
      static std::string lbl("metadata");
      return lbl;
    }

    const std::string & service_info::default_geometry_service_label()
    {
      static std::string lbl("geometry");
      return lbl;
    }

    const std::string & service_info::default_cut_service_label()
    {
      static std::string lbl("cuts");
      return lbl;
    }

    const std::string & service_info::default_database_service_label()
    {
      static std::string lbl("database");
      return lbl;
    }

  } // end of namespace processing

} // end of namespace snemo
