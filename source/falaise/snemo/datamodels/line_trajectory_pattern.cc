// falaise/snemo/datamodels/line_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(line_trajectory_pattern,
                                                      "snemo::datamodel::line_trajectory_pattern")

    // static
    const std::string & line_trajectory_pattern::pattern_id()
    {
      static const std::string _id("line");
      return _id;
    }

    line_trajectory_pattern::line_trajectory_pattern()
      : base_trajectory_pattern(line_trajectory_pattern::pattern_id())
    {
    }

    geomtools::line_3d &line_trajectory_pattern::get_segment()
    {
      return _segment_;
    }

    const geomtools::line_3d &line_trajectory_pattern::get_segment() const
    {
      return _segment_;
    }

    // override
    const geomtools::i_shape_1d &line_trajectory_pattern::get_shape() const
    {
      return dynamic_cast<const geomtools::i_shape_1d &>(_segment_);
    }

    // override
    geomtools::vector_3d line_trajectory_pattern::get_first() const
    {
      return _segment_.get_first();
    }

    // override
    geomtools::vector_3d line_trajectory_pattern::get_first_direction() const
    {
      return (_segment_.get_last() - _segment_.get_first()).unit();
    }

    // override
    geomtools::vector_3d line_trajectory_pattern::get_last() const
    {
      return _segment_.get_last();
    }
    
    // override
    geomtools::vector_3d line_trajectory_pattern::get_last_direction() const
    {
      return (_segment_.get_last() - _segment_.get_first()).unit();
    }

    // override
    unsigned int line_trajectory_pattern::number_of_kinks() const
    {
      return 0;
    }
  
    // override
    geomtools::vector_3d line_trajectory_pattern::get_kink(unsigned int /* kink_index_ */) const
    {
      DT_THROW(std::logic_error, "No kink!");
    }

    // override
    geomtools::vector_3d line_trajectory_pattern::get_kink_direction(unsigned int /* kink_index_ */,
                                                                     direction_type /* dir_ */) const
    {
      DT_THROW(std::logic_error, "No kink!");
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
