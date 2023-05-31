// \file falaise/snemo/reconstruction/charged_particle_tracking_module.cc

// Ourselves:
#include <ChargedParticleTracking/charged_particle_tracking_module.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/service_manager.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/manager.h>

// This project (Falaise):
#include <falaise/property_set.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/services/services.h>

// This plugin (ChargedParticleTracking):
#include <ChargedParticleTracking/alpha_finder_driver.h>
#include <ChargedParticleTracking/calorimeter_association_driver.h>
#include <ChargedParticleTracking/charge_computation_driver.h>
#include <ChargedParticleTracking/vertex_extrapolation_driver.h>

namespace snemo {

  namespace reconstruction {

    // Registration instantiation macro
    DPP_MODULE_REGISTRATION_IMPLEMENT(charged_particle_tracking_module,
                                      "snemo::reconstruction::charged_particle_tracking_module")

    void charged_particle_tracking_module::_set_defaults()
    {
      CDTag_ = snedm::labels::calibrated_data();
      TTDTag_ = snedm::labels::tracker_trajectory_data();
      PTDTag_ = snedm::labels::particle_track_data();

      geoManager_ = snemo::service_handle<snemo::geometry_svc>{};

      VEAlgo_.reset();
      CCAlgo_.reset();
      CAAlgo_.reset();
      AFAlgo_.reset();
    }

    void charged_particle_tracking_module::initialize(const datatools::properties & setup_,
                                                      datatools::service_manager & service_manager_,
                                                      dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      namespace snreco = snemo::reconstruction;

      using VertexExtrapolator = snreco::vertex_extrapolation_driver;
      using ChargeCalculator = snreco::charge_computation_driver;
      using TrackToCaloMatcher = snreco::calorimeter_association_driver;
      using AlphaFinder = snreco::alpha_finder_driver;

      dpp::base_module::_common_initialize(setup_);
   
      falaise::property_set ps{setup_};

      CDTag_  = ps.get<std::string>("CD_label", snedm::labels::calibrated_data());
      TTDTag_ = ps.get<std::string>("TTD_label", snedm::labels::tracker_trajectory_data());
      PTDTag_ = ps.get<std::string>("PTD_label", snedm::labels::particle_track_data());
      DT_LOG_DEBUG(get_logging_priority(), "CDTag  = '" << CDTag_ << "'");
      DT_LOG_DEBUG(get_logging_priority(), "TTDTag = '" << TTDTag_ << "'");
      DT_LOG_DEBUG(get_logging_priority(), "PTDTag = '" << PTDTag_ << "'");

      // Geometry manager :
      geoManager_ = snemo::service_handle<snemo::geometry_svc>{service_manager_};

      auto driver_names = ps.get<std::vector<std::string>>("drivers", {
          VertexExtrapolator::get_id(),
          ChargeCalculator::get_id(),
          TrackToCaloMatcher::get_id(),
          AlphaFinder::get_id(),
        });
      for (const std::string & id : driver_names) {
        auto dps = ps.get<falaise::property_set>(id, {});
        if (datatools::logger::is_debug(get_logging_priority())) {
          DT_LOG_DEBUG(get_logging_priority(), "Configuration for driver '" << id << "'");
          std::cerr << dps.to_string() << std::endl;
        }
        if (id == VertexExtrapolator::get_id()) {
          DT_LOG_DEBUG(get_logging_priority(), "Instantiating VE algo '" << id << "'");
          VEAlgo_ = std::make_unique<VertexExtrapolator>(dps, geoManager_.operator->());
        } else if (id == ChargeCalculator::get_id()) {
          DT_LOG_DEBUG(get_logging_priority(), "Instantiating CC algo '" << id << "'");
          CCAlgo_ = std::make_unique<ChargeCalculator>(dps);
        } else if (id == TrackToCaloMatcher::get_id()) {
          DT_LOG_DEBUG(get_logging_priority(), "Instantiating CA algo '" << id << "'");
          CAAlgo_ = std::make_unique<TrackToCaloMatcher>(dps, geoManager_.operator->());
        } else if (id == AlphaFinder::get_id()) {
          DT_LOG_DEBUG(get_logging_priority(), "Instantiating AF algo '" << id << "'");
          AFAlgo_ = std::make_unique<AlphaFinder>(dps, geoManager_.operator->());
        } else {
          DT_THROW_IF(true, std::logic_error, "Driver '" << id << "' does not exist !");
        }
      }
      // Tag the module as initialized :
      _set_initialized(true);
    }

