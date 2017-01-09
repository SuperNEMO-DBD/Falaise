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
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace reconstruction {

    const std::string & calorimeter_utils::associated_flag()
    {
      static const std::string s("__associated");
      return s;
    }

    const std::string & calorimeter_utils::isolated_flag()
    {
      static const std::string s("__isolated");
      return s;
    }

    const std::string & calorimeter_utils::neighbor_flag()
    {
      static const std::string s("__neighbour");
      return s;
    }

    void calorimeter_utils::flag_as(const snemo::datamodel::calibrated_calorimeter_hit & hit_, const std::string & flag_)
    {
      snemo::datamodel::calibrated_calorimeter_hit & mutable_hit = const_cast<snemo::datamodel::calibrated_calorimeter_hit&>(hit_);
      mutable_hit.grab_auxiliaries().update_flag(flag_);
      return;
    }

    bool calorimeter_utils::has_flag(const snemo::datamodel::calibrated_calorimeter_hit & hit_, const std::string & flag_)
    {
      return hit_.get_auxiliaries().has_flag(flag_);
    }

    const std::string & calorimeter_association_driver::get_id()
    {
      static const std::string s("CAD");
      return s;
    }

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
      if (is_initialized()) {
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
      if (setup_.has_key("matching_tolerance")) {
        _matching_tolerance_ = setup_.fetch_real("matching_tolerance");
        if (! setup_.has_explicit_unit("matching_tolerance")) {
          _matching_tolerance_ *= CLHEP::mm;
        }
      }

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
    void calorimeter_association_driver::reset()
    {
      _set_defaults();
      return;
    }

    void calorimeter_association_driver::_set_defaults()
    {
      _initialized_      = false;
      _logging_priority_ = datatools::logger::PRIO_WARNING;

      _geometry_manager_ = 0;
      _locator_plugin_ = 0;

      _matching_tolerance_ = 50 * CLHEP::mm;
      return;
    }

    void calorimeter_association_driver::process
    (const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_,
     snemo::datamodel::particle_track & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");

      this->_measure_matching_calorimeters_(calorimeter_hits_, particle_);

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

    void calorimeter_association_driver::_measure_matching_calorimeters_
    (const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calorimeter_hits_,
     snemo::datamodel::particle_track & particle_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      if (! particle_.has_vertices()) {
        DT_LOG_DEBUG(get_logging_priority(), "No vertices have been found for the current particle!");
        return;
      }

      // When considering calorimeter hits, there might be some of them that are
      // neighbors. To avoid double count of same calorimeter due to tolerance
      // matching in calorimeter association driver, we first compute the list
      // of neighborhood calorimeter hits and if a vertex matches two of them,
      // we only keep the closest non-associated one.
      //
      //                     |-------
      //                     |      |
      //   particle track 1  |      |
      //  -------------------+      | Here particle track 1 is associated to 2
      //                     |------- calorimeter hits but one is shared with the
      //                     |------- particle 2 : we removed the block already associated
      //                     |      |
      //   particle track 2  |      |
      //  -------------------+      |
      //                     |-------
      //
      // Set the calorimeter locators :
      const snemo::geometry::calo_locator  & calo_locator  = _locator_plugin_->get_calo_locator();
      const snemo::geometry::xcalo_locator & xcalo_locator = _locator_plugin_->get_xcalo_locator();
      const snemo::geometry::gveto_locator & gveto_locator = _locator_plugin_->get_gveto_locator();
      // Get a list of neighbouring calorimeter hits
      std::set<geomtools::geom_id> list_of_neighbours;
      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator
             icalo = calorimeter_hits_.begin(); icalo != calorimeter_hits_.end();
           ++icalo) {
        const snemo::datamodel::calibrated_calorimeter_hit & i_calo_hit = icalo->get();
        const geomtools::geom_id & a_current_gid = i_calo_hit.get_geom_id();

        std::vector<geomtools::geom_id> neighbour_ids;
        if (calo_locator.is_calo_block_in_current_module(a_current_gid)) {
          calo_locator.get_neighbours_ids(a_current_gid, neighbour_ids);
        } else if (xcalo_locator.is_calo_block_in_current_module(a_current_gid)) {
          xcalo_locator.get_neighbours_ids(a_current_gid, neighbour_ids);
        } else if (gveto_locator.is_calo_block_in_current_module(a_current_gid)) {
          gveto_locator.get_neighbours_ids(a_current_gid, neighbour_ids);
        }
        for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator
               jcalo = boost::next(icalo); jcalo != calorimeter_hits_.end();
             ++jcalo) {
          const snemo::datamodel::calibrated_calorimeter_hit & j_calo_hit = jcalo->get();
          const geomtools::geom_id & a_gid = j_calo_hit.get_geom_id();

          if (std::find(neighbour_ids.begin(), neighbour_ids.end(), a_gid) != neighbour_ids.end()) {
            list_of_neighbours.insert(a_gid);
            list_of_neighbours.insert(a_current_gid);
            calorimeter_utils::flag_as(i_calo_hit, calorimeter_utils::neighbor_flag());
            calorimeter_utils::flag_as(j_calo_hit, calorimeter_utils::neighbor_flag());
          }
        }
      }
      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        DT_LOG_TRACE(get_logging_priority(), "Number of neighbours " << list_of_neighbours.size());
        for (std::set<geomtools::geom_id>::const_iterator i = list_of_neighbours.begin();
             i != list_of_neighbours.end(); ++i) {
          DT_LOG_TRACE(get_logging_priority(), "Neighbours @ " << *i);
        }
      }

      // Loop over reconstructed vertices
      snemo::datamodel::particle_track::vertex_collection_type & the_vertices
        = particle_.grab_vertices();
      for (snemo::datamodel::particle_track::vertex_collection_type::iterator
             ivertex = the_vertices.begin();
           ivertex != the_vertices.end(); ++ivertex) {
        geomtools::blur_spot & a_vertex = ivertex->grab();
        if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
          DT_LOG_TRACE(get_logging_priority(), "Vertex:");
          a_vertex.tree_dump(std::clog, "", "[trace]: ");
        }

        // Do not take care of vertex other than the ones on calorimeters
        if (! snemo::datamodel::particle_track::vertex_is_on_main_calorimeter(a_vertex) &&
            ! snemo::datamodel::particle_track::vertex_is_on_x_calorimeter(a_vertex) &&
            ! snemo::datamodel::particle_track::vertex_is_on_gamma_veto(a_vertex)) continue;

        // Look for matching calorimeters
        typedef std::map<double, snemo::datamodel::calibrated_calorimeter_hit::handle_type> calo_collection_type;
        calo_collection_type calo_collection;
        for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator
               icalo = calorimeter_hits_.begin(); icalo != calorimeter_hits_.end();
             ++icalo) {
          const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = icalo->get();
          const geomtools::geom_id & a_current_gid = a_calo_hit.get_geom_id();

          // Getting geometry mapping for parted block
          const geomtools::mapping & the_mapping = get_geometry_manager().get_mapping();
          std::vector<geomtools::geom_id> gids;
          the_mapping.compute_matching_geom_id(a_current_gid, gids);
          for (size_t i = 0; i < gids.size(); ++i) {
            const geomtools::geom_id & a_gid = gids.at(i);
            const geomtools::geom_info * ginfo_ptr = the_mapping.get_geom_info_ptr(a_gid);
            if (! ginfo_ptr) {
              DT_LOG_WARNING(get_logging_priority(), "Unmapped geom id " << a_gid << "!");
              continue;
            }

            // Tolerance must be understood as 'skin' tolerance so must be
            // multiplied by a factor of 2
            const double tolerance = _matching_tolerance_;
            if (the_mapping.check_inside(*ginfo_ptr, a_vertex.get_position(), tolerance, true)) {
              DT_LOG_DEBUG(get_logging_priority(), "Found matching calorimeter with the following geom_id " << a_gid);
              // Compute distance to calorimeter center
              geomtools::vector_3d calo_position;
              geomtools::invalidate(calo_position);
              if (calo_locator.is_calo_block_in_current_module(a_gid)) {
                calo_locator.get_block_position(a_gid, calo_position);
              } else if (xcalo_locator.is_calo_block_in_current_module(a_current_gid)) {
                xcalo_locator.get_block_position(a_gid, calo_position);
              } else if (gveto_locator.is_calo_block_in_current_module(a_current_gid)) {
                gveto_locator.get_block_position(a_gid, calo_position);
              }
              const double distance = (calo_position - a_vertex.get_position()).mag();
              calo_collection.insert(std::make_pair(distance, *icalo));
            } else {
              // Try in a different way
              DT_LOG_DEBUG(get_logging_priority(), "No matching calorimeter !");
            }
          } // end of calorimeter gids
        }// end of calorimeter hits

        // 2012-06-15 XG: If no triggered calorimeter has been associated to
        // track, one may try to find one silent calorimeter by using the
        // 'calo_locator' and finding the corresponding calorimeter block. To be
        // continued...
        if (calo_collection.empty()) continue;
        DT_LOG_TRACE(get_logging_priority(), "Number of associated calorimeter = " << calo_collection.size());

        // Keep only closest calorimeter i.e. the one with the smallest distance
        // within calo_collection type
        for (auto & icalo : calo_collection) {
          icalo.second.grab().grab_auxiliaries().clean(calorimeter_utils::associated_flag());
        }
        const snemo::datamodel::calibrated_calorimeter_hit & a_calo = calo_collection.begin()->second.get();
        const geomtools::geom_id & a_gid = a_calo.get_geom_id();
        particle_.grab_associated_calorimeter_hits().push_back(calo_collection.begin()->second);
        // Add a private property
        calorimeter_utils::flag_as(a_calo, calorimeter_utils::associated_flag());
        // Set the geom_id of the corresponding vertex to the calorimeter hit geom_id
        a_vertex.set_geom_id(a_gid);

        // for (calo_collection_type::const_iterator i = calo_collection.begin();
        //      i != calo_collection.end(); ++i) {
        //   const snemo::datamodel::calibrated_calorimeter_hit & a_calo = i->second.get();
        //   const geomtools::geom_id & a_gid = a_calo.get_geom_id();
        //   // Check association and belonging to neighbours
        //   if (std::find(list_of_neighbours.begin(), list_of_neighbours.end(), a_gid) != list_of_neighbours.end() &&
        //       calorimeter_utils::has_flag(a_calo, calorimeter_utils::associated_flag())) {
        //     continue;
        //   }
        //   particle_.grab_associated_calorimeter_hits().push_back(i->second);
        //   // Add a private property
        //   calorimeter_utils::flag_as(a_calo, calorimeter_utils::associated_flag());
        //   // Set the geom_id of the corresponding vertex to the calorimeter hit geom_id
        //   a_vertex.set_geom_id(a_gid);
        // } // end of calorimeter collection
      }// end of vertices

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }
    // static
    void calorimeter_association_driver::init_ocd(datatools::object_configuration_description & ocd_)
    {

      // Prefix "CAD" stands for "Calorimeter Association Driver" :
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "CAD.");

      {
        // Description of the 'VED.matching_tolerance' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("VED.matching_tolerance")
          .set_from("snemo::reconstruction::calorimeter_association_driver")
          .set_terse_description("Set matching length tolerance between vertex track and calo. block")
          .set_traits(datatools::TYPE_REAL)
          .set_mandatory(false)
          .set_explicit_unit(true)
          .set_unit_label("length")
          .set_unit_symbol("mm")
          .set_default_value_real(50.)
          .add_example("Set the default value::                           \n"
                       "                                                  \n"
                       "  CAD.matching_tolerance : real as length = 50 mm \n"
                       "                                                  \n"
                       )
          ;
      }
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::calorimeter_association_driver,ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::calorimeter_assocation_driver");
  ocd_.set_class_description("A driver class for calorimeter association algorithm");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation("This drivers does the association between a particle track and a calorimter hit. \n"
                               );

  // Invoke specific OCD support :
  ::snemo::reconstruction::calorimeter_association_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::calorimeter_association_driver,
                               "snemo::reconstruction::calorimeter_association_driver")

// end of falaise/snemo/reconstruction/calorimeter_association_driver.cc
