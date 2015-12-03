/// \file falaise/snemo/processing/base_gamma_builder.cc

// Ourselves:
#include <snemo/processing/base_gamma_builder.h>

// Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace processing {

    datatools::logger::priority base_gamma_builder::get_logging_priority() const
    {
      return _logging_priority;
    }

    void base_gamma_builder::set_logging_priority(datatools::logger::priority priority_)
    {
      _logging_priority = priority_;
      return;
    }

    const std::string & base_gamma_builder::get_id() const
    {
      return _id_;
    }

    const snemo::geometry::calo_locator & base_gamma_builder::get_calo_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << get_id() << "' is not initialized !");
      return _locator_plugin_->get_calo_locator();
    }

    const snemo::geometry::xcalo_locator & base_gamma_builder::get_xcalo_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << get_id() << "' is not initialized !");
      return _locator_plugin_->get_xcalo_locator();
    }

    const snemo::geometry::gveto_locator & base_gamma_builder::get_gveto_locator() const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Driver '" << get_id() << "' is not initialized !");
      return _locator_plugin_->get_gveto_locator();
    }

    bool base_gamma_builder::is_initialized() const
    {
      return _initialized_;
    }

    void base_gamma_builder::_set_initialized(bool i_)
    {
      _initialized_ = i_;
      return;
    }

    void base_gamma_builder::_initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Driver '" << get_id() << "' is already initialized !");

      DT_THROW_IF(! has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(! get_geometry_manager().is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      // Extract the setup of the base gamma builder :
      datatools::properties bgb_setup;
      setup_.export_and_rename_starting_with(bgb_setup, "BGB.", "");

      //Logging priority
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration(bgb_setup);
      DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority level for base gamma builder !");
      set_logging_priority(lp);

      // Get geometry locator plugin
      const geomtools::manager & geo_mgr = get_geometry_manager();
      std::string locator_plugin_name;
      if (bgb_setup.has_key("locator_plugin_name")) {
        locator_plugin_name = bgb_setup.fetch_string("locator_plugin_name");
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

      // Extrapolation on the source foil given charged particle
      if (bgb_setup.has_key("add_foil_vertex_extrapolation")) {
        _add_foil_vertex_extrapolation_ = bgb_setup.fetch_boolean("add_foil_vertex_extrapolation");
      }

      if (_add_foil_vertex_extrapolation_) {
        if (bgb_setup.has_key("add_foil_vertex_minimal_probability")) {
          _add_foil_vertex_minimal_probability_
            = bgb_setup.fetch_real("add_foil_vertex_minimal_probability");
          if (! bgb_setup.has_explicit_unit("add_foil_vertex_minimal_probability")) {
            _add_foil_vertex_minimal_probability_ *= CLHEP::perCent;
          }
        }
      }
      return;
    }

    void base_gamma_builder::_reset()
    {
      _set_initialized(false);
      this->base_gamma_builder::_set_defaults();
      return;
    }

    void base_gamma_builder::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized/locked !");
      _geometry_manager_ = &gmgr_;
      return;
    }

    const geomtools::manager & base_gamma_builder::get_geometry_manager() const
    {
      DT_THROW_IF (! has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *_geometry_manager_;
    }


    bool base_gamma_builder::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    void base_gamma_builder::_set_defaults()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;
      _geometry_manager_ = 0;
      _locator_plugin_ = 0;
      _add_foil_vertex_extrapolation_ = true;
      _add_foil_vertex_minimal_probability_ = 1.0 * CLHEP::perCent;
      return;
    }

      // Constructor
    base_gamma_builder::base_gamma_builder(const std::string & id_)
    {
      _id_ = id_;
      _set_initialized(false);
      _set_defaults();
      return;
    }

    base_gamma_builder::~base_gamma_builder()
    {
      if (is_initialized()) {
        _reset();
      }
      return;
    }

    void base_gamma_builder::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) {
        indent = indent_;
      }
      if ( ! title_.empty() ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logging          : '"
           << datatools::logger::get_priority_label(_logging_priority) << "'"
           << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Initialized      : " << is_initialized() << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Geometry manager : " << _geometry_manager_ << std::endl;
      if (has_geometry_manager()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Geometry setup label   : '" << _geometry_manager_->get_setup_label() << "'" << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Geometry setup version : '" << _geometry_manager_->get_setup_version() << "'" << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Foil vertex extrapolation : " << _add_foil_vertex_extrapolation_ << std::endl;
      if (_add_foil_vertex_extrapolation_) {
        out_ << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::last_tag
             << "Minimal TOF probability : " << _add_foil_vertex_minimal_probability_/CLHEP::perCent << "%" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "End." << std::endl;
      return;
    }


    int base_gamma_builder::process(snemo::datamodel::particle_track_data & ptd_)
    {
      int status = 0;
      DT_THROW_IF (! is_initialized(), std::logic_error, "Gamma builder '" << _id_ << "' is not initialized !");

      status = _prepare_process(ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Pre-processing '" << get_id() << "' has failed !");
        return status;
      }

      status = _process_algo(ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of '" << get_id() << "' algorithm has failed !");
        return status;
      }

      status = _post_process(ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Post-processing of '" << get_id() << "' algorithm has failed !");
        return status;
      }
      return status;
    }

    int base_gamma_builder::_prepare_process(snemo::datamodel::particle_track_data & /*ptd_*/)
    {
      return 0;
    }

    int base_gamma_builder::_post_process(snemo::datamodel::particle_track_data & ptd_)
    {
      if (ptd_.has_non_associated_calorimeters()) {
        // 2015/12/04 XG: Remove only isolated calorimeter hit (the one without
        // any tracker cell in front of)
        // ptd_.reset_non_associated_calorimeters();
        snemo::datamodel::calibrated_calorimeter_hit::collection_type & chits
          = ptd_.grab_non_associated_calorimeters();
        for (snemo::datamodel::calibrated_calorimeter_hit::collection_type::iterator
               ihit = chits.begin(); ihit != chits.end();/*++ihit*/) {
          const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = ihit->get();
          if (a_calo_hit.get_auxiliaries().has_flag("__isolated")) {
            ihit = chits.erase(ihit);
          } else {
            ++ihit;
          }
        }
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

      // Perform extrapolation to the source foil given charged particles
      if (! _add_foil_vertex_extrapolation_) return 0;

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
            // const double gamma_energy       = a_calo_hit.get_energy();
            // const double gamma_sigma_energy = a_calo_hit.get_sigma_energy();

            // Compute theoritical time for the gamma in case it comes from the
            // foil vertex
            const double gamma_track_length = (a_foil_vertex - a_spot.get_position()).mag();
            const double gamma_time_th = gamma_track_length / CLHEP::c_light;

            // Assume particle are electron/positron
            const double particle_mass = CLHEP::electron_mass_c2;
            const double beta = std::sqrt(particle_energy*(particle_energy + 2.*particle_mass))
              / (particle_energy+particle_mass);
            const double particle_time_th = particle_track_length / CLHEP::c_light / beta;
            const double sigma_particle_time_th = particle_time_th * std::pow(particle_mass, 2)
              /(particle_energy*(particle_energy+particle_mass)*(particle_energy+2*particle_mass))*particle_sigma_energy;

            const double dt_int = particle_time - gamma_time - (particle_time_th - gamma_time_th);
            const double sigma = std::pow(particle_sigma_time, 2)
              + std::pow(gamma_sigma_time, 2) + std::pow(sigma_particle_time_th, 2);
            const double chi2_int = std::pow(dt_int, 2)/sigma;
            const double int_prob = gsl_cdf_chisq_Q(chi2_int, 1)*100.*CLHEP::perCent;
            const double int_prob_limit = _add_foil_vertex_minimal_probability_;
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

    // static
    void base_gamma_builder::ocd_support(datatools::object_configuration_description & ocd_,
                                          const std::string & prefix_)
    {
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_ + "BGB.");

      {
        // Description of the 'locator_plugin_name' configuration property :
        datatools::configuration_property_description & cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.locator_plugin_name")
          .set_terse_description("The name of the geometry locator plugin to be used")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_STRING)
          .set_long_description("Empty value means automatic search   \n")
          .set_mandatory(false)
          .add_example("Set a specific value::                                   \n"
                       "                                                         \n"
                       "  BGB.locator_plugin_name : string = \"locators_driver\" \n"
                       "                                                         \n"
                       )
          ;
      }

      {
        // Description of the 'BGB.add_foil_vertex_extrapolation' configuration property :
        datatools::configuration_property_description & cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.add_foil_vertex_extrapolation")
          .set_terse_description("Allow the extrapolation of gamma tracks to the source foil")
          .set_long_description("Given the presence of charged particles with vertex on the source foil          \n"
                                "a Time-Of-Flight computation is done to check if gamma are internals            \n"
                                "with the charged particle. Then, the charged particle vertex on the source foil \n"
                                "is also associated to the gamma.                                                \n"
                                )
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(true)
          .set_mandatory(false)
          .add_example("Set the default value::                           \n"
                       "                                                  \n"
                       "  BGB.add_foil_vertex_extrapolation : boolean = 1 \n"
                       "                                                  \n"
                       )
          ;
      }

      {
        // Description of the 'BGB.add_foil_vertex_minimal_probability' configuration property :
        datatools::configuration_property_description & cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.add_foil_vertex_minimal_probability")
          .set_terse_description("Set the minimal internal TOF probability")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_REAL)
          .set_default_value_real(1 * CLHEP::perCent, "%")
          .set_mandatory(false)
          .add_example("Set the default value::                                           \n"
                       "                                                                  \n"
                       "  BGB.add_foil_vertex_minimal_probability : real as fraction = 1% \n"
                       "                                                                  \n"
                       )
          ;
      }

      return;
    }

  }  // end of namespace processing

}  // end of namespace snemo
