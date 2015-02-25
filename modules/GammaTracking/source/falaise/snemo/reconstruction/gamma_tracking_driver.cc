/// \file falaise/snemo/reconstruction/gamma_tracking_driver.cc

// Ourselves:
#include <snemo/reconstruction/gamma_tracking_driver.h>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

// Gamma_Tracking library
#include <GammaTracking/event.h>
#include <GammaTracking/tof_computing.h>

namespace snemo {

  namespace reconstruction {

    const std::string & gamma_tracking_driver::gamma_tracking_id()
    {
      static const std::string _id("gamma_tracking");
      return _id;
    }

    void gamma_tracking_driver::set_initialized(const bool initialized_)
    {
      _initialized_ = initialized_;
      return;
    }

    bool gamma_tracking_driver::is_initialized() const
    {
      return _initialized_;
    }

    void gamma_tracking_driver::set_logging_priority(const datatools::logger::priority priority_)
    {
      _logging_priority_ = priority_;
      return;
    }

    datatools::logger::priority gamma_tracking_driver::get_logging_priority() const
    {
      return _logging_priority_;
    }


    void gamma_tracking_driver::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Driver is already initialized !");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & gamma_tracking_driver::get_geometry_manager() const
    {
      DT_THROW_IF(! has_geometry_manager(), std::logic_error,
                  "No geometry manager is setup !");
      return *_geometry_manager_;
    }

    bool gamma_tracking_driver::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    // Constructor
    gamma_tracking_driver::gamma_tracking_driver()
    {
      _set_defaults();
      set_initialized(false);
      return;
    }

    // Destructor
    gamma_tracking_driver::~gamma_tracking_driver()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    // Initialize the gamma tracker through configuration properties
    void gamma_tracking_driver::initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver '" << gamma_tracking_id() << "' is already initialized !");

      DT_THROW_IF(! has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(! get_geometry_manager().is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      // Logging priority
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration(setup_);
      DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED, std::logic_error,
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

      // Extract the setup of the gamma tracking algo :
      datatools::properties gt_setup;
      setup_.export_and_rename_starting_with(gt_setup, "GT.", "");

      // Initialize Gamma Tracking
      _gt_.initialize(gt_setup);

      set_initialized(true);
      return;
    }

    // Reset the gamma tracker
    void gamma_tracking_driver::reset()
    {
      _set_defaults();
      set_initialized(false);
      return;
    }

    int gamma_tracking_driver::process(const snemo::datamodel::calibrated_calorimeter_hit::collection_type & hits_,
                                       snemo::datamodel::particle_track_data & ptd_)
    {
      int status = 0;
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << gamma_tracking_id() << "' is already initialized !");

      status = _prepare_process(hits_, ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Pre-processing of calorimeter hits by '" << gamma_tracking_id() << "' algorithm has failed !");
        return status;
      }

      status = _process_algo(hits_, ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of calorimeter hits by '" << gamma_tracking_id() << "' algorithm has failed !");
        return status;
      }

      status = _post_process(ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Post-processing of calorimeter hits by '" << gamma_tracking_id() << "' algorithm has failed !");
        return status;
      }

      return status;
    }

    void gamma_tracking_driver::_set_defaults()
    {
      _logging_priority_ = datatools::logger::PRIO_WARNING;
      _geometry_manager_ = 0;
      _locator_plugin_ = 0;
      return;
    }

