/// \file falaise/snemo/reconstruction/charge_computation_driver.cc

// Ourselves:
#include <falaise/snemo/reconstruction/charge_computation_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace reconstruction {

    void charge_computation_driver::set_initialized(const bool initialized_)
    {
      _initialized_ = initialized_;
      return;
    }

    bool charge_computation_driver::is_initialized() const
    {
      return _initialized_;
    }

    void charge_computation_driver::set_logging_priority(const datatools::logger::priority priority_)
    {
      _logging_priority_ = priority_;
      return;
    }

    datatools::logger::priority charge_computation_driver::get_logging_priority() const
    {
      return _logging_priority_;
    }

    /// Constructor
    charge_computation_driver::charge_computation_driver()
    {
      _set_defaults();
      return;
    }

    /// Destructor
    charge_computation_driver::~charge_computation_driver()
    {
      if (is_initialized())
        {
          reset();
        }
      return;
    }

    /// Initialize the driver through configuration properties
    void charge_computation_driver::initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");

      // Logging priority
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration (setup_);
      DT_THROW_IF (lp == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority level for geometry manager !");
      set_logging_priority(lp);

      if (setup_.has_key ("charge_from_source"))
        {
          _charge_from_source_ = setup_.fetch_boolean ("charge_from_source");
        }

      set_initialized (true);
      return;
    }

    /// Reset the driver
    void charge_computation_driver::reset ()
    {
      _set_defaults ();
      return;
    }

    void charge_computation_driver::_set_defaults ()
    {
      _initialized_        = false;
      _logging_priority_   = datatools::logger::PRIO_WARNING;
      _charge_from_source_ = true;
      return;
    }

    void charge_computation_driver::process(const snemo::datamodel::tracker_trajectory & trajectory_,
                                            snemo::datamodel::particle_track           & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");

      this->_measure_particle_charge_(trajectory_, particle_);

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

    void charge_computation_driver::_measure_particle_charge_(const snemo::datamodel::tracker_trajectory & trajectory_,
                                                              snemo::datamodel::particle_track           & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Look first if trajectory pattern is an helix or not
      const snemo::datamodel::base_trajectory_pattern & a_track_pattern = trajectory_.get_pattern();

      if (a_track_pattern.get_pattern_id() == snemo::datamodel::line_trajectory_pattern::pattern_id())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Trajectory pattern is 'line', no curvature can be extracted");
          particle_.set_charge(snemo::datamodel::particle_track::undefined);
          return;
        }

      // Retrieve helix trajectory
      const snemo::datamodel::helix_trajectory_pattern * ptr_helix = 0;
      if (a_track_pattern.get_pattern_id() == snemo::datamodel::helix_trajectory_pattern::pattern_id())
        {
          ptr_helix = dynamic_cast<const snemo::datamodel::helix_trajectory_pattern *>(&a_track_pattern);
        }

      if (!ptr_helix)
        {
          DT_LOG_ERROR(get_logging_priority(), "Tracker trajectory is not an 'helix' !");
          return;
        }

      // Retrieve starting and ending point of helix trajectory
      const geomtools::vector_3d first_point = ptr_helix->get_helix().get_first();
      const geomtools::vector_3d last_point  = ptr_helix->get_helix().get_last();
      const bool is_negative = std::fabs(first_point.x()) < std::fabs(last_point.x());

      if (is_negative) particle_.set_charge(snemo::datamodel::particle_track::negative);
      else             particle_.set_charge(snemo::datamodel::particle_track::positive);

      DT_LOG_TRACE(get_logging_priority(), "Particle charge is " << (is_negative ? "negative" : "positive"));

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }
  }  // end of namespace reconstruction

}  // end of namespace snemo

// end of falaise/snemo/reconstruction/charge_computation_driver.cc
