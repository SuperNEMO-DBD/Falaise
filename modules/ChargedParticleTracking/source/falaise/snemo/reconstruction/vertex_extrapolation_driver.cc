/// \file falaise/snemo/reconstruction/vertex_extrapolation_driver.cc

// Ourselves:
#include <falaise/snemo/reconstruction/vertex_extrapolation_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>

namespace snemo {

  namespace reconstruction {

    void vertex_extrapolation_driver::set_initialized(const bool initialized_)
    {
      _initialized_ = initialized_;
      return;
    }

    bool vertex_extrapolation_driver::is_initialized() const
    {
      return _initialized_;
    }

    void vertex_extrapolation_driver::set_logging_priority(const datatools::logger::priority priority_)
    {
      _logging_priority_ = priority_;
      return;
    }

    datatools::logger::priority vertex_extrapolation_driver::get_logging_priority() const
    {
      return _logging_priority_;
    }

    void vertex_extrapolation_driver::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & vertex_extrapolation_driver::get_geometry_manager() const
    {
      DT_THROW_IF(! has_geometry_manager(), std::logic_error,
                  "No geometry manager is setup !");
      return *_geometry_manager_;
    }

    bool vertex_extrapolation_driver::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    /// Constructor
    vertex_extrapolation_driver::vertex_extrapolation_driver()
    {
      _set_defaults();
      return;
    }

    /// Destructor
    vertex_extrapolation_driver::~vertex_extrapolation_driver()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    /// Initialize the driver through configuration properties
    void vertex_extrapolation_driver::initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");

      DT_THROW_IF(! has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(! get_geometry_manager().is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      // Logging priority
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration (setup_);
      DT_THROW_IF (lp == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority level for geometry manager !");
      set_logging_priority(lp);

      // Get geometry locator plugin
      const geomtools::manager & geo_mgr = get_geometry_manager();
      std::string locator_plugin_name;
      if (setup_.has_key("locator_plugin_name")) {
        locator_plugin_name = setup_.fetch_string("locator_plugin_name");
      } else {
        // If no locator plugin name is set, then search for the first one
        const geomtools::manager::plugins_dict_type & plugins = geo_mgr.get_plugins();
        for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
             ip != plugins.end();
             ip++) {
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
            DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
            locator_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      DT_THROW_IF(! geo_mgr.has_plugin(locator_plugin_name) ||
                  ! geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name),
                  std::logic_error,
                  "Found no locator plugin named '" << locator_plugin_name << "'");
      _locator_plugin_ = &geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);

      set_initialized(true);
      return;
    }

    /// Reset the driver
    void vertex_extrapolation_driver::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");
      _set_defaults();
      return;
    }

    void vertex_extrapolation_driver::_set_defaults()
    {
      _initialized_      = false;
      _logging_priority_ = datatools::logger::PRIO_WARNING;
      _geometry_manager_ = 0;
      return;
    }

    void vertex_extrapolation_driver::process(const snemo::datamodel::tracker_trajectory & trajectory_,
                                              snemo::datamodel::particle_track           & particle_)
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");
      this->_measure_vertices_(trajectory_, particle_.grab_vertices());
      return;
    }