    int gamma_tracking_driver::_prepare_process(const snemo::datamodel::calibrated_calorimeter_hit::collection_type & hits_,
                                                snemo::datamodel::particle_track_data & /*ptd_*/)
    {
      gt::event & an_event = _gt_.grab_event();
      gt::event::calorimeter_collection_type & the_gamma_calos = an_event.grab_calorimeters();

      for (snemo::datamodel::calibrated_calorimeter_hit::collection_type::const_iterator
             icalo = hits_.begin(); icalo != hits_.end(); ++icalo) {
        const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = icalo->get();
        {
          gt::event::calorimeter_hit dummy_hit;
          the_gamma_calos.insert(std::make_pair(a_calo_hit.get_hit_id(), dummy_hit));
        }
        gt::event::calorimeter_hit & new_calo_hit = the_gamma_calos[a_calo_hit.get_hit_id()];

        const geomtools::geom_id & a_gid = a_calo_hit.get_geom_id();
        const snemo::geometry::calo_locator & calo_locator   = _locator_plugin_->get_calo_locator();
        const snemo::geometry::xcalo_locator & xcalo_locator = _locator_plugin_->get_xcalo_locator();
        const snemo::geometry::gveto_locator & gveto_locator = _locator_plugin_->get_gveto_locator();
        if (calo_locator.is_calo_block_in_current_module(a_gid)) {
          calo_locator.get_block_position(a_gid, new_calo_hit.position);
          new_calo_hit.label = snemo::datamodel::particle_track::vertex_on_main_calorimeter_label();
        } else if (xcalo_locator.is_calo_block_in_current_module(a_gid)) {
          xcalo_locator.get_block_position(a_gid, new_calo_hit.position);
          new_calo_hit.label = snemo::datamodel::particle_track::vertex_on_x_calorimeter_label();
        } else if (gveto_locator.is_calo_block_in_current_module(a_gid)) {
          gveto_locator.get_block_position(a_gid, new_calo_hit.position);
          new_calo_hit.label = snemo::datamodel::particle_track::vertex_on_gamma_veto_label();
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "Current geom id '" << a_gid << "' does not match any scintillator block !");
        }

        new_calo_hit.time         = a_calo_hit.get_time();
        new_calo_hit.sigma_time   = a_calo_hit.get_sigma_time();
        new_calo_hit.energy       = a_calo_hit.get_energy();
        new_calo_hit.sigma_energy = a_calo_hit.get_sigma_energy();
      }

      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG(get_logging_priority(), "Event dump: " << an_event);
      }
      _gt_.prepare_process();

