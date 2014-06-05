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
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Driver is already initialized !");
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
      if (is_initialized())
        {
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
      if (setup_.has_key("locator_plugin_name"))
        {
          locator_plugin_name = setup_.fetch_string("locator_plugin_name");
        }
      else
        {
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
      _use_geiger_cell_layer_ = false;
      return;
    }

    void vertex_extrapolation_driver::process(const snemo::datamodel::tracker_trajectory & trajectory_,
                                              snemo::datamodel::particle_track           & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");

      this->_measure_foil_vertex_(trajectory_, particle_.grab_vertices());
      this->_measure_calorimeter_vertex_(trajectory_, particle_.grab_vertices());

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

    void vertex_extrapolation_driver::_measure_foil_vertex_(const snemo::datamodel::tracker_trajectory               & trajectory_,
                                                            snemo::datamodel::particle_track::vertex_collection_type & vertices_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Look first if trajectory pattern is an helix or not
      const snemo::datamodel::base_trajectory_pattern & a_track_pattern
        = trajectory_.get_pattern();

      const std::string & a_pattern_id = a_track_pattern.get_pattern_id();

      // Extrapolate foil vertex
      geomtools::vector_3d vertex;
      geomtools::invalidate(vertex);

      if (a_pattern_id == snemo::datamodel::line_trajectory_pattern::pattern_id())
        {
          const snemo::datamodel::line_trajectory_pattern * ptr_line
            = dynamic_cast<const snemo::datamodel::line_trajectory_pattern *>(&a_track_pattern);
          const geomtools::line_3d & a_line = ptr_line->get_segment();
          const geomtools::vector_3d & first = a_line.get_first();
          const geomtools::vector_3d & last  = a_line.get_last();
          const geomtools::vector_3d direction = first - last;

          const bool is_first = std::fabs(first.x()) < std::fabs(last.x());

          const double x = 0.0 * CLHEP::mm;
          const double y = direction.y()/direction.x() *(x - first.x()) + first.y();
          const double z = direction.z()/direction.y() *(y - first.y()) + first.z();

          // Set value
          vertex.set(x, y, z);
          if (geomtools::is_valid(vertex))
            {
              // Create a mutable line object to set the new angle
              geomtools::line_3d * a_mutable_line = const_cast<geomtools::line_3d *>(&a_line);
              if (is_first) a_mutable_line->set_first(vertex);
              else          a_mutable_line->set_last (vertex);
            }
        }
      else if (a_pattern_id == snemo::datamodel::helix_trajectory_pattern::pattern_id())
        {
          const snemo::datamodel::helix_trajectory_pattern * ptr_helix
            = dynamic_cast<const snemo::datamodel::helix_trajectory_pattern *>(&a_track_pattern);
          const geomtools::helix_3d & a_helix = ptr_helix->get_helix();

          DT_LOG_TRACE(get_logging_priority(), "Helix initial angles:");
          DT_LOG_TRACE(get_logging_priority(), "angle 1 = " << a_helix.get_angle1());
          DT_LOG_TRACE(get_logging_priority(), "angle 2 = " << a_helix.get_angle2());

          const double xfoil   = 0.0 * CLHEP::mm;
          const double xcenter = a_helix.get_center().x();
          const double cangle  = (xfoil - xcenter) / a_helix.get_radius();

          if (std::fabs(cangle) < 1.0)
            {
              // Determine new angle
              const geomtools::vector_3d first_point = a_helix.get_first();
              const geomtools::vector_3d last_point  = a_helix.get_last();
              const bool is_angle1 = std::fabs(first_point.x()) < std::fabs(last_point.x());

              // Changing angle sign since acos function gives a 0 to PI angle
              double angle = std::acos(cangle);
              if (is_angle1  && a_helix.get_angle1()*angle < 0.0) angle *= -1.0;
              if (!is_angle1 && a_helix.get_angle2()*angle < 0.0) angle *= -1.0;

              // Create a mutable helix object to set the new angle
              geomtools::helix_3d * a_mutable_helix = const_cast<geomtools::helix_3d *>(&a_helix);
              if (is_angle1) a_mutable_helix->set_angle1(angle);
              else           a_mutable_helix->set_angle2(angle);

              if (is_angle1) vertex = a_helix.get_first();
              else           vertex = a_helix.get_last ();
            }
          else if (_use_geiger_cell_layer_)
            {
              // If no vertex found then try a very basic way by using the closest
              // cell position wrt to source foil (i.e. the first layer cells)
              DT_LOG_TRACE(get_logging_priority(),
                           "Looking for source foil vertex from Geiger cell");
              // Get Geiger locator to extract cell layer
              const snemo::geometry::gg_locator & gg_locator
                = dynamic_cast<const snemo::geometry::gg_locator&>(_locator_plugin_->get_gg_locator());
              const snemo::datamodel::tracker_cluster & a_cluster = trajectory_.get_cluster();
              const snemo::datamodel::calibrated_tracker_hit::collection_type & hits
                = a_cluster.get_hits();
              for (snemo::datamodel::calibrated_tracker_hit::collection_type::const_iterator
                     igg = hits.begin();
                   igg != hits.end(); ++igg)
                {
                  const snemo::datamodel::calibrated_tracker_hit & a_gg_hit = igg->get();
                  const uint32_t layer = gg_locator.extract_layer(a_gg_hit.get_geom_id());
                  if (layer != 0) continue;
                  // Build a vertex given the position of the closest cell
                  gg_locator.get_cell_position(a_gg_hit.get_geom_id(), vertex);
                  vertex.setX(0.0);
                  break;
                }
            }
        }

      if (geomtools::is_valid(vertex))
        {
          snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot());
          vertices_.push_back(hBS);
          hBS.grab().set_blur_dimension(geomtools::blur_spot::dimension_three);
          hBS.grab().set_position(vertex);
          hBS.grab().grab_auxiliaries().update_flag("foil_vertex");
          if (get_logging_priority() >= datatools::logger::PRIO_TRACE)
            {
              DT_LOG_TRACE(get_logging_priority(), "Foil vertex:");
              hBS.get().tree_dump(std::clog);
            }
        }

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

    void vertex_extrapolation_driver::_measure_calorimeter_vertex_(const snemo::datamodel::tracker_trajectory               & trajectory_,
                                                                   snemo::datamodel::particle_track::vertex_collection_type & vertices_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Extract the side from the geom_id of the tracker_trajectory object:
      if (! trajectory_.has_geom_id())
        {
          DT_LOG_ERROR(get_logging_priority(), "Tracker trajectory has no geom_id! Abort!")
            return;
        }
      const geomtools::geom_id & gid = trajectory_.get_geom_id();
      const geomtools::id_mgr & id_mgr = get_geometry_manager().get_id_mgr();
      if (!id_mgr.has(gid, "module") || !id_mgr.has(gid, "side"))
        {
          DT_LOG_ERROR(get_logging_priority(), "Trajectory geom_id " << gid << " has no 'module' or 'side' address!");
          return;
        }
      else
        {
          DT_LOG_TRACE(get_logging_priority(), "Trajectory geom_id = " << gid);
        }
      const int side = id_mgr.get(gid, "side");

      // Set the calorimeter locators :
      const snemo::geometry::calo_locator  & calo_locator  = _locator_plugin_->get_calo_locator();
      const snemo::geometry::xcalo_locator & xcalo_locator = _locator_plugin_->get_xcalo_locator();
      const snemo::geometry::gveto_locator & gveto_locator = _locator_plugin_->get_gveto_locator();

      // Given the 'module' and the 'side' address get the calorimeter bounds
      // 2013-03-11 XG: There is a possibility that the helix track will cross
      // the oppposite calorimeter wall. So we calculate the extrapolation on
      // the two main wall sides and we reject later the vertex on the other
      // side (cf. ~ line 800)
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

      // Extrapolate calorimeter vertex
      geomtools::vector_3d vertex;
      geomtools::invalidate(vertex);

      // Add a flag into 'blur_spot' auxiliaries to refer to the hit calo
      std::string calo_category_flag;
      if (a_pattern_id == snemo::datamodel::line_trajectory_pattern::pattern_id())
        {
          const snemo::datamodel::line_trajectory_pattern * ptr_line
            = dynamic_cast<const snemo::datamodel::line_trajectory_pattern *>(&a_track_pattern);
          const geomtools::line_3d & a_line = ptr_line->get_segment();
          const geomtools::vector_3d & first = a_line.get_first();
          const geomtools::vector_3d & last  = a_line.get_last();
          const geomtools::vector_3d direction = first - last;

          // Determine which line point should be modified (first or
          // last point)
          bool is_first = true;
          bool found_vertex = false;

          // Calorimeter walls
          if (!found_vertex)
            {
              DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on main wall...");

              is_first = std::fabs(first.x()) > std::fabs(last.x());
              const double x = xcalo_bd[side];
              const double y = direction.y()/direction.x() *(x - first.x()) + first.y();
              const double z = direction.z()/direction.y() *(y - first.y()) + first.z();

              // Extrapolate vertex
              const geomtools::vector_3d a_vertex(x, y, z);

              if (a_vertex.y() > ycalo_bd[snemo::geometry::xcalo_locator::WALL_RIGHT] ||
                  a_vertex.y() < ycalo_bd[snemo::geometry::xcalo_locator::WALL_LEFT])
                {
                  DT_LOG_TRACE(get_logging_priority(), "Cross xwall before!");
                  // Track meets xcalo before main wall
                  found_vertex = false;
                }
              else if (a_vertex.z() > zcalo_bd[snemo::geometry::gveto_locator::WALL_TOP] ||
                       a_vertex.z() < zcalo_bd[snemo::geometry::gveto_locator::WALL_BOTTOM])
                {
                  DT_LOG_TRACE(get_logging_priority(), "Cross gveto before!");
                  // Track meets gveto before main wall
                  found_vertex = false;
                }
              else
                {
                  // Copy vertex
                  vertex = a_vertex;
                  found_vertex = true;
                  calo_category_flag = "calo";
                }
            }// end of main wall search

          // Calorimeter on xwalls
          if (!found_vertex)
            {
              DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on X-wall...");

              is_first = std::fabs(first.y()) > std::fabs(last.y());
              for (size_t iwall = 0; iwall < snemo::geometry::xcalo_locator::NWALLS_PER_SIDE; ++iwall)
                {
                  const double y = ycalo_bd[iwall];
                  const double z = direction.z()/direction.y() *(y - first.y()) + first.z();
                  const double x = direction.x()/direction.y() *(y - first.y()) + first.x();

                  // Extrapolate vertex
                  const geomtools::vector_3d a_vertex(x, y, z);

                  if (std::fabs(a_vertex.x()) > std::fabs(xcalo_bd[side]))
                    {
                      DT_LOG_TRACE(get_logging_priority(), "Cross main wall before!");
                      // Track meets main wall before x-wall
                      found_vertex = false;
                    }
                  else if (a_vertex.z() > zcalo_bd[snemo::geometry::gveto_locator::WALL_TOP] ||
                           a_vertex.z() < zcalo_bd[snemo::geometry::gveto_locator::WALL_BOTTOM])
                    {
                      DT_LOG_TRACE(get_logging_priority(), "Cross gveto before!");
                      // Track meets gveto before main wall
                      found_vertex = false;
                    }
                  else
                    {
                      // Copy vertex
                      vertex = a_vertex;
                      found_vertex = true;
                      calo_category_flag = "xcalo";
                      break;
                    }
                }
            }// end of x-wall search

          // Calorimeter on gveto
          if (!found_vertex)
            {
              DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on gamma veto...");
              is_first = std::fabs(first.z()) > std::fabs(last.z());
              for (size_t iwall = 0; iwall < snemo::geometry::gveto_locator::NWALLS_PER_SIDE; ++iwall)
                {
                  const double z = zcalo_bd[iwall];
                  const double y = direction.y()/direction.z() *(z - first.z()) + first.y();
                  const double x = direction.x()/direction.y() *(y - first.y()) + first.x();

                  // Extrapolate vertex
                  const geomtools::vector_3d a_vertex(x, y, z);

                  if (std::fabs(a_vertex.x()) > std::fabs(xcalo_bd[side]))
                    {
                      DT_LOG_TRACE(get_logging_priority(), "Cross main wall before!");
                      // Track meets main wall before x-wall
                      found_vertex = false;
                    }
                  else if (a_vertex.y() > ycalo_bd[snemo::geometry::xcalo_locator::WALL_RIGHT] ||
                           a_vertex.y() < ycalo_bd[snemo::geometry::xcalo_locator::WALL_LEFT])
                    {
                      DT_LOG_TRACE(get_logging_priority(), "Cross xwall before!");
                      // Track meets gveto before main wall
                      found_vertex = false;
                    }
                  else
                    {
                      // Copy vertex
                      vertex = a_vertex;
                      found_vertex = true;
                      calo_category_flag = "gveto";
                      break;
                    }
                }
            }// end of gveto search

          // Save new first and last position of the line
          if (geomtools::is_valid(vertex))
            {
              // Create a mutable line object to set the new position
              geomtools::line_3d * a_mutable_line = const_cast<geomtools::line_3d *>(&a_line);
              if (is_first) a_mutable_line->set_first(vertex);
              else          a_mutable_line->set_last (vertex);
            }
        }// end of line pattern
      else if (a_pattern_id == snemo::datamodel::helix_trajectory_pattern::pattern_id())
        {
          const snemo::datamodel::helix_trajectory_pattern * ptr_helix
            = dynamic_cast<const snemo::datamodel::helix_trajectory_pattern *>(&a_track_pattern);
          const geomtools::helix_3d & a_helix = ptr_helix->get_helix();

          // Extract helix parameters
          const geomtools::vector_3d & hcenter = a_helix.get_center();
          const double hradius                 = a_helix.get_radius();

          // Store all the computed t parameter values
          std::map<double, std::string> tparams;

          // Calorimeter walls
          {
            DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on main wall...");
            for (size_t iside = 0; iside < snemo::geometry::utils::NSIDES; ++iside)
              {
                const double xcenter = hcenter.x();
                const double cangle  = (xcalo_bd[iside] - xcenter) / hradius;

                if (std::fabs(cangle) < 1.0)
                  {
                    const double angle = std::acos(cangle);
                    tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(+angle), "calo"));
                    tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(-angle), "calo"));
                  }
              }
          }// end of main wall search

          // X-walls
          {
            DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on X-wall...");
            for (size_t iwall = 0; iwall < snemo::geometry::xcalo_locator::NWALLS_PER_SIDE; ++iwall)
              {
                const double ycenter = hcenter.y();
                const double cangle = (ycalo_bd[iwall] - ycenter) / hradius;

                if (std::fabs(cangle) < 1.0)
                  {
                    double angle = std::asin(cangle);
                    tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(angle), "xcalo"));
                    const double mean_angle =(a_helix.get_angle1() + a_helix.get_angle2())/2.0;
                    if (mean_angle < 0.0) angle = - M_PI - angle;
                    else                  angle = + M_PI - angle;
                    tparams.insert(std::make_pair(geomtools::helix_3d::angle_to_t(angle), "xcalo"));
                  }
              }// end of x-wall loop
          }// end of x-wall search

          // Gvetos
          {
            DT_LOG_TRACE(get_logging_priority(), "Looking for vertex on gamma veto...");
            for (size_t iwall = 0; iwall < snemo::geometry::xcalo_locator::NWALLS_PER_SIDE; ++iwall)
              {
                const double t = a_helix.get_t_from_z(zcalo_bd[iwall]);
                tparams.insert(std::make_pair(t, "gveto"));
              }
          }// end of gveto search

          if (get_logging_priority() >= datatools::logger::PRIO_TRACE)
            {
              DT_LOG_TRACE(get_logging_priority(), "Stored t parameters :");
              for (std::map<double, std::string>::const_iterator
                     it = tparams.begin(); it != tparams.end(); ++it)
                {
                  DT_LOG_TRACE(get_logging_priority(),  it->second << " angle = " << it->first);
                }
            }

          // Choose which helix angle to change
          const double t1 = a_helix.get_t1();
          const double t2 = a_helix.get_t2();

          double new_t; datatools::invalidate(new_t);
          double min_distance = std::numeric_limits<double>::infinity();
          bool is_t1 = true;

          for (std::map<double, std::string>::const_iterator it = tparams.begin();
               it != tparams.end(); ++it)
            {
              const double t = it->first;
              const std::string & category = it->second;

              // Calculate delta t values
              const double delta1 = std::fabs(t1 - t);
              const double delta2 = std::fabs(t2 - t);

              // Keep smallest distance but remove also too long extrapolation
              if (delta1 < delta2)
                {
                  if (delta1 > min_distance || delta1 > 1.0) continue;
                  is_t1 = true;
                  new_t = t;
                  calo_category_flag = category;
                  min_distance = delta1;
                }
              else
                {
                  if (delta2 > min_distance || delta2 > 1.0) continue;
                  is_t1 = false;
                  new_t = t;
                  calo_category_flag = category;
                  min_distance = delta2;
                }
            }

          // No angle has been found -> no calorimeter vertices
          if (! datatools::is_valid(new_t))
            {
              DT_LOG_TRACE(get_logging_priority(), "No vertices on calorimeter has been found!");
              return;
            }

          // Check vertex side is on the same side as the trajectory (for xwall)
          const geomtools::vector_3d xy = a_helix.get_point(new_t);
          if ((side == snemo::geometry::utils::SIDE_BACK  && xy.x() > 0.0) ||
              (side == snemo::geometry::utils::SIDE_FRONT && xy.x() < 0.0))
            {
              DT_LOG_TRACE(get_logging_priority(), "Closest vertex is on the opposite side!");
              return;
            }


          // New angle & calorimeter category
          // Create a mutable helix object to set the new angle
          geomtools::helix_3d * a_mutable_helix = const_cast<geomtools::helix_3d *>(&a_helix);
          if (is_t1)
            {
              a_mutable_helix->set_t1(new_t);
              vertex = a_mutable_helix->get_first();
            }
          else
            {
              a_mutable_helix->set_t2(new_t);
              vertex = a_mutable_helix->get_last();
            }
        }// end of helix pattern

      // Save new vertex
      if (geomtools::is_valid(vertex))
        {
          snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot());
          vertices_.push_back(hBS);
          hBS.grab().set_blur_dimension(geomtools::blur_spot::dimension_three);
          hBS.grab().set_position(vertex);
          if (!calo_category_flag.empty())
            hBS.grab().grab_auxiliaries().update_flag(calo_category_flag);
          if (get_logging_priority() >= datatools::logger::PRIO_TRACE)
            {
              DT_LOG_TRACE(get_logging_priority(), "Calorimeter vertex:");
              hBS.get().tree_dump(std::clog);
            }
        }

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

// end of falaise/snemo/reconstruction/vertex_extrapolation_driver.cc
