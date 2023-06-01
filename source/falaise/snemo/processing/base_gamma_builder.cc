/// \file falaise/snemo/processing/base_gamma_builder.cc

// Ourselves:
#include "falaise/snemo/processing/base_gamma_builder.h"

// Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/property_set.h>
#include <falaise/quantity.h>
#include <falaise/snemo/datamodels/vertex_utils.h>
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace snemo {

  namespace processing {

    // Constructor
    base_gamma_builder::base_gamma_builder(const std::string & name)
    {
      id_ = name;
      _set_initialized(false);
      _set_defaults();
    }

    base_gamma_builder::~base_gamma_builder() = default;

    void base_gamma_builder::_set_defaults()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;
      geoManager_ = nullptr;
      geoLocator_ = nullptr;
      extrapolateFoilVertex_ = true;
      minFoilVertexProbability_ = 1.0 * CLHEP::perCent;
      tagAnnihilationGamma_ = false;
      minAnnihilationGammaProbability_ = 1.0 * CLHEP::perCent;
      selectCaloHits_ = false;
      caloHitTags_.clear();
    }

    void base_gamma_builder::_reset()
    {
      _set_initialized(false);
      this->base_gamma_builder::_set_defaults();
      this->base_gamma_builder::_clear_working_arrays();
    }

    datatools::logger::priority base_gamma_builder::get_logging_priority() const
    {
      return _logging_priority;
    }

    void base_gamma_builder::set_logging_priority(datatools::logger::priority priority_)
    {
      DT_THROW_IF(priority_ == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority level for base gamma builder !");
      _logging_priority = priority_;
    }

    const std::string& base_gamma_builder::get_id() const { return id_; }

    const snemo::geometry::calo_locator & base_gamma_builder::get_calo_locator() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Driver '" << get_id() << "' is not initialized !");
      return geoLocator_->caloLocator();
    }

    const snemo::geometry::xcalo_locator & base_gamma_builder::get_xcalo_locator() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Driver '" << get_id() << "' is not initialized !");
      return geoLocator_->xcaloLocator();
    }

    const snemo::geometry::gveto_locator & base_gamma_builder::get_gveto_locator() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Driver '" << get_id() << "' is not initialized !");
      return geoLocator_->gvetoLocator();
    }

    bool base_gamma_builder::is_initialized() const
    {
      return isInitialized_;
    }

    void base_gamma_builder::_set_initialized(bool i_)
    {
      isInitialized_ = i_;
    }

    void base_gamma_builder::_initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Driver '" << get_id() << "' is already initialized !");
      DT_THROW_IF(!has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(!get_geometry_manager().is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      // Extract the setup of the base gamma builder :
      falaise::property_set localSetup{setup_};
      auto ps = localSetup.get<falaise::property_set>("BGB", {});

      // Logging priority
      auto lp = datatools::logger::get_priority(ps.get<std::string>("logging.priority", "warning"));
      set_logging_priority(lp);

      // Get geometry locator plugin
      auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
      const geomtools::manager& geo_mgr = get_geometry_manager();
      geoLocator_ = snemo::geometry::getSNemoLocator(geo_mgr, locator_plugin_name);

      // Select calorimeter hits based on associated tags
      // TODO: boolean flag is redundant, as use indicated by non-empty tags
      selectCaloHits_ = ps.get<bool>("select_calorimeter_hits", false);
      if (selectCaloHits_) {
        caloHitTags_ = ps.get<std::vector<std::string>>("select_calorimeter_hits.tags");
      }

      // Extrapolation on the source foil given charged particle
      extrapolateFoilVertex_ = ps.get<bool>("add_foil_vertex_extrapolation", true);
      if (extrapolateFoilVertex_) {
        minFoilVertexProbability_ = ps.get<falaise::fraction_t>(
                                                                "add_foil_vertex_extrapolation.minimal_probability", {1.0, "percent"})();
      }

      // Search for gamma from e+/e- annihilation
      tagAnnihilationGamma_ = ps.get<bool>("add_gamma_from_annihilation", false);
      if (tagAnnihilationGamma_) {
        minAnnihilationGammaProbability_ = ps.get<falaise::fraction_t>(
                                                                       "add_gamma_from_annihilation.minimal_probability", {1.0, "percent"})();
      }
    }

    void base_gamma_builder::_clear_working_arrays()
    {
      ignoredHits_.clear();
      usedHits_.clear();
    }

    void base_gamma_builder::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized/locked !");
      geoManager_ = &gmgr_;
    }

    const geomtools::manager & base_gamma_builder::get_geometry_manager() const
    {
      DT_THROW_IF(!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *geoManager_;
    }

    bool base_gamma_builder::has_geometry_manager() const
    {
      return geoManager_ != nullptr;
    }

    int base_gamma_builder::process(const base_gamma_builder::hit_collection_type & calo_hits_,
                                    snemo::datamodel::particle_track_data & ptd_)
    {
      int status = 0;
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Gamma builder '" << id_ << "' is not initialized !");

      status = _prepare_process(calo_hits_, ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(), "Pre-processing '" << get_id() << "' has failed !");
        return status;
      }

      status = _process_algo(usedHits_, ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of '" << get_id() << "' algorithm has failed !");
        return status;
      }

      status = _post_process(calo_hits_, ptd_);
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Post-processing of '" << get_id() << "' algorithm has failed !");
        return status;
      }
      return status;
    }

    int base_gamma_builder::_prepare_process(const base_gamma_builder::hit_collection_type & calo_hits_,
                                             snemo::datamodel::particle_track_data & /*ptd_*/)
    {
      this->base_gamma_builder::_clear_working_arrays();
      usedHits_.reserve(calo_hits_.size());
      ignoredHits_.reserve(calo_hits_.size());

      for (const auto& a_calo_hit : calo_hits_) {
        bool use_hit = false;
        if (selectCaloHits_) {
          const datatools::properties& the_auxiliaries = a_calo_hit->get_auxiliaries();
          for (const auto& tag : caloHitTags_) {
            if (the_auxiliaries.has_flag(tag)) {
              use_hit = true;
              break;
            }
          }
        } else {
          use_hit = true;
        }

        if (use_hit) {
          usedHits_.push_back(a_calo_hit);
        } else {
          ignoredHits_.push_back(a_calo_hit);
        }
      }
      DT_LOG_DEBUG(get_logging_priority(),
                   "Number of calorimeter hits used: " << usedHits_.size() << " ("
                   << ignoredHits_.size() << " skipped)");
      return 0;
    }

    int base_gamma_builder::_post_process(const base_gamma_builder::hit_collection_type & /*calo_hits_*/,
                                          snemo::datamodel::particle_track_data & ptd_)
    {
      // Add the ignored hits to the list of non associated calorimeters
      // ptd_.reset_non_associated_calorimeters();
      auto& calos = ptd_.isolatedCalorimeters();
      calos.assign(ignoredHits_.begin(), ignoredHits_.end());

      // Given charged particle then process gammas
      snemo::datamodel::ParticleHdlCollection gamma_particles =
        get_particles_by_charge(ptd_, snemo::datamodel::particle_track::CHARGE_NEUTRAL);
      if (gamma_particles.empty()) {
        return 0;
      }

      snemo::datamodel::ParticleHdlCollection charged_particles
        = get_particles_by_charge(ptd_,
                                  snemo::datamodel::particle_track::CHARGE_NEGATIVE |
                                  snemo::datamodel::particle_track::CHARGE_POSITIVE |
                                  snemo::datamodel::particle_track::CHARGE_UNDEFINED);
      if (charged_particles.empty()) {
        return 0;
      }

      for (const auto& a_particle : charged_particles) {
        // No calorimeter associated, no TOF computation
        if (!a_particle->has_associated_calorimeter_hits()) {
          continue;
        }

        for (auto& a_gamma : gamma_particles) {
          snemo::datamodel::VertexHdlCollection the_vertices_2;
          for (const auto & vtx : a_gamma->get_vertices()) {
            if (vtx->match_categories(snemo::datamodel::VERTEX_CATEGORY_ON_MAIN_CALORIMETER |
                                      snemo::datamodel::VERTEX_CATEGORY_ON_X_CALORIMETER |
                                      snemo::datamodel::VERTEX_CATEGORY_ON_GAMMA_VETO)) {
              the_vertices_2.push_back(vtx);
            }
          }

          if (the_vertices_2.empty()) {
            continue;
          }

          for (const auto & a_vertex : the_vertices_2) {
            // Get associated calorimeter:
            if (!a_gamma->has_associated_calorimeter_hits()) {
              DT_LOG_DEBUG(get_logging_priority(),
                           "Gamma track is not associated to any calorimeter block !");
              continue;
            }
            const auto& hits = a_gamma->get_associated_calorimeter_hits();
            geomtools::base_hit::has_geom_id_predicate hit_pred(a_vertex->get_geom_id());
            datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                                    snemo::datamodel::calibrated_calorimeter_hit>
              pred_M2D(hit_pred);
            datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(
                                                                                                      pred_M2D);
            auto found = std::find_if(hits.begin(), hits.end(), pred_via_handle);
            DT_THROW_IF(
                        found == hits.end(), std::logic_error,
                        "Calibrated calorimeter hit with id " << a_vertex->get_geom_id() << " can not be found");
            const snemo::datamodel::calibrated_calorimeter_hit& a_calo_hit_2 = found->get();
            const double gamma_time = a_calo_hit_2.get_time();
            const double gamma_sigma_time = a_calo_hit_2.get_sigma_time();
            // const double gamma_energy       = a_calo_hit_2.get_energy();
            // const double gamma_sigma_energy = a_calo_hit_2.get_sigma_energy();

            // Perform extrapolation to the source foil given charged particles
            // and favor it wrt the search of gamma from e+/e- annihilation
            if (extrapolateFoilVertex_) {
              // Get track length
              const snemo::datamodel::base_trajectory_pattern& a_track_pattern =
                a_particle->get_trajectory().get_pattern();
              const geomtools::i_shape_1d& a_shape = a_track_pattern.get_shape();
              const double particle_track_length = a_shape.get_length();

              snemo::datamodel::VertexHdlCollection vertices;
              for(const auto & vtx : a_particle->get_vertices()) {
                if(vtx->match_categories(snemo::datamodel::VERTEX_CATEGORY_ON_SOURCE_FOIL)) {
                  vertices.push_back(vtx);
                }
              }

              if (vertices.empty()) {
                continue;
              }
              const geomtools::vector_3d & a_foil_vertex = (vertices.front())->get_spot().get_position();

              // Compute theoritical time for the gamma in case it comes from the foil vertex
              const double gamma_track_length = (a_foil_vertex - a_vertex->get_spot().get_position()).mag();
              const double gamma_time_th = gamma_track_length / CLHEP::c_light;

              // Assume particle are electron/positron
              const snemo::datamodel::CalorimeterHitHdlCollection& the_calorimeters =
                a_particle->get_associated_calorimeter_hits();
              // Only take care of the first associated calorimeter
              const auto& a_calo_hit = the_calorimeters.front();
              const double particle_time = a_calo_hit->get_time();
              const double particle_sigma_time = a_calo_hit->get_sigma_time();
              const double particle_energy = a_calo_hit->get_energy();
              const double particle_sigma_energy = a_calo_hit->get_sigma_energy();
              const double particle_mass = CLHEP::electron_mass_c2;
              const double beta = std::sqrt(particle_energy * (particle_energy + 2. * particle_mass)) /
                (particle_energy + particle_mass);
              const double particle_time_th = particle_track_length / CLHEP::c_light / beta;
              const double sigma_particle_time_th =
                particle_time_th * std::pow(particle_mass, 2) /
                (particle_energy * (particle_energy + particle_mass) *
                 (particle_energy + 2 * particle_mass)) *
                particle_sigma_energy;

              const double dt_int = particle_time - gamma_time - (particle_time_th - gamma_time_th);
              const double sigma = std::pow(particle_sigma_time, 2) + std::pow(gamma_sigma_time, 2) +
                std::pow(sigma_particle_time_th, 2);
              const double chi2_int = std::pow(dt_int, 2) / sigma;
              const double int_prob = gsl_cdf_chisq_Q(chi2_int, 1) * 100. * CLHEP::perCent;
              const double int_prob_limit = minFoilVertexProbability_;
              if (int_prob > int_prob_limit) {
                snemo::datamodel::VertexHdl hBSv = datatools::make_handle<snemo::datamodel::Vertex>();
                a_gamma->get_vertices().insert(a_gamma->get_vertices().begin(), hBSv);
                hBSv->set_hit_id(0);
                hBSv->get_spot().set_blur_dimension(geomtools::blur_spot::dimension_three);
                hBSv->get_spot().set_position(a_foil_vertex);
                hBSv->set_category(snemo::datamodel::VERTEX_CATEGORY_ON_SOURCE_FOIL);
                break;
              }
            }
            // Perform the search for gamma from e+/e- annihilation
            if (tagAnnihilationGamma_) {
              // Get calorimeter hit position associated to charged particle
              const auto& the_calorimeters = a_particle->get_associated_calorimeter_hits();
              // Only take care of the first associated calorimeter
              const auto& a_calo_hit = the_calorimeters.front();
              const double particle_time = a_calo_hit->get_time();
              const double particle_sigma_time = a_calo_hit->get_sigma_time();
              // Get block position and label
              geomtools::vector_3d a_block_position;
              std::string a_label;
              snemo::datamodel::vertex_category_type vertexCategory = snemo::datamodel::VERTEX_CATEGORY_UNDEF;
              const geomtools::geom_id & a_gid = a_calo_hit->get_geom_id();

              if (get_calo_locator().isCaloBlockInThisModule(a_gid)) {
                a_block_position = get_calo_locator().getBlockPosition(a_gid);
                vertexCategory = snemo::datamodel::VERTEX_CATEGORY_ON_MAIN_CALORIMETER;
              } else if (get_xcalo_locator().isCaloBlockInThisModule(a_gid)) {
                a_block_position = get_xcalo_locator().getBlockPosition(a_gid);
                vertexCategory = snemo::datamodel::VERTEX_CATEGORY_ON_X_CALORIMETER;
              } else if (get_gveto_locator().isCaloBlockInThisModule(a_gid)) {
                a_block_position = get_gveto_locator().getBlockPosition(a_gid);
                vertexCategory = snemo::datamodel::VERTEX_CATEGORY_ON_GAMMA_VETO;
              } else {
                DT_THROW(std::logic_error,
                         "Current geom id '" << a_gid << "' does not match any scintillator block !");
              }

              const double track_length = (a_block_position - a_vertex->get_spot().get_position()).mag();
              const double dt_exp = std::abs(particle_time - gamma_time);
              const double dt_th = track_length / CLHEP::c_light;
              const double dt_int = dt_exp - dt_th;
              const double sigma = std::pow(particle_sigma_time, 2) + std::pow(gamma_sigma_time, 2);
              const double chi2_int = std::pow(dt_int, 2) / sigma;
              const double int_prob = gsl_cdf_chisq_Q(chi2_int, 1) * 100. * CLHEP::perCent;
              const double int_prob_limit = minAnnihilationGammaProbability_;
              if (int_prob > int_prob_limit) {
                // Do not add the calorimeter hit from the charged particle to
                // the list of calorimeter hits of the gamma particle
                // snemo::datamodel::CalorimeterHitHdlCollection & hits =
                // a_gamma.grab_associated_calorimeter_hits(); hits.insert(hits.begin(),
                // the_calorimeters.front());
                snemo::datamodel::VertexHdl hBSv = datatools::make_handle<snemo::datamodel::Vertex>();
                a_gamma->get_vertices().insert(a_gamma->get_vertices().begin(), hBSv);
                hBSv->set_hit_id(a_calo_hit->get_hit_id());
                hBSv->set_geom_id(a_calo_hit->get_geom_id());
                hBSv->get_spot().set_blur_dimension(geomtools::blur_spot::dimension_three);
                hBSv->get_spot().set_position(a_block_position);
                hBSv->set_category(vertexCategory);
                a_gamma->grab_auxiliaries().update_flag("__gamma_from_annihilation");
                break;
              }
            }
          }
        }

        // Only take care of one particle (to be improved later)
        break;
      }

      return 0;
    }

    void base_gamma_builder::tree_dump(std::ostream & out,
                                       const std::string & title,
                                       const std::string & indent,
                                       bool inherit) const
    {
      if (!title.empty()) {
        out << indent << title << std::endl;
      }

      out << indent << datatools::i_tree_dumpable::tag << "Logging          : '"
          << datatools::logger::get_priority_label(_logging_priority) << "'" << std::endl;
      out << indent << datatools::i_tree_dumpable::tag << "Initialized      : " << is_initialized()
          << std::endl;
      out << indent << datatools::i_tree_dumpable::tag << "Geometry manager : " << geoManager_
          << std::endl;
      if (has_geometry_manager()) {
        out << indent << datatools::i_tree_dumpable::tag << "Geometry setup label   : '"
            << geoManager_->get_setup_label() << "'" << std::endl;
        out << indent << datatools::i_tree_dumpable::tag << "Geometry setup version : '"
            << geoManager_->get_setup_version() << "'" << std::endl;
      }

      out << indent << datatools::i_tree_dumpable::tag
          << "Foil vertex extrapolation : " << extrapolateFoilVertex_ << std::endl;
      if (extrapolateFoilVertex_) {
        out << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::last_tag
            << "Minimal TOF probability : " << minFoilVertexProbability_ / CLHEP::perCent << "%"
            << std::endl;
      }
      out << indent << datatools::i_tree_dumpable::tag
          << "Search for gamma from e+/e- annihilation : " << tagAnnihilationGamma_ << std::endl;
      if (tagAnnihilationGamma_) {
        out << indent << datatools::i_tree_dumpable::skip_tag << datatools::i_tree_dumpable::last_tag
            << "Minimal TOF probability : " << minAnnihilationGammaProbability_ / CLHEP::perCent << "%"
            << std::endl;
      }
      out << indent << datatools::i_tree_dumpable::tag
          << "Selection of calorimeter hits : " << selectCaloHits_ << std::endl;
      if (selectCaloHits_) {
        for (size_t i = 0; i < caloHitTags_.size(); i++) {
          out << indent << datatools::i_tree_dumpable::skip_tag;
          if (i + 1 == caloHitTags_.size()) {
            out << datatools::i_tree_dumpable::last_tag;
          } else {
            out << datatools::i_tree_dumpable::tag;
          }
          out << "tag[" << i << "] = " << caloHitTags_[i] << std::endl;
        }
      }
      out << indent << datatools::i_tree_dumpable::inherit_tag(inherit) << "End." << std::endl;
    }

    // static
    void base_gamma_builder::ocd_support(datatools::object_configuration_description & ocd_,
                                         const std::string & prefix_)
    {
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_ + "BGB.");

      {
        // Description of the 'locator_plugin_name' configuration property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.locator_plugin_name")
          .set_terse_description("The name of the geometry locator plugin to be used")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_STRING)
          .set_long_description("Empty value means automatic search   \n")
          .set_mandatory(false)
          .add_example(
                       "Set a specific value::                                   \n"
                       "                                                         \n"
                       "  BGB.locator_plugin_name : string = \"locators_driver\" \n"
                       "                                                         \n");
      }

      {
        // Description of the 'BGB.add_foil_vertex_extrapolation' configuration property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.add_foil_vertex_extrapolation")
          .set_terse_description("Allow the extrapolation of gamma tracks to the source foil")
          .set_long_description(
                                "Given the presence of charged particles with vertex on the source foil          \n"
                                "a Time-Of-Flight computation is done to check if gamma are internals            \n"
                                "with the charged particle. Then, the charged particle vertex on the source foil \n"
                                "is also associated to the gamma.                                                \n")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(true)
          .set_mandatory(false)
          .add_example(
                       "Set the default value::                              \n"
                       "                                                     \n"
                       "  BGB.add_foil_vertex_extrapolation : boolean = true \n"
                       "                                                     \n");
      }

      {
        // Description of the 'BGB.add_foil_vertex.minimal_probability' configuration property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.add_foil_vertex_extrapolation.minimal_probability")
          .set_terse_description("Set the minimal internal TOF probability")
          .set_from("snemo::processing::base_gamma_builder")
          .set_triggered_by_flag("BGB.add_foil_vertex_extrapolation")
          .set_traits(datatools::TYPE_REAL)
          .set_default_value_real(1 * CLHEP::perCent, "%")
          .set_mandatory(false)
          .add_example(
                       "Set the default value::                                           \n"
                       "                                                                  \n"
                       "  BGB.add_foil_vertex_extrapolation.minimal_probability : real as fraction = 1% \n"
                       "                                                                  \n");
      }

      {
        // Description of the 'BGB.add_gamma_from_annihilation' configuration property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.add_gamma_from_annihilation")
          .set_terse_description("Allow the search and the tagging of gamma from e+/e- annihilation")
          .set_long_description(
                                "Given the presence of charged particles, a Time-Of-Flight computation       \n"
                                "is done to check if gamma comes from an e+/e- annihilation.                 \n"
                                "The TOF calculation is performed on calorimeter hit of the charged particle \n"
                                "and the first calorimeter hits associated to gamma.                         \n")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(true)
          .set_mandatory(false)
          .add_example(
                       "Set the default value::                            \n"
                       "                                                   \n"
                       "  BGB.add_gamma_from_annihilation : boolean = true \n"
                       "                                                   \n");
      }

      {
        // Description of the 'BGB.add_gamma_from_annihilation.minimal_probability' configuration
        // property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.add_gamma_from_annihilation.minimal_probability")
          .set_terse_description("Set the minimal TOF probability for gamma from e+/e- annihilation")
          .set_from("snemo::processing::base_gamma_builder")
          .set_triggered_by_flag("BGB.add_gamma_from_annihilation")
          .set_traits(datatools::TYPE_REAL)
          .set_default_value_real(1 * CLHEP::perCent, "%")
          .set_mandatory(false)
          .add_example(
                       "Set the default value::                                                       \n"
                       "                                                                              \n"
                       "  BGB.add_gamma_from_annihilation.minimal_probability : real as fraction = 1% \n"
                       "                                                                              \n");
      }

      {
        // Description of the 'BGB.select_calorimeter_hits' configuration property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.select_calorimeter_hits")
          .set_terse_description("Allow the pre-selection of calorimeter hits")
          .set_long_description(
                                "The calorimeter hits can be pre-selected before sending them  \n"
                                "to the gamma tracking/clustering algorithm based on a list of \n"
                                "tags given by the used                                        \n")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(false)
          .set_mandatory(false)
          .add_example(
                       "Activate the selection::                       \n"
                       "                                               \n"
                       "  BGB.select_calorimeter_hits : boolean = true \n"
                       "                                               \n");
      }

      {
        // Description of the 'BGB.select_calorimeter_hits.tags' configuration property :
        datatools::configuration_property_description& cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BGB.select_calorimeter_hits.tags")
          .set_terse_description("Set the list of tags for calorimeter selection")
          .set_triggered_by_flag("BGB.select_calorimeter_hits")
          .set_from("snemo::processing::base_gamma_builder")
          .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
          .set_mandatory(false)
          .add_example(
                       "Set values::                                                    \n"
                       "                                                                \n"
                       "  BGB.select_calorimeter_hits.tags : string[1] = \"__isolated\" \n"
                       "                                                                \n");
      }
    }

  } // end of namespace processing

} // end of namespace snemo