    void vertex_extrapolation_driver::_measure_vertices_(const snemo::datamodel::tracker_trajectory               & trajectory_,
                                                         snemo::datamodel::particle_track::vertex_collection_type & vertices_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Extract the side from the geom_id of the tracker_trajectory object:
      if (! trajectory_.has_geom_id()) {
        DT_LOG_ERROR(get_logging_priority(), "Tracker trajectory has no geom_id! Abort!");
        return;
      }
      const geomtools::geom_id & gid = trajectory_.get_geom_id();
      const geomtools::id_mgr & id_mgr = get_geometry_manager().get_id_mgr();
      if (!id_mgr.has(gid, "module") || !id_mgr.has(gid, "side")) {
        DT_LOG_ERROR(get_logging_priority(), "Trajectory geom_id " << gid << " has no 'module' or 'side' address!");
        return;
      }
      DT_LOG_TRACE(get_logging_priority(), "Trajectory geom_id = " << gid);
      const int side = id_mgr.get(gid, "side");

      // Set the calorimeter locators :
      const snemo::geometry::calo_locator  & calo_locator  = _locator_plugin_->get_calo_locator();
      const snemo::geometry::xcalo_locator & xcalo_locator = _locator_plugin_->get_xcalo_locator();
      const snemo::geometry::gveto_locator & gveto_locator = _locator_plugin_->get_gveto_locator();
      // TODO: Add source strip locator...

      const double xcalo_bd[2]
        = {calo_locator.get_wall_window_x(snemo::geometry::utils::SIDE_BACK),
           calo_locator.get_wall_window_x(snemo::geometry::utils::SIDE_FRONT)};
      const double ycalo_bd[2]
        = {xcalo_locator.get_wall_window_y(side, snemo::geometry::xcalo_locator::WALL_LEFT),
           xcalo_locator.get_wall_window_y(side, snemo::geometry::xcalo_locator::WALL_RIGHT)};
      const double zcalo_bd[2]
        = {gveto_locator.get_wall_window_z(side, snemo::geometry::gveto_locator::WALL_BOTTOM),
           gveto_locator.get_wall_window_z(side, snemo::geometry::gveto_locator::WALL_TOP)};

      // Look first if trajectory pattern is an helix or not:
      const snemo::datamodel::base_trajectory_pattern & a_track_pattern = trajectory_.get_pattern();
      const std::string & a_pattern_id = a_track_pattern.get_pattern_id();

      // Extrapolated vertices:
      std::map<std::string, geomtools::vector_3d> vertices;

      // Add a property into 'blur_spot' auxiliaries to refer to the hit calo:
      std::string calo_category_flag;
      if (a_pattern_id == snemo::datamodel::line_trajectory_pattern::pattern_id()) {
        const snemo::datamodel::line_trajectory_pattern * ptr_line
          = dynamic_cast<const snemo::datamodel::line_trajectory_pattern *>(&a_track_pattern);
        const geomtools::line_3d & a_line = ptr_line->get_segment();
        const geomtools::vector_3d & first = a_line.get_first();
        const geomtools::vector_3d & last  = a_line.get_last();
        const geomtools::vector_3d direction = first - last;

        typedef std::map<geomtools::vector_3d, std::string> vertex_dict_type;
        vertex_dict_type vtxlist;
        // Source foil:
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on source foil...");
          const double x = 0.0 * CLHEP::mm;
          const double y = direction.y()/direction.x() *(x - first.x()) + first.y();
          const double z = direction.z()/direction.y() *(y - first.y()) + first.z();

          // Extrapolated vertex:
          const geomtools::vector_3d a_vertex(x, y, z);
          vtxlist.insert(std::make_pair(a_vertex, snemo::datamodel::particle_track::vertex_on_source_foil_label()));
        }// end of source foil search

        // Calorimeter walls:
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on main wall...");
          for (size_t iside = 0; iside < snemo::geometry::utils::NSIDES; ++iside) {
            const double x = xcalo_bd[iside];
            const double y = direction.y()/direction.x() *(x - first.x()) + first.y();
            const double z = direction.z()/direction.y() *(y - first.y()) + first.z();

            // Extrapolated vertex
            const geomtools::vector_3d a_vertex(x, y, z);
            vtxlist.insert(std::make_pair(a_vertex, snemo::datamodel::particle_track::vertex_on_main_calorimeter_label()));
          }
        }// end of main wall search

