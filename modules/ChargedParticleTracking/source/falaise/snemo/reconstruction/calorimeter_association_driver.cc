// \file falaise/snemo/reconstruction/calorimeter_association_driver.cc

// Ourselves:
#include <falaise/snemo/reconstruction/calorimeter_association_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace reconstruction {

    void calorimeter_association_driver::set_initialized(const bool initialized_)
    {
      _initialized_ = initialized_;
      return;
    }

    bool calorimeter_association_driver::is_initialized() const
    {
      return _initialized_;
    }

    void calorimeter_association_driver::set_logging_priority(const datatools::logger::priority priority_)
    {
      _logging_priority_ = priority_;
      return;
    }

    datatools::logger::priority calorimeter_association_driver::get_logging_priority() const
    {
      return _logging_priority_;
    }

    bool calorimeter_association_driver::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    void calorimeter_association_driver::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & calorimeter_association_driver::get_geometry_manager() const
    {
      DT_THROW_IF(! has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *_geometry_manager_;
    }

    /// Constructor
    calorimeter_association_driver::calorimeter_association_driver()
    {
      _set_defaults();
      return;
    }

    /// Destructor
    calorimeter_association_driver::~calorimeter_association_driver()
    {
      if (is_initialized())
        {
          reset();
        }
      return;
    }

    /// Initialize the driver through configuration properties
    void calorimeter_association_driver::initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");

      DT_THROW_IF(! has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(! get_geometry_manager().is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      // Logging priority
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration (setup_);
      DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority level for geometry manager !");
      set_logging_priority(lp);

      // Matching distance tolerance for calorimeter association
      if (setup_.has_key("matching_tolerance"))
        {
          _matching_tolerance_ = setup_.fetch_real("matching_tolerance");
          if (! setup_.has_explicit_unit("matching_tolerance")) {
            _matching_tolerance_ *= CLHEP::mm;
          }
        }

      if (setup_.has_key("use_last_geiger_cell"))
        {
          _use_last_geiger_cell_ = setup_.fetch_boolean("use_last_geiger_cell");
        }

      set_initialized(true);
      return;
    }

    /// Reset the driver
    void calorimeter_association_driver::reset()
    {
      _set_defaults();
      return;
    }

    void calorimeter_association_driver::_set_defaults()
    {
      _initialized_          = false;
      _logging_priority_     = datatools::logger::PRIO_WARNING;

      _matching_tolerance_   = 50 * CLHEP::mm;
      _use_last_geiger_cell_ = false;
      return;
    }

    void calorimeter_association_driver::process
    (const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_,
     snemo::datamodel::particle_track & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");

      if (_use_last_geiger_cell_)
        {
          DT_LOG_WARNING(get_logging_priority(), "Association mode using Geiger cell is not implemented yet !");
          return;
        }

      this->_measure_matching_calorimeters_(calorimeter_hits_, particle_);

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

    void calorimeter_association_driver::_measure_matching_calorimeters_
    (const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_,
     snemo::datamodel::particle_track & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      if (! particle_.has_vertices())
        {
          DT_LOG_DEBUG(get_logging_priority(), "No vertices have been found for the current particle!");
          return;
        }

      const snemo::datamodel::particle_track::vertex_collection_type & the_vertices
        = particle_.get_vertices();
      for (snemo::datamodel::particle_track::vertex_collection_type::const_iterator
             ivertex = the_vertices.begin();
           ivertex != the_vertices.end(); ++ivertex)
        {
          const geomtools::blur_spot & a_vertex = ivertex->get();

          // Only take care of main wall and x-calorimeter vertex
          // 2014-06-05 XG: I am not sure we need to be so strict...
          if (! a_vertex.get_auxiliaries().has_flag("calo") &&
              ! a_vertex.get_auxiliaries().has_flag("xcalo")) continue;

          if (get_logging_priority() >= datatools::logger::PRIO_TRACE)
            {
              DT_LOG_TRACE(get_logging_priority(), "Vertex:");
              a_vertex.tree_dump(std::clog);
            }

          // Look for matching calorimeters
          for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator
                 icalo = calorimeter_hits_.begin();
               icalo != calorimeter_hits_.end(); ++icalo)
            {
              const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = icalo->get();
              const geomtools::geom_id & gid = a_calo_hit.get_geom_id();

              bool has_associated_calorimeter = false;

              // Getting geometry mapping
              const geomtools::mapping & the_mapping = get_geometry_manager().get_mapping();
              std::vector<geomtools::geom_id> gids;
              the_mapping.compute_matching_geom_id(gid, gids);
              for (size_t i = 0; i < gids.size(); ++i)
                {
                  const geomtools::geom_id & a_gid = gids.at(i);
                  const geomtools::geom_info * ginfo_ptr = the_mapping.get_geom_info_ptr(a_gid);
                  if (! ginfo_ptr)
                    {
                      DT_LOG_WARNING(get_logging_priority(), "Unmapped geom id " << a_gid << "!");
                      continue;
                    }

                  // Tolerance must be understood as 'skin' tolerance
                  // so must be multiplied by a factor of 2
                  const double tolerance = _matching_tolerance_;
                  if (the_mapping.check_inside(*ginfo_ptr, a_vertex.get_position(), tolerance, true))
                    {
                      DT_LOG_DEBUG(get_logging_priority(), "Found matching calorimeter with the following geom_id " << a_gid);
                      has_associated_calorimeter = true;
                    }
                  else
                    {
                      // Try in a different way
                      DT_LOG_DEBUG(get_logging_priority(), "No matching calorimeter !");
                    }
                }// end of calorimeter gids
              if (has_associated_calorimeter)
                {
                  particle_.grab_associated_calorimeters().push_back(*icalo);
                  // Add a private property
                  snemo::datamodel::calibrated_calorimeter_hit * mutable_hit
                    = const_cast<snemo::datamodel::calibrated_calorimeter_hit *>(&(a_calo_hit));
                  mutable_hit->grab_auxiliaries().update_flag("__associated");
                }
            }// end of calorimeter hits

          // 2012-06-15 XG: If no triggered calorimeter has been
          // associated to track, one may try to find one silent
          // calorimeter by using the 'calo_locator' and finding
          // the corresponding calorimeter block. To be
          // continued...

        }// end of vertices

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

// end of falaise/snemo/reconstruction/calorimeter_association_driver.cc
