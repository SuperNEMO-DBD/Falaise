// falaise/snemo/datamodels/helix_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(helix_trajectory_pattern,
                                                      "snemo::datamodel::helix_trajectory_pattern")

    // static
    const std::string &helix_trajectory_pattern::pattern_id() {
      static const std::string _id("helix");
      return _id;
    }

    helix_trajectory_pattern::helix_trajectory_pattern()
      : base_trajectory_pattern(helix_trajectory_pattern::pattern_id()) {
    }

    geomtools::helix_3d &helix_trajectory_pattern::get_helix() { return _helix_; }

    const geomtools::helix_3d &helix_trajectory_pattern::get_helix() const { return _helix_; }

    // override
    const geomtools::i_shape_1d &helix_trajectory_pattern::get_shape() const {
      return dynamic_cast<const geomtools::i_shape_1d &>(_helix_);
    }

    // override
    geomtools::vector_3d helix_trajectory_pattern::get_first() const
    {
      return _helix_.get_first();
    }

    // override
    geomtools::vector_3d helix_trajectory_pattern::get_first_direction() const
    {
      double t = _helix_.get_t1();
      double dt = 0.01;
      return (_helix_.get_point(t+dt) - _helix_.get_point(t-dt)).unit();
    }

    // override
    geomtools::vector_3d helix_trajectory_pattern::get_last() const
    {
      return _helix_.get_last();
    }

    // override
    geomtools::vector_3d helix_trajectory_pattern::get_last_direction() const
    {
      double t = _helix_.get_t2();
      double dt = 0.01;
      return (_helix_.get_point(t+dt) - _helix_.get_point(t-dt)).unit();
    }

    // override
    unsigned int helix_trajectory_pattern::number_of_kinks() const
    {
      return 0;
    }
  
    // override
    geomtools::vector_3d helix_trajectory_pattern::get_kink(unsigned int /* kink_index_ */) const
    {
      DT_THROW(std::logic_error, "No kink!");
    }
    
    // override
    geomtools::vector_3d helix_trajectory_pattern::get_kink_direction(unsigned int /* kink_index_ */,
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