      return 0;
    }

    // Main tracking method
    int gamma_tracking_driver::_process_algo(const snemo::datamodel::calibrated_calorimeter_hit::collection_type & hits_,
                                             snemo::datamodel::particle_track_data & ptd_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Running gamma tracking
      _gt_.process();
      gt::gamma_tracking::solution_type gamma_tracks;
      _gt_.get_reflects(gamma_tracks);
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG(get_logging_priority(), "Number of gammas = " << gamma_tracks.size());
        _gt_.dump();
      }

      for (gt::gamma_tracking::solution_type::const_iterator
             it = gamma_tracks.begin(); it != gamma_tracks.end(); ++it) {
        DT_LOG_TRACE(get_logging_priority(), "Adding a new gamma");
        const gt::gamma_tracking::list_type & a_list = *it;
        snemo::datamodel::particle_track::handle_type hPT(new snemo::datamodel::particle_track);
        hPT.grab().set_track_id(ptd_.get_number_of_particles());
        hPT.grab().set_charge(snemo::datamodel::particle_track::neutral);
        ptd_.add_particle(hPT);

        // List of associated calorimeters
        for (gt::gamma_tracking::list_type::const_iterator jt = a_list.begin();
             jt != a_list.end(); ++jt) {
          const int calo_id = *jt;
          // Set calorimeter association
          geomtools::base_hit::has_hit_id_predicate hit_pred(calo_id);
          datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                                  snemo::datamodel::calibrated_calorimeter_hit> pred_M2D(hit_pred);
          datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(pred_M2D);
          snemo::datamodel::calibrated_calorimeter_hit::collection_type::const_iterator
            found = std::find_if(hits_.begin(), hits_.end(), pred_via_handle);
          DT_THROW_IF(found == hits_.end(), std::logic_error,
                      "Calibrated calorimeter hit with id " << calo_id << " can not be found");
          hPT.grab().grab_associated_calorimeter_hits().push_back(*found);

          // Build vertex
          snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot);
          hPT.grab().grab_vertices().push_back(hBS);
          geomtools::blur_spot & spot = hBS.grab();
          spot.set_hit_id(calo_id);
          spot.set_geom_id(found->get().get_geom_id());
          spot.set_blur_dimension(geomtools::blur_spot::dimension_three);
          const gt::event::calorimeter_collection_type & the_gamma_calos
            = _gt_.get_event().get_calorimeters();
          spot.grab_auxiliaries().store(snemo::datamodel::particle_track::vertex_type_key(),
                                        the_gamma_calos.at(calo_id).label);
          spot.set_position(the_gamma_calos.at(calo_id).position);
        } // end of gamma hits
      } // end of gammas

      // Reset GT algorithm
      _gt_.reset();

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return 0;
    }

    int gamma_tracking_driver::_post_process(snemo::datamodel::particle_track_data & ptd_)
    {
      if (ptd_.has_non_associated_calorimeters()) {
        DT_LOG_DEBUG(get_logging_priority(), "Removing non-associated calorimeters");
        ptd_.reset_non_associated_calorimeters();
      }

      snemo::datamodel::particle_track_data::particle_collection_type charged_particles;
      ptd_.fetch_particles(charged_particles,
                           snemo::datamodel::particle_track::NEGATIVE |
                           snemo::datamodel::particle_track::POSITIVE |
                           snemo::datamodel::particle_track::UNDEFINED);
      if (charged_particles.empty()) {
        DT_LOG_DEBUG(get_logging_priority(), "No charged particles have been found !");
        return 0;
      }

      for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
             iparticle = charged_particles.begin();
           iparticle != charged_particles.end();
           ++iparticle) {
        const snemo::datamodel::particle_track & a_particle = iparticle->get();

        if (! a_particle.has_associated_calorimeter_hits()) continue;
        const snemo::datamodel::calibrated_calorimeter_hit::collection_type &
          the_calorimeters = a_particle.get_associated_calorimeter_hits();
        // Only take care of the first associated calorimeter
        const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = the_calorimeters.front().get();
        const double particle_time         = a_calo_hit.get_time();
        const double particle_sigma_time   = a_calo_hit.get_sigma_time();
        const double particle_energy       = a_calo_hit.get_energy();
        const double particle_sigma_energy = a_calo_hit.get_sigma_energy();

        // Get track length
        const snemo::datamodel::base_trajectory_pattern & a_track_pattern
          = a_particle.get_trajectory().get_pattern();
        const geomtools::i_shape_1d & a_shape = a_track_pattern.get_shape();
        const double particle_track_length = a_shape.get_length();
        DT_LOG_DEBUG(get_logging_priority(), "Track length = " << particle_track_length / CLHEP::mm << " mm");

        snemo::datamodel::particle_track::vertex_collection_type vertices;
        a_particle.fetch_vertices(vertices, snemo::datamodel::particle_track::VERTEX_ON_SOURCE_FOIL);
        if (vertices.empty()) continue;
        const geomtools::vector_3d & a_foil_vertex = vertices.front().get().get_position();

        // Given charged particle then process gammas
        snemo::datamodel::particle_track_data::particle_collection_type gamma_particles;
        ptd_.fetch_particles(gamma_particles, snemo::datamodel::particle_track::NEUTRAL);
        for (snemo::datamodel::particle_track_data::particle_collection_type::iterator
               igamma = gamma_particles.begin();
             igamma != gamma_particles.end();
             ++igamma) {
          snemo::datamodel::particle_track & a_gamma = igamma->grab();

          // snemo::datamodel::particle_track::vertex_collection_type & vertices = a_gamma.grab_vertices();
          snemo::datamodel::particle_track::vertex_collection_type vertices;
          a_gamma.fetch_vertices(vertices,
                                 snemo::datamodel::particle_track::VERTEX_ON_MAIN_CALORIMETER  |
                                 snemo::datamodel::particle_track::VERTEX_ON_X_CALORIMETER     |
                                 snemo::datamodel::particle_track::VERTEX_ON_GAMMA_VETO);
          if (vertices.empty()) {
            DT_LOG_DEBUG(get_logging_priority(), "Gamma track has no vertices associated !");
            continue;
          }

          for (snemo::datamodel::particle_track::vertex_collection_type::iterator
                 ivtx = vertices.begin();
               ivtx != vertices.end(); ++ivtx) {
            const geomtools::blur_spot & a_spot = ivtx->get();
            // Get associated calorimeter
            if (! a_gamma.has_associated_calorimeter_hits()) {
              DT_LOG_DEBUG(get_logging_priority(), "Gamma track is not associated to any calorimeter block !");
              continue;
            }
            const snemo::datamodel::calibrated_calorimeter_hit::collection_type &
              hits = a_gamma.get_associated_calorimeter_hits();
            geomtools::base_hit::has_geom_id_predicate hit_pred(a_spot.get_geom_id());
            datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                                    snemo::datamodel::calibrated_calorimeter_hit> pred_M2D(hit_pred);
            datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(pred_M2D);
            snemo::datamodel::calibrated_calorimeter_hit::collection_type::const_iterator
              found = std::find_if(hits.begin(), hits.end(), pred_via_handle);
            DT_THROW_IF(found == hits.end(), std::logic_error,
                        "Calibrated calorimeter hit with id " << a_spot.get_geom_id()
                        << " can not be found");
            const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = found->get();
            const double gamma_time         = a_calo_hit.get_time();
            const double gamma_sigma_time   = a_calo_hit.get_sigma_time();
            const double gamma_energy       = a_calo_hit.get_energy();
            // const double gamma_sigma_energy = a_calo_hit.get_sigma_energy();

            // Compute theoritical time for the gamma in case it comes from the
            // foil vertex
            const double gamma_track_length = (a_foil_vertex - a_spot.get_position()).mag();
            const double gamma_time_th = gt::tof_computing::get_theoritical_time(gamma_energy, 0.0, gamma_track_length);

            // Assume particle are electron/positron
            const double mass = CLHEP::electron_mass_c2;
            const double particle_time_th = gt::tof_computing::get_theoritical_time(particle_energy, mass, particle_track_length);
            const double sigma_particle_time_th = particle_time_th * std::pow(mass, 2)
              /(particle_energy*(particle_energy+mass)*(particle_energy+2*mass))*particle_sigma_energy;

            const double dt_int = particle_time - gamma_time - (particle_time_th - gamma_time_th);
            const double sigma = std::pow(particle_sigma_time, 2)
              + std::pow(gamma_sigma_time, 2) + std::pow(sigma_particle_time_th, 2);
            const double chi2_int = std::pow(dt_int, 2)/sigma;
            const double int_prob = gt::tof_computing::get_internal_probability(chi2_int);
            const double int_prob_limit = 0.04;
            if (int_prob > int_prob_limit) {
              DT_LOG_DEBUG(get_logging_priority(),
                           "Adding foil vertex (with internal probability = " << int_prob << ")");

              snemo::datamodel::particle_track::handle_spot hBSv(new geomtools::blur_spot);
              // a_gamma.grab_vertices().push_back(hBSv);
              a_gamma.grab_vertices().insert(a_gamma.grab_vertices().begin(),hBSv);
              geomtools::blur_spot & spot_v = hBSv.grab();
              spot_v.set_hit_id(0);
              spot_v.grab_auxiliaries().store(snemo::datamodel::particle_track::vertex_type_key(),
                                              snemo::datamodel::particle_track::vertex_on_source_foil_label());
              spot_v.set_blur_dimension(geomtools::blur_spot::dimension_three);
              spot_v.set_position(a_foil_vertex);
              break;
            }
          }
        }

        // Only take care of one particle (to be improved later)
        break;
      }
      return 0;
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo
/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::gamma_tracking_driver, ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::gamma_tracking_driver");
  ocd_.set_class_description("A driver class for the Gamma Tracking algorithm");
  ocd_.set_class_library("Falaise_GammaTracking");
  ocd_.set_class_documentation("The driver manager for the Gamma Tracking algorithms\n"
                               "/todo What does the manager do ?"
                               );


  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::gamma_tracking_driver,
                               "snemo::reconstruction::gamma_tracking_driver")