        // Calorimeter on xwalls
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on X-wall...");
          for (size_t iwall = 0; iwall < snemo::geometry::xcalo_locator::NWALLS_PER_SIDE; ++iwall) {
            const double y = ycalo_bd[iwall];
            const double z = direction.z()/direction.y() *(y - first.y()) + first.z();
            const double x = direction.x()/direction.y() *(y - first.y()) + first.x();

            // Extrapolate vertex
            const geomtools::vector_3d a_vertex(x, y, z);
            vtxlist.insert(std::make_pair(a_vertex, snemo::datamodel::particle_track::vertex_on_x_calorimeter_label()));
          }
        }// end of x-wall search

        // Calorimeter on gveto
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on gamma veto...");
          for (size_t iwall = 0; iwall < snemo::geometry::gveto_locator::NWALLS_PER_SIDE; ++iwall) {
            const double z = zcalo_bd[iwall];
            const double y = direction.y()/direction.z() *(z - first.z()) + first.y();
            const double x = direction.x()/direction.y() *(y - first.y()) + first.x();

            // Extrapolate vertex
            const geomtools::vector_3d a_vertex(x, y, z);
            vtxlist.insert(std::make_pair(a_vertex,  snemo::datamodel::particle_track::vertex_on_gamma_veto_label()));
          }
        }// end of gveto search

        std::pair<double, double> min_distances;
        datatools::infinity(min_distances.first);
        datatools::infinity(min_distances.second);
        vertex_dict_type::const_iterator jt1 = vtxlist.begin();
        vertex_dict_type::const_iterator jt2 = vtxlist.begin();
        for (vertex_dict_type::const_iterator it = vtxlist.begin();
             it != vtxlist.end(); ++it) {
          const double l1 = (first - it->first).mag();
          const double l2 = (last - it->first).mag();

          if (l1 < l2) {
            if (l1 > min_distances.first) continue;
            jt1 = it;
            min_distances.first = l1;
          } else {
            if (l2 > min_distances.second) continue;
            jt2 = it;
            min_distances.second = l2;
          }
        }

        // Create a mutable line object to set the new position
        geomtools::line_3d * a_mutable_line = const_cast<geomtools::line_3d *>(&a_line);
        a_mutable_line->set_first(jt1->first);
        a_mutable_line->set_last(jt2->first);
        vertices[jt1->second] = jt1->first;
        vertices[jt2->second] = jt2->first;
      }// end of line pattern
      else if (a_pattern_id == snemo::datamodel::helix_trajectory_pattern::pattern_id()) {
        const snemo::datamodel::helix_trajectory_pattern * ptr_helix
          = dynamic_cast<const snemo::datamodel::helix_trajectory_pattern *>(&a_track_pattern);
        const geomtools::helix_3d & a_helix = ptr_helix->get_helix();

        // Extract helix parameters
        const geomtools::vector_3d & hcenter = a_helix.get_center();
        const double hradius                 = a_helix.get_radius();

        // Store all the computed t parameter values
        std::map<double, std::string> tparams;

        // Source foil
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on source foil...");
          const double xfoil   = 0.0 * CLHEP::mm;
          const double xcenter = hcenter.x();
          const double cangle  = (xfoil - xcenter) / hradius;

          if (std::fabs(cangle) < 1.0) {
            const double angle = std::acos(cangle);
            tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(+angle), snemo::datamodel::particle_track::vertex_on_source_foil_label()));
            tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(-angle), snemo::datamodel::particle_track::vertex_on_source_foil_label()));
          }
        } // end of source foil search

          // Calorimeter walls
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on main wall...");
          for (size_t iside = 0; iside < snemo::geometry::utils::NSIDES; ++iside) {
            const double xcenter = hcenter.x();
            const double cangle  = (xcalo_bd[iside] - xcenter) / hradius;

            if (std::fabs(cangle) < 1.0) {
              const double angle = std::acos(cangle);
              tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(+angle), snemo::datamodel::particle_track::vertex_on_main_calorimeter_label()));
              tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(-angle), snemo::datamodel::particle_track::vertex_on_main_calorimeter_label()));
            }
          }
        }// end of main wall search

        // X-walls
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on X-wall...");
          for (size_t iwall = 0; iwall < snemo::geometry::xcalo_locator::NWALLS_PER_SIDE; ++iwall) {
            const double ycenter = hcenter.y();
            const double cangle = (ycalo_bd[iwall] - ycenter) / hradius;

            if (std::fabs(cangle) < 1.0) {
              double angle = std::asin(cangle);
              tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(angle), snemo::datamodel::particle_track::vertex_on_x_calorimeter_label()));
              const double mean_angle = (a_helix.get_angle1() + a_helix.get_angle2())/2.0;
              if (mean_angle < 0.0) angle = - M_PI - angle;
              else                  angle = + M_PI - angle;
              tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(angle), snemo::datamodel::particle_track::vertex_on_x_calorimeter_label()));
            }
          }// end of x-wall loop
        }// end of x-wall search

        // Gvetos
        {
          DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on gamma veto...");
          for (size_t iwall = 0; iwall < snemo::geometry::xcalo_locator::NWALLS_PER_SIDE; ++iwall) {
            const double t = a_helix.get_t_from_z(zcalo_bd[iwall]);
            tparams.insert(std::make_pair(t, snemo::datamodel::particle_track::vertex_on_gamma_veto_label()));
          }
        }// end of gveto search

        if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
          DT_LOG_TRACE(get_logging_priority(), "Stored t parameters :");
          for (std::map<double, std::string>::const_iterator
                 it = tparams.begin(); it != tparams.end(); ++it) {
            DT_LOG_TRACE(get_logging_priority(),  it->second << ", t = " << it->first);
          }
        }

        // Choose which helix angle to change
        const double t1 = a_helix.get_t1();
        const double t2 = a_helix.get_t2();

        std::pair<double,double> new_ts;
        datatools::invalidate(new_ts.first);
        datatools::invalidate(new_ts.second);
        std::pair<double,double> min_distances;
        datatools::infinity(min_distances.first);
        datatools::infinity(min_distances.second);
        std::pair<std::string, std::string> category_flags;

        for (std::map<double, std::string>::const_iterator it = tparams.begin();
             it != tparams.end(); ++it) {
          const double t = it->first;
          const std::string & category = it->second;

          // Calculate delta t values
          const double delta1 = std::fabs(t1 - t);
          const double delta2 = std::fabs(t2 - t);

          // Keep smallest distance but remove also too long extrapolation
          if (delta1 < delta2) {
            if (delta1 > min_distances.first || delta1 > 1.0) continue;
            new_ts.first = t;
            category_flags.first = category;
            min_distances.first = delta1;
          } else {
            if (delta2 > min_distances.second || delta2 > 1.0) continue;
            new_ts.second = t;
            category_flags.second = category;
            min_distances.second = delta2;
          }
        }

        // New angle & calorimeter category
        // Create a mutable helix object to set the new angle
        geomtools::helix_3d * a_mutable_helix = const_cast<geomtools::helix_3d *>(&a_helix);
        if (datatools::is_valid(new_ts.first)) {
          a_mutable_helix->set_t1(new_ts.first);
          vertices[category_flags.first] = a_mutable_helix->get_first();
        }
        if (datatools::is_valid(new_ts.second)) {
          a_mutable_helix->set_t2(new_ts.second);
          vertices[category_flags.second] = a_mutable_helix->get_last();
        }
      }// end of helix pattern

      // Save new vertex
      int spot_id = 0;
      for (std::map<std::string, geomtools::vector_3d>::const_iterator
             it = vertices.begin(); it != vertices.end(); ++it, ++spot_id) {
        // Check vertex side is on the same side as the trajectory
        if ((side == snemo::geometry::utils::SIDE_BACK  && it->second.x() > 0.0) ||
            (side == snemo::geometry::utils::SIDE_FRONT && it->second.x() < 0.0)) {
          DT_LOG_DEBUG(get_logging_priority(), "Closest vertex is on the opposite side!");
        }
        snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot);
        vertices_.push_back(hBS);
        geomtools::blur_spot & spot = hBS.grab();
        spot.set_hit_id(spot_id);
        spot.grab_auxiliaries().update(snemo::datamodel::particle_track::vertex_type_key(),
                                       it->first);
        // Future: determine the GID of the scintillator block or source strip
        // associated to the impact vertex:
        //
        //   int module = 0;
        //   int side = 0;
        //   int strip = -1;
        //   spot.grab_geom_id().set_type(???);
        //   spot.grab_geom_id().set_addresses(module, side/strip?, wall?, column?, row?...);
        // or:
        //   spot.set_geom_id(matchind_gid?);
        //

        // For now it is dimension 3 with no errors nor rotation defined:
        spot.set_blur_dimension(geomtools::blur_spot::dimension_three);
        spot.set_position(it->second);
        //
        // Future implementation:= ???
        //
        //   double sigma_x = ???; // Computed from the TrackFit error matrix
        //   double sigma_y = ???; // Computed from the TrackFit error matrix
        //   double sigma_z = ???; // Computed from the TrackFit error matrix
        //   spot.grab_placement().set_translation(it->second);
        //   if (snemo::datamodel::particle_track::vertex_is_on_main_calorimeter(spot)) {
        //     //
        //     // Possibility:
        //     //    +---------------------------------+
        //     //   /                                 /|
        //     //  /                                 / |
        //     // +---------------------------------+  |
        //     // |           ^ z        Calo block |  |
        //     // |  sigma(z) :            entrance |  |
        //     // |          ---            surface |  |
        //     // |         / : \                   |  |
        //     // |        /  :  \ sigma(y)         |  |
        //     // |    - - |- + -|- - -> y          |  |
        //     // |        | /:  |                  |  |
        //     // |  --->   / : /  2D blur spot     |  +
        //     // | normal / ---                    | /
        //     // |       L   :                     |/
        //     // +---------------------------------+
        //     //
        //     //
        //     spot.set_blur_dimension(geomtools::blur_spot::dimension_two);
        //     if (side == snemo::geometry::utils::SIDE_BACK) {
        //       spot.grab_placement().set_orientation(ROTATION_AXIS_Y, 90.0 * CLHEP::degree);
        //     } else {
        //       spot.grab_placement().set_orientation(ROTATION_AXIS_Y, -90.0 * CLHEP::degree);
        //     }
        //     spot.set_errors(sigma_y, sigma_z);
        //   } else ...
        //
        if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
          DT_LOG_TRACE(get_logging_priority(), "Vertex:");
          spot.tree_dump(std::clog);
        }
      }

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

    // static
    void vertex_extrapolation_driver::init_ocd(datatools::object_configuration_description & ocd_)
    {

      // Prefix "VED" stands for "Vertex Extrapolation Driver" :
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "VED.");

      {
        // Description of the 'VED.use_linear_extrapolation' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("VED.use_linear_extrapolation")
          .set_from("snemo::reconstruction::vertex_extrapolation_driver")
          .set_terse_description("Use a linear extrapolation to determine vertex position")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_mandatory(false)
          .set_default_value_boolean(false)
          .set_long_description("Feature to be implemented")
          .add_example("Set the default value::                      \n"
                       "                                             \n"
                       "  VED.use_linear_extrapolation : boolean = 0 \n"
                       "                                             \n"
                       )
          ;
      }
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::vertex_extrapolation_driver,ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::vertex_extrapolation_driver");
  ocd_.set_class_description("A driver class for vertex extrapolation algorithm");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation("This drivers does the extrapolation of tracker trajectories     \n"
                               "and builds the list of vertices attached to the particle track. \n"
                               );

  // Invoke specific OCD support :
  ::snemo::reconstruction::vertex_extrapolation_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::vertex_extrapolation_driver,
                               "snemo::reconstruction::vertex_extrapolation_driver")

// end of falaise/snemo/reconstruction/vertex_extrapolation_driver.cc
