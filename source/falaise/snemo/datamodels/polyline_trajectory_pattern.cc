// falaise/snemo/datamodels/polyline_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(polyline_trajectory_pattern,
                                                      "snemo::datamodel::polyline_trajectory_pattern")

    // static
    const std::string & polyline_trajectory_pattern::pattern_id() {
      static const std::string _id("polyline");
      return _id;
    }

    polyline_trajectory_pattern::polyline_trajectory_pattern()
      : base_trajectory_pattern(polyline_trajectory_pattern::pattern_id()) {
    }


    geomtools::polyline_3d & polyline_trajectory_pattern::get_path()
    {
      return _path_;
    }

    const geomtools::polyline_3d & polyline_trajectory_pattern::get_path() const
    {
      return _path_;
    }

    // override
    const geomtools::i_shape_1d & polyline_trajectory_pattern::get_shape() const {
      return dynamic_cast<const geomtools::i_shape_1d &>(_path_);
    }

    // override
    geomtools::vector_3d polyline_trajectory_pattern::get_first() const
    {
      return _path_.get_point(0);
    }

    // override
    geomtools::vector_3d polyline_trajectory_pattern::get_first_direction() const
    {
      return (_path_.get_point(1) - _path_.get_point(0)).unit();
    }

    // override
    geomtools::vector_3d polyline_trajectory_pattern::get_last() const
    {
      int n = _path_.get_number_of_vertexes();
      return (_path_.get_point(n - 1) - _path_.get_point(n - 2)).unit();
    }

    // override
    geomtools::vector_3d polyline_trajectory_pattern::get_last_direction() const
    {
      return (_path_.get_point(1) - _path_.get_point(0)).unit();
    }

    // override
    unsigned int polyline_trajectory_pattern::number_of_kinks() const
    {
      return (unsigned int) (_path_.get_number_of_vertexes() - 2);
    }
  
    // override
    geomtools::vector_3d polyline_trajectory_pattern::get_kink(unsigned int kink_index_) const
    {
      DT_THROW_IF(kink_index_ >= number_of_kinks(),
                  std::range_error,
                  "Invalid kink index!");
      return _path_.get_vertex(1 + kink_index_);
    }

    // override
    geomtools::vector_3d
    polyline_trajectory_pattern::get_kink_direction(unsigned int kink_index_,
                                                    direction_type dir_) const
    {
      DT_THROW_IF(kink_index_ >= number_of_kinks(),
                  std::range_error,
                  "Invalid kink index!");

      /*
       *   last        kink[2]
       *     *----------*
       * path[4]  path[3]\
       *                  \
       *                   \  kink[1]
       *            path[2] *   
       *                    |
       *                    |
       *            path[1] *  
       *                   /  kink[0]
       *                  /
       *                 /
       *                /
       *       path[0] * 
       *                first
       */
      unsigned int n1 = kink_index_;
      if (dir_ == DIRECTION_BACKWARD) {     
      } else if (dir_ == DIRECTION_FORWARD) {
        n1++;
      } else {
        DT_THROW(std::logic_error, "Invalid direction at kink!");       
      }
      unsigned int n2 = n1 + 1;
      return (_path_.get_point(n2) - _path_.get_point(n1)).unit();    
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
