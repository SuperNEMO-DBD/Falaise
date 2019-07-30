/// \file falaise/snemo/reconstruction/gamma_tracking_driver.cc

// Ourselves:
#include <GammaTracking/gamma_tracking_driver.h>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

// Gamma_Tracking library
#include <GammaTracking/event.h>
#include <GammaTracking/tof_computing.h>

namespace snemo {

namespace reconstruction {

const std::string& gamma_tracking_driver::gamma_tracking_id() {
  static const std::string _id("gamma_tracking");
  return _id;
}

// Constructor
gamma_tracking_driver::gamma_tracking_driver()
    : snemo::processing::base_gamma_builder(gamma_tracking_driver::gamma_tracking_id()) {
  _set_defaults();
}

// Destructor
gamma_tracking_driver::~gamma_tracking_driver() = default;

// Initialize the gamma tracker through configuration properties
void gamma_tracking_driver::initialize(const datatools::properties& setup_) {
  this->snemo::processing::base_gamma_builder::_initialize(setup_);

  // Extract the setup of the gamma tracking algo :
  datatools::properties gt_setup;
  setup_.export_and_rename_starting_with(gt_setup, "GT.", "");
  gtAlgo_.initialize(gt_setup);

  _set_initialized(true);
}

// Reset the gamma tracker
void gamma_tracking_driver::reset() {
  this->snemo::processing::base_gamma_builder::_reset();
  _set_defaults();
}

void gamma_tracking_driver::_set_defaults() {
  this->snemo::processing::base_gamma_builder::_set_defaults();
}

int gamma_tracking_driver::_prepare_process(
    const base_gamma_builder::hit_collection_type& calo_hits_,
    snemo::datamodel::particle_track_data& ptd_) {
  base_gamma_builder::_prepare_process(calo_hits_, ptd_);

  gt::event& an_event = gtAlgo_.grab_event();
  gt::event::calorimeter_collection_type& the_gamma_calos = an_event.grab_calorimeters();

  for (const auto& a_calo_hit : calo_hits_) {
    // TODO: Why this awkward way?
    // Instead: Create the new hit, then as last step emplace it.
    the_gamma_calos.insert(std::make_pair(a_calo_hit->get_hit_id(), gt::event::calorimeter_hit{}));
    gt::event::calorimeter_hit& new_calo_hit = the_gamma_calos[a_calo_hit->get_hit_id()];

    const geomtools::geom_id& a_gid = a_calo_hit->get_geom_id();

    const snemo::geometry::calo_locator& calo_locator = get_calo_locator();
    const snemo::geometry::xcalo_locator& xcalo_locator = get_xcalo_locator();
    const snemo::geometry::gveto_locator& gveto_locator = get_gveto_locator();
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

    new_calo_hit.time = a_calo_hit->get_time();
    new_calo_hit.sigma_time = a_calo_hit->get_sigma_time();
    new_calo_hit.energy = a_calo_hit->get_energy();
    new_calo_hit.sigma_energy = a_calo_hit->get_sigma_energy();
  }

  gtAlgo_.prepare_process();

  return 0;
}

// Main tracking method
int gamma_tracking_driver::_process_algo(
    const base_gamma_builder::hit_collection_type& /*calo_hits_*/,
    snemo::datamodel::particle_track_data& ptd_) {
  // Running gamma tracking
  gtAlgo_.process();
  gt::gamma_tracking::solution_type gamma_tracks;
  gtAlgo_.get_reflects(gamma_tracks);

  for (const auto& a_list : gamma_tracks) {
    auto hPT = datatools::make_handle<snemo::datamodel::particle_track>();
    hPT->set_track_id(ptd_.get_number_of_particles());
    hPT->set_charge(snemo::datamodel::particle_track::neutral);
    ptd_.add_particle(hPT);

    // List of associated calorimeters
    for (const int calo_id : a_list) {
      // Set calorimeter association
      const snemo::datamodel::calibrated_calorimeter_hit::collection_type& cch =
          ptd_.get_non_associated_calorimeters();
      geomtools::base_hit::has_hit_id_predicate hit_pred(calo_id);
      datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                              snemo::datamodel::calibrated_calorimeter_hit>
          pred_M2D(hit_pred);
      datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(
          pred_M2D);
      auto found = std::find_if(cch.begin(), cch.end(), pred_via_handle);
      DT_THROW_IF(found == cch.end(), std::logic_error,
                  "Calibrated calorimeter hit with id " << calo_id << " can not be found");
      hPT->get_associated_calorimeter_hits().push_back(*found);

      // Build vertex
      auto hBS = datatools::make_handle<geomtools::blur_spot>();
      hBS->set_hit_id(calo_id);
      hBS->set_geom_id((*found)->get_geom_id());
      hBS->set_blur_dimension(geomtools::blur_spot::dimension_three);

      const gt::event::calorimeter_collection_type& the_gamma_calos =
          gtAlgo_.get_event().get_calorimeters();
      hBS->grab_auxiliaries().store(snemo::datamodel::particle_track::vertex_type_key(),
                                    the_gamma_calos.at(calo_id).label);
      hBS->set_position(the_gamma_calos.at(calo_id).position);

      hPT->get_vertices().push_back(hBS);
    }  // end of gamma hits
  }    // end of gammas

  // Reset GT algorithm
  gtAlgo_.reset();

  return 0;
}

// static
void gamma_tracking_driver::init_ocd(datatools::object_configuration_description& ocd_) {
  // Invoke OCD support from parent class :
  ::snemo::processing::base_gamma_builder::ocd_support(ocd_);
}

}  // end of namespace reconstruction

}  // end of namespace snemo
/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::gamma_tracking_driver, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::gamma_tracking_driver");
  ocd_.set_class_description("A driver class for the Gamma Tracking algorithm");
  ocd_.set_class_library("Falaise_GammaTracking");
  ocd_.set_class_documentation(
      "The driver manager for the Gamma Tracking algorithms\n"
      "/todo What does the manager do ?");

  ocd_.set_validation_support(true);
  ocd_.lock();
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::gamma_tracking_driver,
                               "snemo::reconstruction::gamma_tracking_driver")
