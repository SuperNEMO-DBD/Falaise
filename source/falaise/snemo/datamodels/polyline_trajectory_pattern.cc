// falaise/snemo/datamodels/polyline_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
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

    // virtual
    const geomtools::i_shape_1d & polyline_trajectory_pattern::get_shape() const
    {
      return dynamic_cast<const geomtools::i_shape_1d &>(_path_);
    }

  } // end of namespace datamodel

} // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
