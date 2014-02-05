// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/polyline_trajectory_pattern.cc
 */

#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>

namespace snemo {

  namespace datamodel {

    // serial tag for datatools::serialization::i_serializable interface :
    //DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(polyline_trajectory_pattern, "snemo::core::model::polyline_trajectory_pattern")
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(polyline_trajectory_pattern, "snemo::datamodel::polyline_trajectory_pattern")

    // static
    const std::string & polyline_trajectory_pattern::pattern_id()
    {
      static const std::string _id(polyline_trajectory_pattern::pattern_id());
      return _id;
    }

    polyline_trajectory_pattern::polyline_trajectory_pattern()
      : base_trajectory_pattern(polyline_trajectory_pattern::pattern_id())
    {
      return;
    }

    polyline_trajectory_pattern::~polyline_trajectory_pattern()
    {
      return;
    }

    geomtools::polyline_3d & polyline_trajectory_pattern::grab_path()
    {
      return _path_;
    }

    const geomtools::polyline_3d & polyline_trajectory_pattern::get_path() const
    {
      return _path_;
    }

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/polyline_trajectory_pattern.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
