// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_calorimeter_s2c_module.cc
 */

// Ourselves:
#include "mock_calorimeter_s2c_module.h"

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
#include <mctools/utils.h>

// This project :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/services/services.h>

namespace snemo {

namespace processing {

// Registration instantiation macro :
DPP_MODULE_REGISTRATION_IMPLEMENT(mock_calorimeter_s2c_module,
                                  "snemo::processing::mock_calorimeter_s2c_module")

void mock_calorimeter_s2c_module::initialize(const datatools::properties& ps,
                                             datatools::service_manager& sm,
                                             dpp::module_handle_dict_type& /*unused*/) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is already initialized ! ");

  this->base_module::_common_initialize(ps);
  falaise::property_set fps{ps};

  sdInputTag = fps.get<std::string>("SD_label", snedm::labels::simulated_data());
  cdOutputTag = fps.get<std::string>("CD_label", snedm::labels::calibrated_data());

  geoManager = snemo::service_handle<snemo::geometry_svc>{sm};

  // Initialize the embedded random number generator:
  int random_seed = fps.get<int>("random.seed", 12345);
  std::string random_id = fps.get<std::string>("random.id", "mt19937");
  RNG_.init(random_id, random_seed);

  // Configure models for each calorimeter type
  caloTypes = fps.get<std::vector<std::string>>("hit_categories", {"calo", "xcalo", "gveto"});

  caloModels = {};

  // Initialize the calorimeter regime utility from the database
  if (ps.has_key("calorimeter_regime_database_path")) {
    std::string _calorimeter_regime_database_path_ = ps.fetch_string("calorimeter_regime_database_path");
    datatools::fetch_path_with_env(_calorimeter_regime_database_path_);
    this->parse_calorimeter_regime_database(_calorimeter_regime_database_path_);
  }

  // Setup trigger time
  timeWindow = fps.get<falaise::time_t>("cluster_time_width", {100., "ns"})();

  // 2012-09-17 FM : support reference to the MC true hit ID
  assocMCHitId = fps.get<bool>("store_mc_hit_id", false);

  // Get the alpha quenching (always)
  quenchAlphas = true;