    void charged_particle_tracking_module::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      _set_defaults();
    }

    // Constructor :
    charged_particle_tracking_module::charged_particle_tracking_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
    }

    // Destructor :
    charged_particle_tracking_module::~charged_particle_tracking_module()
    {
      if (is_initialized()) {
        charged_particle_tracking_module::reset();
      }
    }

    // Processing :
    dpp::base_module::process_status
    charged_particle_tracking_module::process(datatools::things & event)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");
      namespace snedm = snemo::datamodel;

      // Get required input products
      const auto & the_calibrated_data = event.get<snedm::calibrated_data>(CDTag_);
      const auto & the_tracker_trajectory_data = event.get<snedm::tracker_trajectory_data>(TTDTag_);

      // Create or reset output bank
      auto & the_particle_track_data =
        ::snedm::getOrAddToEvent<snedm::particle_track_data>(PTDTag_, event);
      the_particle_track_data.clear();

      if (datatools::logger::is_debug(get_logging_priority())) {
        std::cerr << "\n\n[debug] ************ CHARGED_PARTICLE_TRACKING_MODULE : NEW EVENT **************\n\n\n";
      }

      // Main processing method :
      DT_LOG_TRACE(get_logging_priority(), "Main processing...");
      this->_process(the_calibrated_data, the_tracker_trajectory_data, the_particle_track_data);
      DT_LOG_TRACE(get_logging_priority(), "Post processing...");
      this->_post_process(the_calibrated_data, the_particle_track_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void charged_particle_tracking_module::_process(const snemo::datamodel::calibrated_data & calibrated_data_,
                                                    const snemo::datamodel::tracker_trajectory_data & tracker_trajectory_data_,
                                                    snemo::datamodel::particle_track_data & particle_track_data_)
    {
      namespace snedm = snemo::datamodel;

      if (!tracker_trajectory_data_.has_default_solution()) {
        DT_LOG_DEBUG(get_logging_priority(), "No tracker trajectory default solution...");
        // Fill non associated calorimeter hits
        DT_LOG_DEBUG(get_logging_priority(), "Fill non associated calorimeter hits...");
        for (const auto & caloHit : calibrated_data_.calorimeter_hits()) {
          particle_track_data_.isolatedCalorimeters().push_back(caloHit);
        }
        return;
      }

      const snedm::tracker_trajectory_solution & a_solution =
        tracker_trajectory_data_.get_default_solution();
      const snedm::TrackerTrajectoryHdlCollection & trajectories = a_solution.get_trajectories();

      for (const datatools::handle<snedm::tracker_trajectory> & a_trajectory : trajectories) {
        // Look into trajectories to find the default
        // trajectory. Here, default means the one with the best
        // chi2. This flag is set by the 'fitting' module.
        if (not a_trajectory->get_fit_infos().is_best()) {
          continue;
        }
        DT_LOG_DEBUG(get_logging_priority(), "Processing the best (default) trajectory...");

        // Add a new particle track
        auto hPT = datatools::make_handle<snedm::particle_track>();
        hPT->set_trajectory_handle(a_trajectory);
        hPT->set_track_id(particle_track_data_.numberOfParticles());
        particle_track_data_.insertParticle(hPT);

        // Compute particle charge:
        if (CCAlgo_) {
          DT_LOG_DEBUG(get_logging_priority(), "Processing CC...");
          CCAlgo_->process(*a_trajectory, *hPT);
        }
        
        // Determine track vertices:
        if (VEAlgo_) {
          DT_LOG_DEBUG(get_logging_priority(), "Processing VE...");
          VEAlgo_->process(*a_trajectory, *hPT);
        }
        
        // Associate vertices to calorimeter hits:
        if (CAAlgo_) {
          DT_LOG_DEBUG(get_logging_priority(), "Processing CA...");
          CAAlgo_->process(calibrated_data_.calorimeter_hits(), *hPT);
        }
        
      }

      // Alpha finder:
      if (AFAlgo_) {
        DT_LOG_DEBUG(get_logging_priority(), "Processing AF...");
        AFAlgo_->process(tracker_trajectory_data_, particle_track_data_);
      }
      return;
    }

    void charged_particle_tracking_module::_post_process(const snemo::datamodel::calibrated_data & calibrated_data_,
                                                         snemo::datamodel::particle_track_data & particle_track_data_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Post-processing...");
      namespace snedm = snemo::datamodel;
      // Grab non associated calorimeters :
      if (!particle_track_data_.hasIsolatedCalorimeters()) {
        DT_LOG_DEBUG(get_logging_priority(), "No isolated calorimeter");
        geomtools::base_hit::has_flag_predicate asso_pred(calorimeter_utils::associated_flag());
        geomtools::base_hit::negates_predicate not_asso_pred(asso_pred);
        // Wrapper predicates :
        datatools::mother_to_daughter_predicate<geomtools::base_hit, snedm::calibrated_calorimeter_hit>
          pred_M2D(not_asso_pred);
        datatools::handle_predicate<snedm::calibrated_calorimeter_hit> pred_via_handle(pred_M2D);

        const snedm::CalorimeterHitHdlCollection & chits =
          calibrated_data_.calorimeter_hits();
        // The below might be better with copy_if and back_inserter?
        auto ihit = std::find_if(chits.begin(), chits.end(), pred_via_handle);
        while (ihit != chits.end()) {
          particle_track_data_.isolatedCalorimeters().push_back(*ihit);
          ihit = std::find_if(++ihit, chits.end(), pred_via_handle);
        }
      }

      // 2015/12/02 XG: Also look if the non associated calorimeters are
      // isolated i.e. without Geiger cells in front or not: tag them
      // consequently
      const snedm::TrackerHitHdlCollection & thits = calibrated_data_.tracker_hits();
      snedm::CalorimeterHitHdlCollection & chits = particle_track_data_.isolatedCalorimeters();

      for (datatools::handle<snedm::calibrated_calorimeter_hit> & a_calo_hit : chits) {
        DT_LOG_DEBUG(get_logging_priority(), "Loop on isolated calorimeters");
        const bool has_neighbors =
          calorimeter_utils::has_flag(*a_calo_hit, calorimeter_utils::neighbor_flag());
        bool has_gg_in_front = false;

        // Getting geometry mapping for parted block
        const geomtools::mapping & the_mapping = geoManager_->get_mapping();
        std::vector<geomtools::geom_id> gids;
        the_mapping.compute_matching_geom_id(a_calo_hit->get_geom_id(), gids);

        for (const geomtools::geom_id & a_gid : gids) {
          const geomtools::geom_info * ginfo_ptr = the_mapping.get_geom_info_ptr(a_gid);
          if (ginfo_ptr == nullptr) {
            DT_LOG_WARNING(get_logging_priority(), "Unmapped geom id " << a_gid << "!");
            continue;
          }
          // Loop over all calibrated geiger hits to find one close enough
          for (const datatools::handle<snedm::calibrated_tracker_hit> & a_tracker_hit : thits) {
            if (!a_tracker_hit->has_xy()) {
              continue;
            }
            const geomtools::vector_3d cell_pos(a_tracker_hit->get_x(),
                                                a_tracker_hit->get_y(),
                                                a_tracker_hit->get_z());
            // Tolerance must be understood as 'skin' tolerance so must be
            // multiplied by a factor of 2
            const double tolerance = 100 * CLHEP::mm;
            if (geomtools::mapping::check_inside(*ginfo_ptr, cell_pos, tolerance, true)) {
              has_gg_in_front = true;
              break;
            }
          }  // end of tracker hits

          if (has_gg_in_front) {
            break;
          }
        } // end of calorimeter geom ids

        if (!has_gg_in_front || (has_neighbors && has_gg_in_front)) {
          calorimeter_utils::flag_as(*a_calo_hit, calorimeter_utils::isolated_flag());
        }
      }  // end of calorimeter hits
      DT_LOG_DEBUG(get_logging_priority(), "Post-processing is done.");
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::charged_particle_tracking_module, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::charged_particle_tracking_module");
  ocd_.set_class_description(
                             "A module that performs the physical interpretation of tracker trajectory");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation(
                               "This module uses some dedicated drivers to reconstruct physical quantities        \n"
                               "such as electric charge or track vertices.                                        \n"
                               "It uses 3 dedicated drivers to perform reconstruction steps:                      \n"
                               " 1) Charge Computation Driver determines the electric charge of the track         \n"
                               " 2) Vertex Extrapolation Driver builds the list of vertices such as               \n"
                               "    foil vertex or calorimeter wall vertices                                      \n"
                               " 3) Calorimeter Association Driver associates a track with a calorimeter hit      \n"
                               " 4) Alpha Finder Driver looks for short alpha track with only 1 or 2 tracker hits \n");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
      .set_terse_description("The label/name of the 'calibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used  \n"
                            "as the source of input calorimeter hits. \n")
      .set_default_value_string(snedm::labels::calibrated_data())
      .add_example(
                   "Use an alternative name for the \n"
                   "'calibrated data' bank::        \n"
                   "                                \n"
                   "  CD_label : string = \"CD2\"   \n"
                   "                                \n");
  }

  {
    // Description of the 'TTD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TTD_label")
      .set_terse_description("The label/name of the 'tracker trajectory data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used      \n"
                            "as the source of input tracker trajectories. \n")
      .set_default_value_string(snedm::labels::tracker_trajectory_data())
      .add_example(
                   "Use an alternative name for the  \n"
                   "'tracker trajectory data' bank:: \n"
                   "                                 \n"
                   "  TTD_label : string = \"TTD2\"  \n"
                   "                                 \n");
  }

  {
    // Description of the 'PTD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("PTD_label")
      .set_terse_description("The label/name of the 'particle track data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "This is the name of the bank to be used as \n"
                            "the sink of output particle tracks.        \n")
      .set_default_value_string(snedm::labels::particle_track_data())
      .add_example(
                   "Use an alternative name for the \n"
                   "'particle track data' bank::    \n"
                   "                                \n"
                   "  PTD_label : string = \"PTD2\" \n"
                   "                                \n");
  }

  {
    // Description of the 'drivers' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("drivers")
      .set_terse_description("The list of drivers id to be used")
      .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
      .set_long_description(
                            "Supported values are:                         \n"
                            "                                              \n"
                            " * ``CCD`` for Charge Computation Driver      \n"
                            " * ``VED`` for Vertex Extrapolation Driver    \n"
                            " * ``CAD`` for Calorimeter Association Driver \n"
                            " * ``AFD`` for Alpha Finder Driver            \n"
                            "                                              \n")
      .set_mandatory(false)
      .add_example(
                   "Use Vertex Extrapolation Driver only:: \n"
                   "                                       \n"
                   "  drivers : string[1] = \"VED\"        \n"
                   "                                       \n");
  }

  // Invoke specific OCD support from the driver class:
  //::snemo::reconstruction::vertex_extrapolation_driver::init_ocd(ocd_);
  //::snemo::reconstruction::charge_computation_driver::init_ocd(ocd_);
  //::snemo::reconstruction::calorimeter_association_driver::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
                               "Here is a full configuration example in the ``datatools::properties`` \n"
                               "ASCII format::                                                        \n"
                               "                                                                      \n"
                               "  CD_label                     : string = \"CD\"                      \n"
                               "  TTD_label                    : string = \"TTD\"                     \n"
                               "  PTD_label                    : string = \"PTD\"                     \n"
                               "  drivers                      : string[3] = \"VED\" \"CCD\" \"CAD\"  \n"
                               "  VED.logging.priority         : string = \"fatal\"                   \n"
                               "  VED.use_linear_extrapolation : boolean = 0                          \n"
                               "  CCD.logging_priority         : string = \"fatal\"                   \n"
                               "  CCD.charge_from_source       : boolean = 1                          \n"
                               "  CAD.logging_priority         : string = \"fatal\"                   \n"
                               "  CAD.matching_tolerance       : real as length = 50 mm               \n"
                               "                                                                      \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::charged_particle_tracking_module,
                               "snemo::reconstruction::charged_particle_tracking_module")

// end of falaise/snemo/reconstruction/charged_particle_tracking_module.cc