  this->base_module::_set_initialized(true);
}

void mock_calorimeter_s2c_module::reset() { this->base_module::_set_initialized(false); }

void mock_calorimeter_s2c_module::parse_calorimeter_regime_database(const std::string & database_path_)
{
  DT_LOG_DEBUG(get_logging_priority(), "parsing file: " <<  database_path_);
  std::ifstream database_file_ (database_path_.c_str());

  // Remove first line (header)
  std::string a_line_;
  std::getline(database_file_, a_line_);

  while ( std::getline(database_file_, a_line_) ) {

    std::stringstream a_stream (a_line_);

    // Retrieve each field separated by "\t"
    std::string a_field_;
    std::vector<std::string> stream_fields_;
    while ( std::getline(a_stream, a_field_, '\t') ) {
      stream_fields_.push_back(a_field_);
    }

    // field 0 : geom_id
    std::string a_geomid_string_ = stream_fields_[0];
    // database_file_ >> a_geomid_string_;
    DT_LOG_DEBUG(get_logging_priority(), "read '" << a_geomid_string_ << "'");

    std::istringstream a_geomid_iss (a_geomid_string_);
    geomtools::geom_id a_geomid_;
    a_geomid_iss >> a_geomid_;

    // Make sure geom_id is syntaxically valid
    DT_THROW_IF(!a_geomid_.is_valid(), std::logic_error, "geom_id syntax of '" << a_geomid_iss.str() << "' is not valid !");

    // main wall geom_id need an additional depth '*'
    if (a_geomid_.get_type() == 1302)
      a_geomid_.set_any(a_geomid_.get_depth());

    // field1: energy resolution
    falaise::fraction_t a_fwhm_value_ {datatools::units::get_value_with_unit(stream_fields_[1])/CLHEP::perCent, "%"};

    // field2/field3: high/low energy threshold
    falaise::energy_t a_ht_value_   {datatools::units::get_value_with_unit(stream_fields_[2])/CLHEP::keV, "keV"};
    falaise::energy_t a_lt_value_   {datatools::units::get_value_with_unit(stream_fields_[3])/CLHEP::keV, "keV"};

    // field4: alpha quenching parameters
    datatools::properties::data::vdouble some_alpha_quenching_pars_;
    std::istringstream alpha_quenching_iss (stream_fields_[4]);
    for (int par = 0; par < 3 ; ++par) {
      double a_alpha_quenching_par;
      alpha_quenching_iss >> a_alpha_quenching_par;
      some_alpha_quenching_pars_.push_back(a_alpha_quenching_par);
    }

    // field5: scintillator relaxation time
    falaise::time_t a_sc_relax_time_ {datatools::units::get_value_with_unit(stream_fields_[5])/CLHEP::ns, "ns"};

    falaise::property_set a_calorimeter_regime_ps;
    a_calorimeter_regime_ps.put("energy.resolution",     a_fwhm_value_);
    a_calorimeter_regime_ps.put("energy.high_threshold", a_ht_value_);
    a_calorimeter_regime_ps.put("energy.low_threshold",  a_lt_value_);
    a_calorimeter_regime_ps.put("alpha_quenching_parameters",   some_alpha_quenching_pars_);
    a_calorimeter_regime_ps.put("scintillator_relaxation_time", a_sc_relax_time_);

    // create and store this regim in the model map
    CalorimeterModel a_calorimeter_regime_ (a_calorimeter_regime_ps);
    caloModels[a_geomid_] = a_calorimeter_regime_;

  }

  DT_LOG_DEBUG(get_logging_priority(), "parsing file end");
}

// Processing :
dpp::base_module::process_status mock_calorimeter_s2c_module::process(datatools::things& event) {
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Module '" << get_name() << "' is not initialized !");

  // Check Input simulated data exists, or fail
  if (!event.has(sdInputTag)) {
    throw std::logic_error("Missing simulated data to be processed !");
    return dpp::base_module::PROCESS_ERROR;
  }
  auto& simulatedData = event.get<mctools::simulated_data>(sdInputTag);

  // check if some 'calibrated_data' are available in the data model:
  // Calibrated Data is a single object with each hit collection
  // May, or may not, have it depending on if we run before or after
  // other calibrators
  auto& calibratedData =
      snedm::getOrAddToEvent<snemo::datamodel::calibrated_data>(cdOutputTag, event);

  // Always rewrite hits....
  calibratedData.calorimeter_hits().clear();

  // Main processing method :
  process_impl(simulatedData, calibratedData.calorimeter_hits());

  return dpp::base_module::PROCESS_SUCCESS;
}

const CalorimeterModel& mock_calorimeter_s2c_module::get_calorimeter_regime(const geomtools::geom_id & gid)
{
  DT_THROW_IF(caloModels.find(gid) == caloModels.end(), std::logic_error, "Missing calorimeter regime for '" << gid << "' !");
  return caloModels.at(gid);
}

// Here collect the 'calorimeter' raw hits from the simulation data source
// and build the final list of calibrated 'calorimeter' hits
void mock_calorimeter_s2c_module::digitizeHits(
    const mctools::simulated_data& simdata,
    snemo::datamodel::CalorimeterHitHdlCollection& calohits) {
  uint32_t calibrated_calorimeter_hit_id = 0;

  for (const auto& caloType : caloTypes) {

    if (!simdata.has_step_hits(caloType)) {
      continue;
    }

    // Loop over per-category hits
    auto& mcHits = simdata.get_step_hits(caloType);

    // auto& theCaloModel = calo.second;

    for (auto& a_calo_mc_hit : mcHits) {

      // Extract the corresponding geom ID:
      auto& geomID = a_calo_mc_hit->get_geom_id();

      // Retrieve the calorimeter regime
      auto& theCaloModel = this->get_calorimeter_regime(geomID);

      // Quench energy if it's an Alpha particle
      double energyDeposit = a_calo_mc_hit->get_energy_deposit();

      if (quenchAlphas && a_calo_mc_hit->get_particle_name() == "alpha") {
        energyDeposit = theCaloModel.quenchAlphaParticle(energyDeposit);
      }

      // Get the step hit time start:
      const double step_hit_time_start = a_calo_mc_hit->get_time_start();

      using CCHitHdl = snemo::datamodel::CalorimeterHitHdlCollection::value_type;

      auto found = std::find_if(calohits.rbegin(), calohits.rend(), [&geomID](CCHitHdl const& x) {
        return x->get_geom_id() == geomID;
      });

      if (found == calohits.rend()) {
        // Then it's a new hit
        auto newHit = datatools::make_handle<snemo::datamodel::calibrated_calorimeter_hit>();
        // auto& newHit = newHandle.grab();

        newHit->set_hit_id(calibrated_calorimeter_hit_id++);
        newHit->set_geom_id(a_calo_mc_hit->get_geom_id());

        // sigma time and sigma energy are computed later
        newHit->set_time(step_hit_time_start);
        newHit->set_energy(energyDeposit);

        // Add a properties to ease the final calibration
        newHit->grab_auxiliaries().store("category", caloType);

        // 2012-09-17 FM : support reference to the MC true hit ID
        if (assocMCHitId) {
          newHit->grab_auxiliaries().store(mctools::hit_utils::HIT_MC_HIT_ID_KEY,
                                           a_calo_mc_hit->get_hit_id());
        }

        // Append it to the collection :
        calohits.push_back(newHit);
      } else {
        // This geom_id is already used by some previous calorimeter hit:
        // we update this hit !
        auto& existingHit = *found;

        // Grab auxiliaries :
        datatools::properties& cc_prop = existingHit->grab_auxiliaries();

        // 2012-07-26 FM : support reference to the MC true hit ID
        if (assocMCHitId) {
          cc_prop.update(mctools::hit_utils::HIT_MC_HIT_ID_KEY, a_calo_mc_hit->get_hit_id());
        }

        // Check time between clusters
        const double delta_time = step_hit_time_start - existingHit->get_time();
        // cluster arriving too late : do not care of it
        if (delta_time > timeWindow) {
          cc_prop.update_flag("pile_up");
          continue;
        }

        // Cluster coming before : it becomes the new one
        if (delta_time < -timeWindow) {
          cc_prop.update_flag("pile_up");
          existingHit->set_time(step_hit_time_start);
          existingHit->set_energy(energyDeposit);
          continue;
        }

        // Find the first hit in time
        const double calo_time = std::min(step_hit_time_start, existingHit->get_time());
        existingHit->set_time(calo_time);

        // Sum energies
        const double calo_energy = energyDeposit + existingHit->get_energy();
        existingHit->set_energy(calo_energy);
      }
    }  // loop over hits
  }    // loop over hit category
}

// Calibrate calorimeter hits from digitization informations:
void mock_calorimeter_s2c_module::calibrateHits(
    snemo::datamodel::CalorimeterHitHdlCollection& calohits) {
  for (auto& theCaloHit : calohits) {
    // Setting category in order to get the correct energy resolution:
    // first recover the calorimeter category
    const geomtools::geom_id& geomID = theCaloHit->get_geom_id();
    const CalorimeterModel& the_calo_regime =  this->get_calorimeter_regime(geomID);

    // Compute a random 'experimental' energy taking into account
    // the expected energy resolution of the calorimeter hit:
    const double energy = theCaloHit->get_energy();
    const double measuredEnergy = the_calo_regime.smearEnergy(RNG_, energy);
    const double measuredEnergySigma = the_calo_regime.getSigmaEnergy(measuredEnergy);
    theCaloHit->set_energy(measuredEnergy);
    theCaloHit->set_sigma_energy(measuredEnergySigma);

    // Compute a random 'experimental' time taking into account
    // the expected time resolution of the calorimeter hit:
    const double time = theCaloHit->get_time();
    const double measuredTime = the_calo_regime.smearTime(RNG_, time, measuredEnergy);
    const double measuredTimeSigma = the_calo_regime.getSigmaTime(measuredEnergy);
    theCaloHit->set_time(measuredTime);
    theCaloHit->set_sigma_time(measuredTimeSigma);
  }
}

// Select calorimeter hit following trigger conditions
void mock_calorimeter_s2c_module::triggerHits(
    snemo::datamodel::CalorimeterHitHdlCollection& calohits) {
  bool high_threshold = false;
  for (auto& theCaloHit : calohits) {
    // Setting category in order to get the correct trigger parameters:
    // first recover the calorimeter category
    const geomtools::geom_id& geomID = theCaloHit->get_geom_id();
    const CalorimeterModel& the_calo_regime =  this->get_calorimeter_regime(geomID);
    const double energy = theCaloHit->get_energy();
    if (the_calo_regime.aboveHighThreshold(energy)) {
      high_threshold = true;
      break;
    }
  }

  if (high_threshold) {
    // Search and erase for low threshold hits:
    // Awkward because we have to handle all hit categories together
    for (auto iCheckedHit = calohits.begin(); iCheckedHit != calohits.end(); /**/) {
      const geomtools::geom_id& geomID = (*iCheckedHit)->get_geom_id();
      const CalorimeterModel& the_calo_regime =  this->get_calorimeter_regime(geomID);
      const double energy = (*iCheckedHit)->get_energy();
      // If energy hit is too low then remove calorimeter hit
      if (!the_calo_regime.aboveLowThreshold(energy)) {
        iCheckedHit = calohits.erase(iCheckedHit);
      } else {
        ++iCheckedHit;
      }
    }
  } else {
    calohits.clear();
  }
}

void mock_calorimeter_s2c_module::process_impl(
    const mctools::simulated_data& simdata,
    snemo::datamodel::CalorimeterHitHdlCollection& calohits) {
  digitizeHits(simdata, calohits);
  calibrateHits(calohits);
  triggerHits(calohits);
}

}  // end of namespace processing

}  // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::mock_calorimeter_s2c_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::mock_calorimeter_s2c_module, ocd_) {
  ocd_.set_class_name("snemo::processing::mock_calorimeter_s2c_module");
  ocd_.set_class_description(
      "A module that performs a mock digitization/calibration of the calorimeter simulated data");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SD_label")
        .set_terse_description("The label/name of the 'simulated data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used   \n"
            "as the input simulated calorimeter hits.  \n")
        .set_default_value_string(snedm::labels::simulated_data())
        .add_example(
            "Use an alternative name for the 'simulated data' bank:: \n"
            "                                \n"
            "  SD_label : string = \"SD2\"   \n"
            "                                \n");
  }

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
        .set_terse_description("The label/name of the 'calibrated data' bank")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This is the name of the bank to be used    \n"
            "as the output calibrated calorimeter hits. \n")
        .set_default_value_string(snedm::labels::calibrated_data())
        .add_example(
            "Use an alternative name for the 'calibrated data' bank:: \n"
            "                                \n"
            "  CD_label : string = \"CD2\"   \n"
            "                                \n");
  }

  {
    // Description of the 'random.seed' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("random.seed")
        .set_terse_description("The seed for the embedded PRNG")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_long_description("Only used if no external PRNG is provided.")
        .set_default_value_integer(12345)
        .add_example(
            "Use an alternative seed for the PRNG:: \n"
            "                                       \n"
            "  random.seed : integer = 314159       \n"
            "                                       \n");
  }

  {
    // Description of the 'random.id' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("random.id")
        .set_terse_description("The Id for the embedded PRNG")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("Only used if no external PRNG is provided.")
        .set_default_value_string("mt19937")
        .add_example(
            "Use an alternative Id for the PRNG::   \n"
            "                                       \n"
            "  random.id : string = \"taus2\"       \n"
            "                                       \n");
  }

  {
    // Description of the 'cluster_time_width' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cluster_time_width")
        .set_terse_description("The width of the clustering time window")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description(
            "This is the time spread in which one consider a set of \n"
            "calorimeter hits to be part of the same signal.        \n")
        .set_explicit_unit(true)
        .set_unit_label("energy")
        .set_unit_symbol("ns")
        .set_default_value_real(100 * CLHEP::ns)
        .add_example(
            "Use the default value::                     \n"
            "                                             \n"
            "  cluster_time_width : real as time = 100 ns \n"
            "                                             \n");
  }

  {
    // Description of the 'store_mc_hit_id' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("store_mc_hit_id")
        .set_terse_description(
            "Flag to activate the storage of the truth hit Ids in the calibrated hits")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        // .set_long_description("Default value: ``0`` ")
        .set_default_value_boolean(false)
        .add_example(
            "Use the default value::          \n"
            "                                 \n"
            "  store_mc_hit_id : boolean = 0  \n"
            "                                 \n");
  }

  {
    // Description of the 'hit_categories' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("hit_categories")
        .set_terse_description("The list of hit categories to be processed")
        .set_traits(datatools::TYPE_STRING, configuration_property_description::ARRAY)
        .set_mandatory(true)
        .set_long_description(
            "For each hit category, we must declare some            \n"
            "configuration parameters for the associated            \n"
            "calorimeter regime. These properties must be prefixed  \n"
            "by the name of the hit category (see OCD support for   \n"
            "the ``snemo::processing::calorimeter_regime`` class).  \n")
        .add_example(
            "Use 2 hit categories to be processed by this module:: \n"
            "                                                      \n"
            "                                                      \n"
            "  hit_categories : string[2] = \"calo\" \"xcalo\"               \n"
            "                                                                \n"
            "  # Parameters for the calorimeter regime for 'calo' hits:      \n"
            "  calo.energy.resolution     : real as fraction = 8 %           \n"
            "  calo.energy.low_threshold  : real as energy = 50 keV          \n"
            "  calo.energy.high_threshold : real as energy = 150 keV         \n"
            "  calo.scintillator_relaxation_time : real as time = 6.0 ns     \n"
            "  calo.alpha_quenching_parameters   : real[3] = 77.4 0.639 2.34 \n"
            "  ...                                                           \n"
            "  # Parameters for the calorimeter regime for 'xcalo' hits:     \n"
            "  xcalo.energy.resolution     : real as fraction = 12 %         \n"
            "  xcalo.energy.low_threshold  : real as energy = 50 keV         \n"
            "  xcalo.energy.high_threshold : real as energy = 150 keV        \n"
            "  xcalo.scintillator_relaxation_time : real as time = 6.0 ns    \n"
            "  xcalo.alpha_quenching_parameters   : real[3] = 77.4 0.639 2.34\n"
            "  ...                                                           \n"
            "                                                                \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the                    \n"
      "``datatools::properties`` ASCII format::                       \n"
      "                                                               \n"
      "  SD_label    : string = \"SD\"                                \n"
      "  CD_label    : string = \"CD\"                                \n"
      "  Geo_label   : string = \"geometry\"                          \n"
      "  random.seed : integer = 314159                               \n"
      "  random.id   : string = \"taus2\"                             \n"
      "  cluster_time_width : real as time = 100 ns                   \n"
      "  alpha_quenching    : boolean = 1                             \n"
      "  store_mc_hit_id    : boolean = 0                             \n"
      "                                                               \n"
      "  hit_categories     : string[2] = \"calo\" \"xcalo\"          \n"
      "                                                               \n"
      "  calo.energy.resolution     : real as fraction = 8 %          \n"
      "  calo.energy.low_threshold  : real as energy = 50 keV         \n"
      "  calo.energy.high_threshold : real as energy = 150 keV        \n"
      "  calo.scintillator_relaxation_time : real as time = 6.0 ns    \n"
      "  calo.alpha_quenching_parameters : real[3] = 77.4 0.639 2.34  \n"
      "                                                               \n"
      "  xcalo.energy.resolution     : real as fraction = 12 %        \n"
      "  xcalo.energy.low_threshold  : real as energy = 50 keV        \n"
      "  xcalo.energy.high_threshold : real as energy = 150 keV       \n"
      "  xcalo.scintillator_relaxation_time : real as time = 6.0 ns   \n"
      "  xcalo.alpha_quenching_parameters : real[3] = 77.4 0.639 2.34 \n"
      "                                                              \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::processing::mock_calorimeter_s2c_module' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::mock_calorimeter_s2c_module,
                               "snemo::processing::mock_calorimeter_s2c_module")

// end of falaise/snemo/processing/mock_calorimeter_s2c_module.cc
