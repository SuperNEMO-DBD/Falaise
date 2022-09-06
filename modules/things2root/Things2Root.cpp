// - Implementation of Things2Root
// Example on how to access some geometry data from the geometry manager.
// Uses only the 'SD' data bank and there only the 'gg' geiger counter
// data from the tracker.
// 1) Access all available tracker data in SD
// 2) Write data to a flat TTree ROOT file

// Ourselves
#include "Things2Root.h"

// Standard Library
// Third Party
// - Boost:
#include <boost/foreach.hpp>
// - Root:
#include "TFile.h"
#include "TTree.h"

// Bayeux:
#include "bayeux/mctools/utils.h"

// This Project
// Macro which automatically implements the interface needed
// to enable the module to be loaded at runtime
// The first argument is the typename
// The second is the string key used to access the module in pipeline
// scripts. This must be globally unique.

DPP_MODULE_REGISTRATION_IMPLEMENT(Things2Root, "Things2Root")

struct Things2Root::working_space {

  // Calibrated tracker data
  std::vector<int> trackerid;
  std::vector<int> trackermodule;
  std::vector<int> trackerside;
  std::vector<int> trackerlayer;
  std::vector<int> trackercolumn;
  std::vector<double> trackerx;
  std::vector<double> trackery;
  std::vector<double> trackerz;
  std::vector<double> trackersigmaz;
  std::vector<double> trackerr;
  std::vector<double> trackersigmar;
  std::vector<double> trackerdelayedtime;
  std::vector<double> trackerdelayedtimeerror;
  std::vector<bool> isdelayed;
  std::vector<bool> isnoisy;
  std::vector<bool> isbottomcathodemissing;
  std::vector<bool> istopcathodemissing;
  std::vector<int> trackertruehitid;
  std::vector<int> trackertruetrackid;
  std::vector<int> trackertrueparenttrackid;

  // Calibrated calorimeter data
  std::vector<int> caloid;
  std::vector<int> calomodule;
  std::vector<int> caloside;
  std::vector<int> calocolumn;
  std::vector<int> calorow;
  std::vector<int> calowall;
  std::vector<double> calotime;
  std::vector<double> calosigmatime;
  std::vector<double> caloenergy;
  std::vector<double> calosigmaenergy;
  std::vector<int> calotype;

  // Precalibrated tracker data
  std::vector<int> pcdtrackerid;
  std::vector<int> pcdtrackermodule;
  std::vector<int> pcdtrackerside;
  std::vector<int> pcdtrackerlayer;
  std::vector<int> pcdtrackercolumn;
  std::vector<double> pcdtrackerreferencetime;
  std::vector<double> pcdtrackeranodicdrifttime;
  std::vector<double> pcdtrackersigmaanodicdrifttime;
  std::vector<double> pcdtrackerbottomcathodedrifttime;
  std::vector<double> pcdtrackersigmabottomcathodedrifttime;
  std::vector<double> pcdtrackertopcathodedrifttime;
  std::vector<double> pcdtrackersigmatopcathodedrifttime;
  std::vector<bool>   pcdtrackerisdelayed;

  // Precalibrated calo data
  std::vector<int> pcdcaloid;
  std::vector<int> pcdcalotype;
  std::vector<int> pcdcalomodule;
  std::vector<int> pcdcaloside;
  std::vector<int> pcdcalocolumn;
  std::vector<int> pcdcalorow;
  std::vector<int> pcdcalowall;
  std::vector<double> pcdcaloamplitude;
  std::vector<double> pcdcalosigmaamplitude;
  std::vector<double> pcdcalocharge;
  std::vector<double> pcdcalosigmacharge;
  std::vector<double> pcdcalotime;
  std::vector<double> pcdcalosigmatime;
  std::vector<double> pcdcalobaseline;
  std::vector<double> pcdcalosigmabaseline;
  std::vector<double> pcdcalorisingtime;
  std::vector<double> pcdcalosigmarisingtime;
  std::vector<double> pcdcalofallingtime;
  std::vector<double> pcdcalosigmafallingtime;
  std::vector<double> pcdcalotimewidth;
  std::vector<double> pcdcalosigmatimewidth;

  // Digitized tracker data
  std::vector<int> digitrackerid;
  std::vector<int> digitrackermodule;
  std::vector<int> digitrackerside;
  std::vector<int> digitrackerlayer;
  std::vector<int> digitrackercolumn;
  std::vector<std::vector<long int> > digitrackeranodetimestampR0;
  std::vector<std::vector<long int> > digitrackeranodetimestampR1;
  std::vector<std::vector<long int> > digitrackeranodetimestampR2;
  std::vector<std::vector<long int> > digitrackeranodetimestampR3;
  std::vector<std::vector<long int> > digitrackeranodetimestampR4;
  std::vector<std::vector<long int> > digitrackerbottomcathodetimestamp;
  std::vector<std::vector<long int> > digitrackertopcathodetimestamp;

  // Digitized calorimeter data
  std::vector<int> digicaloid;
  std::vector<int> digicalotype;
  std::vector<int> digicalomodule;
  std::vector<int> digicaloside;
  std::vector<int> digicalocolumn;
  std::vector<int> digicalorow;
  std::vector<int> digicalowall;
  std::vector<long int> digicalotimestamp;
  std::vector<bool> digicalolto;
  std::vector<bool> digicaloht;
  std::vector<uint16_t> digicalofcr;
  std::vector<uint16_t> digicalolttriggercounter;
  std::vector<uint32_t> digicaloltimecounter;
  std::vector<std::vector<int16_t> > digicalowaveform;
  std::vector<int16_t> digicalobaseline;
  std::vector<int16_t> digicalopeakamplitude;
  std::vector<int16_t> digicalopeakcell;
  std::vector<int32_t> digicalocharge;
  std::vector<int32_t> digicalorisingcell;
  std::vector<int32_t> digicalofallingcell;

  // for true gg hits
  std::vector<int> truetrackerid;
  std::vector<int> truetrackermodule;
  std::vector<int> truetrackerside;
  std::vector<int> truetrackerlayer;
  std::vector<int> truetrackercolumn;
  std::vector<double> truetrackertime;
  std::vector<double> truetrackerxstart;
  std::vector<double> truetrackerystart;
  std::vector<double> truetrackerzstart;
  std::vector<double> truetrackerxstop;
  std::vector<double> truetrackerystop;
  std::vector<double> truetrackerzstop;
  std::vector<int> truetrackertrackid;
  std::vector<int> truetrackerparenttrackid;

  // for true calo hits
  std::vector<int> truecaloid;
  std::vector<int> truecalotype;
  std::vector<int> truecalomodule;
  std::vector<int> truecaloside;
  std::vector<int> truecalocolumn;
  std::vector<int> truecalorow;
  std::vector<int> truecalowall;
  std::vector<double> truecalox;
  std::vector<double> truecaloy;
  std::vector<double> truecaloz;
  std::vector<double> truecalotime;
  std::vector<double> truecaloenergy;

  // for primary particles
  std::vector<int> partid;
  std::vector<int> parttype;
  std::vector<double> partpx;
  std::vector<double> partpy;
  std::vector<double> partpz;
  std::vector<double> parttime;
  std::vector<double> partenergy;

  void clear();
};

void Things2Root::working_space::clear() {

  // Clear calibrated tracker data
  trackerid.clear();
  trackermodule.clear();
  trackerside.clear();
  trackerlayer.clear();
  trackercolumn.clear();
  trackerx.clear();
  trackery.clear();
  trackerz.clear();
  trackersigmaz.clear();
  trackerr.clear();
  trackersigmar.clear();
  trackerdelayedtime.clear();
  trackerdelayedtimeerror.clear();
  isdelayed.clear();
  isnoisy.clear();
  isbottomcathodemissing.clear();
  istopcathodemissing.clear();
  trackertruehitid.clear();
  trackertruetrackid.clear();
  trackertrueparenttrackid.clear();

  // Clear calibrated calorimeter data
  caloid.clear();
  calomodule.clear();
  caloside.clear();
  calocolumn.clear();
  calorow.clear();
  calowall.clear();
  calotime.clear();
  calosigmatime.clear();
  caloenergy.clear();
  calosigmaenergy.clear();
  calotype.clear();

  // Clear precalibrated tracker data
  pcdtrackerid.clear();
  pcdtrackermodule.clear();
  pcdtrackerside.clear();
  pcdtrackerlayer.clear();
  pcdtrackercolumn.clear();
  pcdtrackerreferencetime.clear();
  pcdtrackeranodicdrifttime.clear();
  pcdtrackersigmaanodicdrifttime.clear();
  pcdtrackerbottomcathodedrifttime.clear();
  pcdtrackersigmabottomcathodedrifttime.clear();
  pcdtrackertopcathodedrifttime.clear();
  pcdtrackersigmatopcathodedrifttime.clear();
  pcdtrackerisdelayed.clear();

  // Clear precalibrated calo data
  pcdcaloid.clear();
  pcdcalotype.clear();
  pcdcalomodule.clear();
  pcdcaloside.clear();
  pcdcalocolumn.clear();
  pcdcalorow.clear();
  pcdcalowall.clear();
  pcdcaloamplitude.clear();
  pcdcalosigmaamplitude.clear();
  pcdcalocharge.clear();
  pcdcalosigmacharge.clear();
  pcdcalotime.clear();
  pcdcalosigmatime.clear();
  pcdcalobaseline.clear();
  pcdcalosigmabaseline.clear();
  pcdcalorisingtime.clear();
  pcdcalosigmarisingtime.clear();
  pcdcalofallingtime.clear();
  pcdcalosigmafallingtime.clear();
  pcdcalotimewidth.clear();
  pcdcalosigmatimewidth.clear();

  // Clear digitized tracker data
  digitrackerid.clear();
  digitrackermodule.clear();
  digitrackerside.clear();
  digitrackerlayer.clear();
  digitrackercolumn.clear();
  digitrackeranodetimestampR0.clear();
  digitrackeranodetimestampR1.clear();
  digitrackeranodetimestampR2.clear();
  digitrackeranodetimestampR3.clear();
  digitrackeranodetimestampR4.clear();
  digitrackerbottomcathodetimestamp.clear();
  digitrackertopcathodetimestamp.clear();

  // Clear digitized calorimeter data
  digicaloid.clear();
  digicalotype.clear();
  digicalomodule.clear();
  digicaloside.clear();
  digicalocolumn.clear();
  digicalorow.clear();
  digicalowall.clear();
  digicalotimestamp.clear();
  digicalolto.clear();
  digicaloht.clear();
  digicalofcr.clear();
  digicalolttriggercounter.clear();
  digicaloltimecounter.clear();
  digicalowaveform.clear();
  digicalobaseline.clear();
  digicalopeakamplitude.clear();
  digicalopeakcell.clear();
  digicalocharge.clear();
  digicalorisingcell.clear();
  digicalofallingcell.clear();

  // clear true gg hits
  truetrackerid.clear();
  truetrackermodule.clear();
  truetrackerside.clear();
  truetrackerlayer.clear();
  truetrackercolumn.clear();
  truetrackertime.clear();
  truetrackerxstart.clear();
  truetrackerystart.clear();
  truetrackerzstart.clear();
  truetrackerxstop.clear();
  truetrackerystop.clear();
  truetrackerzstop.clear();
  truetrackertrackid.clear();
  truetrackerparenttrackid.clear();

  // clear true calo hits
  truecaloid.clear();
  truecalotype.clear();
  truecalomodule.clear();
  truecaloside.clear();
  truecalocolumn.clear();
  truecalorow.clear();
  truecalowall.clear();
  truecalox.clear();
  truecaloy.clear();
  truecaloz.clear();
  truecalotime.clear();
  truecaloenergy.clear();

  // clear primary particles
  partid.clear();
  parttype.clear();
  partpx.clear();
  partpy.clear();
  partpz.clear();
  parttime.clear();
  partenergy.clear();
}

// Construct
Things2Root::Things2Root() : dpp::base_module() {
  filename_output_ = "things2root.default.root";
  ws_ = 0;
  hfile_ = 0;
  tree_ = 0;
  geometry_manager_ = 0;
}

// Destruct
Things2Root::~Things2Root() {
  // MUST reset module at destruction
  if (this->is_initialized()) {
    this->Things2Root::reset();
  }
}

// Initialize
void Things2Root::initialize(const datatools::properties& myConfig,
                             datatools::service_manager& flServices,
                             dpp::module_handle_dict_type& /*moduleDict*/) {
  // Throw logic exception if we've already initialized this instance
  DT_THROW_IF(this->is_initialized(), std::logic_error, "Things2Root already initialized");
  // Extract the filename_out key from the supplied config, if
  // the key exists. datatools::properties throws an exception if
  // the key isn't in the config, so catch this if thrown and don't do
  // anything
  try {
    myConfig.fetch("output_file", this->filename_output_);
  } catch (std::logic_error& e) {
  }

  // Look for services
  if (flServices.has("geometry")) {
    const geomtools::geometry_service& GS = flServices.get<geomtools::geometry_service>("geometry");

    // initialize geometry manager
    geometry_manager_ = &GS.get_geom_manager();
    DT_THROW_IF(!geometry_manager_, std::runtime_error,
                "Null pointer to geometry manager return by geometry_service");
  }
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Create TFile...");

  // Next all root file output here
  hfile_ = new TFile(filename_output_.c_str(), "RECREATE", "Output file of Simulation data");
  hfile_->cd();

  tree_ = new TTree("SimData", "SimData");
  // 2014-02-05, F.Mauger: Force affectation of the tree's current directory to
  // explicitly avoid the tree to be reaffcted to another concurrent TFile
  // (output_module & brio format):
  // TO BE CHECKED
  // 2014-02-11, YR, seems to work fine
  tree_->SetDirectory(hfile_);

  // header data
  tree_->Branch("header.runnumber", &header_.runnumber_);
  tree_->Branch("header.eventnumber", &header_.eventnumber_);
  tree_->Branch("header.date", &header_.date_);
  tree_->Branch("header.runtype", &header_.runtype_);
  tree_->Branch("header.simulated", &header_.simulated_);
  tree_->Branch("header.real", &header_.real_);

  // calibrated tracker data
  tree_->Branch("tracker.nohits", &tracker_.nohits_);
  tree_->Branch("tracker.id", &tracker_.id_);
  tree_->Branch("tracker.module", &tracker_.module_);
  tree_->Branch("tracker.side", &tracker_.side_);
  tree_->Branch("tracker.layer", &tracker_.layer_);
  tree_->Branch("tracker.column", &tracker_.column_);
  tree_->Branch("tracker.x", &tracker_.x_);
  tree_->Branch("tracker.y", &tracker_.y_);
  tree_->Branch("tracker.z", &tracker_.z_);
  tree_->Branch("tracker.sigmaz", &tracker_.sigmaz_);
  tree_->Branch("tracker.r", &tracker_.r_);
  tree_->Branch("tracker.sigmar", &tracker_.sigmar_);
  tree_->Branch("tracker.delayedtime", &tracker_.delayed_time_);
  tree_->Branch("tracker.delayedtimeerror", &tracker_.delayed_time_error_);
  tree_->Branch("tracker.isdelayed", &tracker_.is_delayed_);
  tree_->Branch("tracker.isnoisy", &tracker_.is_noisy_);
  tree_->Branch("tracker.isbottomcathodemissing", &tracker_.is_bottom_cathode_missing_);
  tree_->Branch("tracker.istopcathodemissing", &tracker_.is_top_cathode_missing_);
  tree_->Branch("tracker.truehitid", &tracker_.truehitid_);

  // calibrated calorimeter data
  tree_->Branch("calo.nohits", &calo_.nohits_);
  tree_->Branch("calo.id", &calo_.id_);
  tree_->Branch("calo.module", &calo_.module_);
  tree_->Branch("calo.side", &calo_.side_);
  tree_->Branch("calo.column", &calo_.column_);
  tree_->Branch("calo.row", &calo_.row_);
  tree_->Branch("calo.wall", &calo_.wall_);
  tree_->Branch("calo.time", &calo_.time_);
  tree_->Branch("calo.sigmatime", &calo_.sigmatime_);
  tree_->Branch("calo.energy", &calo_.energy_);
  tree_->Branch("calo.sigmaenergy", &calo_.sigmaenergy_);
  tree_->Branch("calo.type", &calo_.type_);

  // precalibrated tracker data
  tree_->Branch("pcdtracker.nohits", &pcdtracker_.nohits_);
  tree_->Branch("pcdtracker.id", &pcdtracker_.id_);
  tree_->Branch("pcdtracker.module", &pcdtracker_.module_);
  tree_->Branch("pcdtracker.side", &pcdtracker_.side_);
  tree_->Branch("pcdtracker.layer", &pcdtracker_.layer_);
  tree_->Branch("pcdtracker.column", &pcdtracker_.column_);
  tree_->Branch("pcdtracker.referencetime", &pcdtracker_.reference_time_);
  tree_->Branch("pcdtracker.anodicdrifttime", &pcdtracker_.anodic_drift_time_);
  tree_->Branch("pcdtracker.sigmaanodicdrifttime", &pcdtracker_.sigma_anodic_drift_time_);
  tree_->Branch("pcdtracker.bottomcathodedrifttime", &pcdtracker_.bottom_cathode_drift_time_);
  tree_->Branch("pcdtracker.sigmabottomcathodedrifttime", &pcdtracker_.sigma_bottom_cathode_drift_time_);
  tree_->Branch("pcdtracker.topcathodedrifttime", &pcdtracker_.top_cathode_drift_time_);
  tree_->Branch("pcdtracker.sigmatopcathodedrifttime", &pcdtracker_.sigma_top_cathode_drift_time_);
  tree_->Branch("pcdtracker.isdelayed", &pcdtracker_.is_delayed_);

  // precalibrated calorimeter data
  tree_->Branch("pcdcalo.nohits", &pcdcalo_.nohits_);
  tree_->Branch("pcdcalo.id", &pcdcalo_.id_);
  tree_->Branch("pcdcalo.type", &pcdcalo_.type_);
  tree_->Branch("pcdcalo.module", &pcdcalo_.module_);
  tree_->Branch("pcdcalo.side", &pcdcalo_.side_);
  tree_->Branch("pcdcalo.column", &pcdcalo_.column_);
  tree_->Branch("pcdcalo.row", &pcdcalo_.row_);
  tree_->Branch("pcdcalo.wall", &pcdcalo_.wall_);
  tree_->Branch("pcdcalo.amplitude", &pcdcalo_.amplitude_);
  tree_->Branch("pcdcalo.sigma_amplitude", &pcdcalo_.sigma_amplitude_);
  tree_->Branch("pcdcalo.charge", &pcdcalo_.charge_);
  tree_->Branch("pcdcalo.sigma_charge", &pcdcalo_.sigma_charge_);
  tree_->Branch("pcdcalo.time", &pcdcalo_.time_);
  tree_->Branch("pcdcalo.sigma_time", &pcdcalo_.sigma_time_);
  tree_->Branch("pcdcalo.baseline", &pcdcalo_.baseline_);
  tree_->Branch("pcdcalo.sigma_baseline", &pcdcalo_.sigma_baseline_);
  tree_->Branch("pcdcalo.rising_time", &pcdcalo_.rising_time_);
  tree_->Branch("pcdcalo.sigma_rising_time", &pcdcalo_.sigma_rising_time_);
  tree_->Branch("pcdcalo.falling_time", &pcdcalo_.falling_time_);
  tree_->Branch("pcdcalo.sigma_falling_time", &pcdcalo_.sigma_falling_time_);
  tree_->Branch("pcdcalo.time_width", &pcdcalo_.time_width_);
  tree_->Branch("pcdcalo.sigma_time_width", &pcdcalo_.sigma_time_width_);

  // digitized tracker data
  tree_->Branch("digitracker.nohits", &digitracker_.nohits_);
  tree_->Branch("digitracker.id", &digitracker_.id_);
  tree_->Branch("digitracker.module", &digitracker_.module_);
  tree_->Branch("digitracker.side", &digitracker_.side_);
  tree_->Branch("digitracker.layer", &digitracker_.layer_);
  tree_->Branch("digitracker.column", &digitracker_.column_);
  tree_->Branch("digitracker.anodetimestampR0", &digitracker_.anode_timestamp_R0_);
  tree_->Branch("digitracker.anodetimestampR1", &digitracker_.anode_timestamp_R1_);
  tree_->Branch("digitracker.anodetimestampR2", &digitracker_.anode_timestamp_R2_);
  tree_->Branch("digitracker.anodetimestampR3", &digitracker_.anode_timestamp_R3_);
  tree_->Branch("digitracker.anodetimestampR4", &digitracker_.anode_timestamp_R4_);
  tree_->Branch("digitracker.bottomcathodetimestamp", &digitracker_.bottom_cathode_timestamp_);
  tree_->Branch("digitracker.topcathodetimestamp",    &digitracker_.top_cathode_timestamp_);

  // digitized calorimeter data
  tree_->Branch("digicalo.nohits", &digicalo_.nohits_);
  tree_->Branch("digicalo.id", &digicalo_.id_);
  tree_->Branch("digicalo.module", &digicalo_.module_);
  tree_->Branch("digicalo.side", &digicalo_.side_);
  tree_->Branch("digicalo.column", &digicalo_.column_);
  tree_->Branch("digicalo.row", &digicalo_.row_);
  tree_->Branch("digicalo.timestamp", &digicalo_.timestamp_);
  tree_->Branch("digicalo.lowthresholdonly", &digicalo_.low_threshold_only_);
  tree_->Branch("digicalo.highthreshold", &digicalo_.high_threshold_);
  tree_->Branch("digicalo.fcr", &digicalo_.fcr_);
  tree_->Branch("digicalo.lttriggercounter", &digicalo_.lt_trigger_counter_);
  tree_->Branch("digicalo.lttimecounter", &digicalo_.lt_time_counter_);
  tree_->Branch("digicalo.waveform", &digicalo_.waveform_);
  tree_->Branch("digicalo.baseline", &digicalo_.baseline_);
  tree_->Branch("digicalo.peakamplitude", &digicalo_.peak_amplitude_);
  tree_->Branch("digicalo.peakcell", &digicalo_.peak_cell_);
  tree_->Branch("digicalo.charge", &digicalo_.charge_);
  tree_->Branch("digicalo.rising_cell", &digicalo_.rising_cell_);
  tree_->Branch("digicalo.falling_cell", &digicalo_.falling_cell_);

  // truth tracker data
  tree_->Branch("truetracker.nohits", &truetracker_.nohits_);
  tree_->Branch("truetracker.id", &truetracker_.id_);
  tree_->Branch("truetracker.module", &truetracker_.module_);
  tree_->Branch("truetracker.side", &truetracker_.side_);
  tree_->Branch("truetracker.layer", &truetracker_.layer_);
  tree_->Branch("truetracker.column", &truetracker_.column_);
  tree_->Branch("truetracker.time", &truetracker_.time_);
  tree_->Branch("truetracker.xstart", &truetracker_.xstart_);
  tree_->Branch("truetracker.ystart", &truetracker_.ystart_);
  tree_->Branch("truetracker.zstart", &truetracker_.zstart_);
  tree_->Branch("truetracker.xstop", &truetracker_.xstop_);
  tree_->Branch("truetracker.ystop", &truetracker_.ystop_);
  tree_->Branch("truetracker.zstop", &truetracker_.zstop_);
  tree_->Branch("truetracker.trackid", &truetracker_.trackid_);
  tree_->Branch("truetracker.parenttrackid", &truetracker_.parenttrackid_);

  // truth calorimeter data
  tree_->Branch("truecalo.nohits", &truecalo_.nohits_);
  tree_->Branch("truecalo.id", &truecalo_.id_);
  tree_->Branch("truecalo.type", &truecalo_.type_);
  tree_->Branch("truecalo.x", &truecalo_.x_);
  tree_->Branch("truecalo.y", &truecalo_.y_);
  tree_->Branch("truecalo.z", &truecalo_.z_);
  tree_->Branch("truecalo.time", &truecalo_.time_);
  tree_->Branch("truecalo.energy", &truecalo_.energy_);
  tree_->Branch("truecalo.module", &truecalo_.module_);
  tree_->Branch("truecalo.side", &truecalo_.side_);
  tree_->Branch("truecalo.wall", &truecalo_.wall_);
  tree_->Branch("truecalo.column", &truecalo_.column_);
  tree_->Branch("truecalo.row", &truecalo_.row_);

  // truth vertex data
  tree_->Branch("truevertex.x", &truevertex_.x_);
  tree_->Branch("truevertex.y", &truevertex_.y_);
  tree_->Branch("truevertex.z", &truevertex_.z_);
  tree_->Branch("truevertex.time", &truevertex_.time_);

  // truth primary particle data
  tree_->Branch("trueparticle.noparticles", &trueparticle_.noparticles_);
  tree_->Branch("trueparticle.id", &trueparticle_.id_);
  tree_->Branch("trueparticle.type", &trueparticle_.type_);
  tree_->Branch("trueparticle.px", &trueparticle_.px_);
  tree_->Branch("trueparticle.py", &trueparticle_.py_);
  tree_->Branch("trueparticle.pz", &trueparticle_.pz_);
  tree_->Branch("trueparticle.time", &trueparticle_.time_);
  tree_->Branch("trueparticle.kinenergy", &trueparticle_.ke_);

  this->_set_initialized(true);
}

// Process
dpp::base_module::process_status Things2Root::process(datatools::things& workItem) {
  // Local variables
  if (!ws_) {
    ws_ = new working_space;
  } else {
    ws_->clear();
  }

  unsigned int calo_geom_type = 1302;
  unsigned int xcalo_geom_type = 1232;
  unsigned int gveto_geom_type = 1252;

  // Access the workItem
  if (workItem.has("SD")) {
    const mctools::simulated_data& SD = workItem.get<mctools::simulated_data>("SD");

    truevertex_.x_ = SD.get_vertex().x();
    truevertex_.y_ = SD.get_vertex().y();
    truevertex_.z_ = SD.get_vertex().z();
    truevertex_.time_ = SD.get_primary_event().get_time();

    int count = 0;
    const genbb::primary_event primev = SD.get_primary_event();
    const std::list<genbb::primary_particle> prcoll = primev.get_particles();
    trueparticle_.noparticles_ = prcoll.size();

    for (std::list<genbb::primary_particle>::const_iterator it = prcoll.begin(); it != prcoll.end();
         ++it) {
      genbb::primary_particle the_particle = *it;
      ws_->partid.push_back(count);
      ws_->parttype.push_back(the_particle.get_type());
      ws_->partpx.push_back(the_particle.get_momentum().x());
      ws_->partpy.push_back(the_particle.get_momentum().y());
      ws_->partpz.push_back(the_particle.get_momentum().z());
      ws_->parttime.push_back(the_particle.get_time());
      ws_->partenergy.push_back(the_particle.get_kinetic_energy());
      count++;
    }

    trueparticle_.id_ = &ws_->partid;
    trueparticle_.type_ = &ws_->parttype;
    trueparticle_.px_ = &ws_->partpx;
    trueparticle_.py_ = &ws_->partpy;
    trueparticle_.pz_ = &ws_->partpz;
    trueparticle_.time_ = &ws_->parttime;
    trueparticle_.ke_ = &ws_->partenergy;

    // tracker truth hits
    if (SD.has_step_hits("gg")) {
      int nggtruehits = SD.get_number_of_step_hits("gg");
      truetracker_.nohits_ = nggtruehits;

      // this needs the geometry manager
      static int gid_gg_module_index = geometry_manager_->get_id_mgr()
        .get_category_info("drift_cell_core")
        .get_subaddress_index("module");

      static int gid_gg_side_index = geometry_manager_->get_id_mgr()
        .get_category_info("drift_cell_core")
        .get_subaddress_index("side");

      static int gid_gg_layer_index = geometry_manager_->get_id_mgr()
        .get_category_info("drift_cell_core")
        .get_subaddress_index("layer");

      static int gid_gg_row_index = geometry_manager_->get_id_mgr()
        .get_category_info("drift_cell_core")
        .get_subaddress_index("row");

      // this is the event loop
      for (int i = 0; i < nggtruehits; ++i) {
        const mctools::base_step_hit& gg_true_hit = SD.get_step_hit("gg", i);
        ws_->truetrackerid.push_back(gg_true_hit.get_hit_id());
        ws_->truetrackermodule.push_back(gg_true_hit.get_geom_id().get(gid_gg_module_index));
        ws_->truetrackerside.push_back(gg_true_hit.get_geom_id().get(gid_gg_side_index));
        ws_->truetrackerlayer.push_back(gg_true_hit.get_geom_id().get(gid_gg_layer_index));
        ws_->truetrackercolumn.push_back(gg_true_hit.get_geom_id().get(gid_gg_row_index));

        ws_->truetrackertime.push_back(gg_true_hit.get_time_start() / CLHEP::ns);
        ws_->truetrackerxstart.push_back(gg_true_hit.get_position_start().x() / CLHEP::mm);
        ws_->truetrackerystart.push_back(gg_true_hit.get_position_start().y() / CLHEP::mm);
        ws_->truetrackerzstart.push_back(gg_true_hit.get_position_start().z() / CLHEP::mm);
        ws_->truetrackerxstop.push_back(gg_true_hit.get_position_stop().x() / CLHEP::mm);
        ws_->truetrackerystop.push_back(gg_true_hit.get_position_stop().y() / CLHEP::mm);
        ws_->truetrackerzstop.push_back(gg_true_hit.get_position_stop().z() / CLHEP::mm);
        ws_->truetrackertrackid.push_back(gg_true_hit.get_track_id());
        ws_->truetrackerparenttrackid.push_back(gg_true_hit.get_parent_track_id());
      }
    }

    truetracker_.id_ = &ws_->truetrackerid;
    truetracker_.module_ = &ws_->truetrackermodule;
    truetracker_.side_ = &ws_->truetrackerside;
    truetracker_.layer_ = &ws_->truetrackerlayer;
    truetracker_.column_ = &ws_->truetrackercolumn;
    truetracker_.time_ = &ws_->truetrackertime;
    truetracker_.xstart_ = &ws_->truetrackerxstart;
    truetracker_.ystart_ = &ws_->truetrackerystart;
    truetracker_.zstart_ = &ws_->truetrackerzstart;
    truetracker_.xstop_ = &ws_->truetrackerxstop;
    truetracker_.ystop_ = &ws_->truetrackerystop;
    truetracker_.zstop_ = &ws_->truetrackerzstop;
    truetracker_.trackid_ = &ws_->truetrackertrackid;
    truetracker_.parenttrackid_ = &ws_->truetrackerparenttrackid;

    // calorimeter truth hits
    truecalo_.nohits_ = 0;

    if (SD.has_step_hits("calorimeter")) {
      truecalo_.nohits_ += SD.get_number_of_step_hits("calorimeter");
      for (int ihit = 0; ihit < truecalo_.nohits_; ihit++) {
        const mctools::base_step_hit& the_scin_hit = SD.get_step_hit("calorimeter", ihit);

        static int gid_calo_module_index = geometry_manager_->get_id_mgr()
          .get_category_info("calorimeter_block")
          .get_subaddress_index("module");
        static int gid_calo_side_index = geometry_manager_->get_id_mgr()
          .get_category_info("calorimeter_block")
          .get_subaddress_index("side");
        static int gid_calo_column_index = geometry_manager_->get_id_mgr()
          .get_category_info("calorimeter_block")
          .get_subaddress_index("column");
        static int gid_calo_row_index = geometry_manager_->get_id_mgr()
          .get_category_info("calorimeter_block")
          .get_subaddress_index("row");

        ws_->truecaloid.push_back(the_scin_hit.get_hit_id());
        ws_->truecalox.push_back(the_scin_hit.get_position_start().x() / CLHEP::cm);
        ws_->truecaloy.push_back(the_scin_hit.get_position_start().y() / CLHEP::cm);
        ws_->truecaloz.push_back(the_scin_hit.get_position_start().z() / CLHEP::cm);
        ws_->truecalotime.push_back(the_scin_hit.get_time_start() / CLHEP::ns);
        ws_->truecaloenergy.push_back(the_scin_hit.get_energy_deposit() / CLHEP::MeV);

        ws_->truecalotype.push_back(0);
        ws_->truecalowall.push_back(0);
        ws_->truecalomodule.push_back(the_scin_hit.get_geom_id().get(gid_calo_module_index));
        ws_->truecaloside.push_back(the_scin_hit.get_geom_id().get(gid_calo_side_index));
        ws_->truecalocolumn.push_back(the_scin_hit.get_geom_id().get(gid_calo_column_index));
        ws_->truecalorow.push_back(the_scin_hit.get_geom_id().get(gid_calo_row_index));
      }
    }

    if (SD.has_step_hits("xcalo")) {
      truecalo_.nohits_ += SD.get_number_of_step_hits("xcalo");
      for (unsigned int ihit = 0; ihit < SD.get_number_of_step_hits("xcalo"); ihit++) {
        const mctools::base_step_hit& the_scin_hit = SD.get_step_hit("xcalo", ihit);

        static int gid_xcalo_module_index = geometry_manager_->get_id_mgr()
          .get_category_info("xcalo_block")
          .get_subaddress_index("module");
        static int gid_xcalo_side_index = geometry_manager_->get_id_mgr()
          .get_category_info("xcalo_block")
          .get_subaddress_index("side");
        static int gid_xcalo_wall_index = geometry_manager_->get_id_mgr()
          .get_category_info("xcalo_block")
          .get_subaddress_index("wall");
        static int gid_xcalo_column_index = geometry_manager_->get_id_mgr()
          .get_category_info("xcalo_block")
          .get_subaddress_index("column");
        static int gid_xcalo_row_index = geometry_manager_->get_id_mgr()
          .get_category_info("xcalo_block")
          .get_subaddress_index("row");
        ws_->truecaloid.push_back(the_scin_hit.get_hit_id());

        ws_->truecalox.push_back(the_scin_hit.get_position_start().x() / CLHEP::cm);
        ws_->truecaloy.push_back(the_scin_hit.get_position_start().y() / CLHEP::cm);
        ws_->truecaloz.push_back(the_scin_hit.get_position_start().z() / CLHEP::cm);
        ws_->truecalotime.push_back(the_scin_hit.get_time_start() / CLHEP::ns);
        ws_->truecaloenergy.push_back(the_scin_hit.get_energy_deposit() / CLHEP::MeV);

        ws_->truecalotype.push_back(1);
        ws_->truecalomodule.push_back(the_scin_hit.get_geom_id().get(gid_xcalo_module_index));
        ws_->truecaloside.push_back(the_scin_hit.get_geom_id().get(gid_xcalo_side_index));
        ws_->truecalowall.push_back(the_scin_hit.get_geom_id().get(gid_xcalo_wall_index));
        ws_->truecalocolumn.push_back(the_scin_hit.get_geom_id().get(gid_xcalo_column_index));
        ws_->truecalorow.push_back(the_scin_hit.get_geom_id().get(gid_xcalo_row_index));
      }
    }

    if (SD.has_step_hits("gveto")) {
      truecalo_.nohits_ += SD.get_number_of_step_hits("gveto");
      for (unsigned int ihit = 0; ihit < SD.get_number_of_step_hits("gveto"); ihit++) {
        const mctools::base_step_hit& the_scin_hit = SD.get_step_hit("gveto", ihit);

        static int gid_gveto_module_index = geometry_manager_->get_id_mgr()
          .get_category_info("gveto_block")
          .get_subaddress_index("module");
        static int gid_gveto_side_index = geometry_manager_->get_id_mgr()
          .get_category_info("gveto_block")
          .get_subaddress_index("side");
        static int gid_gveto_wall_index = geometry_manager_->get_id_mgr()
          .get_category_info("gveto_block")
          .get_subaddress_index("wall");
        static int gid_gveto_column_index = geometry_manager_->get_id_mgr()
          .get_category_info("gveto_block")
          .get_subaddress_index("column");

        ws_->truecaloid.push_back(the_scin_hit.get_hit_id());
        ws_->truecalox.push_back(the_scin_hit.get_position_start().x() / CLHEP::cm);
        ws_->truecaloy.push_back(the_scin_hit.get_position_start().y() / CLHEP::cm);
        ws_->truecaloz.push_back(the_scin_hit.get_position_start().z() / CLHEP::cm);
        ws_->truecalotime.push_back(the_scin_hit.get_time_start() / CLHEP::ns);
        ws_->truecaloenergy.push_back(the_scin_hit.get_energy_deposit() / CLHEP::MeV);

        ws_->truecalotype.push_back(2);
        ws_->truecalorow.push_back(0);
        ws_->truecalomodule.push_back(the_scin_hit.get_geom_id().get(gid_gveto_module_index));
        ws_->truecaloside.push_back(the_scin_hit.get_geom_id().get(gid_gveto_side_index));
        ws_->truecalowall.push_back(the_scin_hit.get_geom_id().get(gid_gveto_wall_index));
        ws_->truecalocolumn.push_back(the_scin_hit.get_geom_id().get(gid_gveto_column_index));
      }
    }

    truecalo_.id_ = &ws_->truecaloid;
    truecalo_.type_ = &ws_->truecalotype;
    truecalo_.module_ = &ws_->truecalomodule;
    truecalo_.side_ = &ws_->truecaloside;
    truecalo_.column_ = &ws_->truecalocolumn;
    truecalo_.row_ = &ws_->truecalorow;
    truecalo_.wall_ = &ws_->truecalowall;
    truecalo_.x_ = &ws_->truecalox;
    truecalo_.y_ = &ws_->truecaloy;
    truecalo_.z_ = &ws_->truecaloz;
    truecalo_.time_ = &ws_->truecalotime;
    truecalo_.energy_ = &ws_->truecaloenergy;
  }


  // look for unified digitized data
  if (workItem.has("UDD")) {
    const snemo::datamodel::unified_digitized_data& UDD =
      workItem.get<snemo::datamodel::unified_digitized_data>("UDD");
    // std::clog << "In process: found UDD data bank " << std::endl;
    digitracker_.nohits_ = UDD.get_tracker_hits().size();
    BOOST_FOREACH (const snemo::datamodel::TrackerDigiHitHdl& gg_handle, UDD.get_tracker_hits()) {
      if (!gg_handle.has_data()) continue;

      const snemo::datamodel::tracker_digitized_hit& digi_gg_hit = gg_handle.get();
      ws_->digitrackerid.push_back(digi_gg_hit.get_hit_id());
      ws_->digitrackermodule.push_back(digi_gg_hit.get_geom_id().get(0));
      ws_->digitrackerside.push_back(digi_gg_hit.get_geom_id().get(1));
      ws_->digitrackerlayer.push_back(digi_gg_hit.get_geom_id().get(2));
      ws_->digitrackercolumn.push_back(digi_gg_hit.get_geom_id().get(3));

      std::vector<long int> temp_digi_tracker_anode_R0;
      std::vector<long int> temp_digi_tracker_anode_R1;
      std::vector<long int> temp_digi_tracker_anode_R2;
      std::vector<long int> temp_digi_tracker_anode_R3;
      std::vector<long int> temp_digi_tracker_anode_R4;
      std::vector<long int> temp_digi_tracker_bottom_cathode;
      std::vector<long int> temp_digi_tracker_top_cathode;

      for (std::size_t iggtime = 0; iggtime < digi_gg_hit.get_times().size(); iggtime++) {
        const snemo::datamodel::tracker_digitized_hit::gg_times a_gg_time = digi_gg_hit.get_times()[iggtime];
        temp_digi_tracker_anode_R0.push_back(a_gg_time.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R0));
        temp_digi_tracker_anode_R1.push_back(a_gg_time.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R1));
        temp_digi_tracker_anode_R2.push_back(a_gg_time.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R2));
        temp_digi_tracker_anode_R3.push_back(a_gg_time.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R3));
        temp_digi_tracker_anode_R4.push_back(a_gg_time.get_anode_time(snemo::datamodel::tracker_digitized_hit::ANODE_R4));
        temp_digi_tracker_bottom_cathode.push_back(a_gg_time.get_bottom_cathode_time());
        temp_digi_tracker_top_cathode.push_back(a_gg_time.get_top_cathode_time());

      } // end iggtime

      ws_->digitrackeranodetimestampR0.push_back(temp_digi_tracker_anode_R0);
      ws_->digitrackeranodetimestampR1.push_back(temp_digi_tracker_anode_R1);
      ws_->digitrackeranodetimestampR2.push_back(temp_digi_tracker_anode_R2);
      ws_->digitrackeranodetimestampR3.push_back(temp_digi_tracker_anode_R3);
      ws_->digitrackeranodetimestampR4.push_back(temp_digi_tracker_anode_R4);
      ws_->digitrackerbottomcathodetimestamp.push_back(temp_digi_tracker_bottom_cathode);
      ws_->digitrackertopcathodetimestamp.push_back(temp_digi_tracker_top_cathode);

    } // end of digi tracker hit

    digitracker_.id_ = &ws_->digitrackerid;
    digitracker_.module_ = &ws_->digitrackermodule;
    digitracker_.side_ = &ws_->digitrackerside;
    digitracker_.layer_ = &ws_->digitrackerlayer;
    digitracker_.column_ = &ws_->digitrackercolumn;
    digitracker_.anode_timestamp_R0_ = &ws_->digitrackeranodetimestampR0;
    digitracker_.anode_timestamp_R1_ = &ws_->digitrackeranodetimestampR1;
    digitracker_.anode_timestamp_R2_ = &ws_->digitrackeranodetimestampR2;
    digitracker_.anode_timestamp_R3_ = &ws_->digitrackeranodetimestampR3;
    digitracker_.anode_timestamp_R4_ = &ws_->digitrackeranodetimestampR4;
    digitracker_.bottom_cathode_timestamp_ = &ws_->digitrackerbottomcathodetimestamp;
    digitracker_.top_cathode_timestamp_ = &ws_->digitrackertopcathodetimestamp;


    digicalo_.nohits_ = UDD.get_calorimeter_hits().size();
    BOOST_FOREACH (const snemo::datamodel::CalorimeterDigiHitHdl& calo_handle, UDD.get_calorimeter_hits()) {
      if (!calo_handle.has_data()) continue;

      const snemo::datamodel::calorimeter_digitized_hit& digi_calo_hit = calo_handle.get();

      ws_->digicaloid.push_back(digi_calo_hit.get_hit_id());
      ws_->digicalomodule.push_back(digi_calo_hit.get_geom_id().get(0));
      ws_->digicaloside.push_back(digi_calo_hit.get_geom_id().get(1));

      if (digi_calo_hit.get_geom_id().get_type() == calo_geom_type) {
        // CALO
        ws_->digicalowall.push_back(0);
        ws_->digicalocolumn.push_back(digi_calo_hit.get_geom_id().get(2));
        ws_->digicalorow.push_back(digi_calo_hit.get_geom_id().get(3));
        ws_->digicalotype.push_back(0);
      }
      if (digi_calo_hit.get_geom_id().get_type() == xcalo_geom_type) {
        // XCALO
        ws_->digicalowall.push_back(digi_calo_hit.get_geom_id().get(2));
        ws_->digicalocolumn.push_back(digi_calo_hit.get_geom_id().get(3));
        ws_->digicalorow.push_back(digi_calo_hit.get_geom_id().get(0));
        ws_->digicalotype.push_back(1);
      }
      if (digi_calo_hit.get_geom_id().get_type() == gveto_geom_type) {
        // GVETO
        ws_->digicalowall.push_back(digi_calo_hit.get_geom_id().get(2));
        ws_->digicalocolumn.push_back(digi_calo_hit.get_geom_id().get(3));
        ws_->digicalorow.push_back(0);
        ws_->digicalotype.push_back(2);
      }

      ws_->digicalotimestamp.push_back(digi_calo_hit.get_timestamp());
      ws_->digicalolto.push_back(digi_calo_hit.is_low_threshold_only());
      ws_->digicaloht.push_back(digi_calo_hit.is_high_threshold());
      ws_->digicalofcr.push_back(digi_calo_hit.get_fcr());
      ws_->digicalolttriggercounter.push_back(digi_calo_hit.get_lt_trigger_counter());
      ws_->digicaloltimecounter.push_back(digi_calo_hit.get_lt_time_counter());
      ws_->digicalowaveform.push_back(digi_calo_hit.get_waveform());
      ws_->digicalobaseline.push_back(digi_calo_hit.get_fwmeas_baseline());
      ws_->digicalopeakamplitude.push_back(digi_calo_hit.get_fwmeas_peak_amplitude());
      ws_->digicalopeakcell.push_back(digi_calo_hit.get_fwmeas_peak_cell());
      ws_->digicalocharge.push_back(digi_calo_hit.get_fwmeas_charge());
      ws_->digicalorisingcell.push_back(digi_calo_hit.get_fwmeas_rising_cell());
      ws_->digicalofallingcell.push_back(digi_calo_hit.get_fwmeas_falling_cell());

    } // end of digi calo hit

    digicalo_.id_     = &ws_->digicaloid;
    digicalo_.type_   = &ws_->digicalotype;
    digicalo_.module_ = &ws_->digicalomodule;
    digicalo_.side_   = &ws_->digicaloside;
    digicalo_.column_ = &ws_->digicalocolumn;
    digicalo_.row_    = &ws_->digicalorow;
    digicalo_.wall_   = &ws_->digicalowall;
    digicalo_.timestamp_          = &ws_->digicalotimestamp;
    digicalo_.low_threshold_only_ = &ws_-> digicalolto;
    digicalo_.high_threshold_     = &ws_-> digicaloht;
    digicalo_.fcr_                = &ws_-> digicalofcr;
    digicalo_.lt_trigger_counter_ = &ws_-> digicalolttriggercounter;
    digicalo_.lt_time_counter_    = &ws_-> digicaloltimecounter;
    digicalo_.waveform_           = &ws_-> digicalowaveform;
    digicalo_.baseline_           = &ws_-> digicalobaseline;
    digicalo_.peak_amplitude_     = &ws_-> digicalopeakamplitude;
    digicalo_.peak_cell_          = &ws_-> digicalopeakcell;
    digicalo_.charge_             = &ws_-> digicalocharge;
    digicalo_.rising_cell_        = &ws_-> digicalorisingcell;
    digicalo_.falling_cell_       = &ws_-> digicalofallingcell;

  } // end of workitem UDD


  // look for precalibrated data
  if (workItem.has("pCD")) {
    const snemo::datamodel::precalibrated_data& pCD = workItem.get<snemo::datamodel::precalibrated_data>("pCD");

    pcdtracker_.nohits_ = pCD.tracker_hits().size();
    BOOST_FOREACH (const snemo::datamodel::PreCalibTrackerHitHdl& gg_handle, pCD.tracker_hits()) {
      if (!gg_handle.has_data()) continue;

      const snemo::datamodel::precalibrated_tracker_hit& pcd_gg_hit = gg_handle.get();

      ws_->pcdtrackerid.push_back(pcd_gg_hit.get_hit_id());
      ws_->pcdtrackermodule.push_back(pcd_gg_hit.get_geom_id().get(0));
      ws_->pcdtrackerside.push_back(pcd_gg_hit.get_geom_id().get(1));
      ws_->pcdtrackerlayer.push_back(pcd_gg_hit.get_geom_id().get(2));
      ws_->pcdtrackercolumn.push_back(pcd_gg_hit.get_geom_id().get(3));
      ws_->pcdtrackerreferencetime.push_back(pcd_gg_hit.get_reference_time());
      ws_->pcdtrackeranodicdrifttime.push_back(pcd_gg_hit.get_anodic_drift_time());
      ws_->pcdtrackersigmaanodicdrifttime.push_back(pcd_gg_hit.get_sigma_anodic_drift_time());
      ws_->pcdtrackerbottomcathodedrifttime.push_back(pcd_gg_hit.get_bottom_cathode_drift_time());
      ws_->pcdtrackersigmabottomcathodedrifttime.push_back(pcd_gg_hit.get_sigma_bottom_cathode_drift_time());
      ws_->pcdtrackertopcathodedrifttime.push_back(pcd_gg_hit.get_top_cathode_drift_time());
      ws_->pcdtrackersigmatopcathodedrifttime.push_back(pcd_gg_hit.get_sigma_top_cathode_drift_time());
      ws_->pcdtrackerisdelayed.push_back(pcd_gg_hit.is_delayed());

    }

    pcdtracker_.id_     = &ws_->pcdtrackerid;
    pcdtracker_.module_ = &ws_->pcdtrackermodule;
    pcdtracker_.side_   = &ws_->pcdtrackerside;
    pcdtracker_.layer_  = &ws_->pcdtrackerlayer;
    pcdtracker_.column_ = &ws_->pcdtrackercolumn;
    pcdtracker_.reference_time_ = &ws_->pcdtrackerreferencetime;
    pcdtracker_.anodic_drift_time_ = &ws_->pcdtrackeranodicdrifttime;
    pcdtracker_.sigma_anodic_drift_time_ = &ws_->pcdtrackersigmaanodicdrifttime;
    pcdtracker_.bottom_cathode_drift_time_ = &ws_->pcdtrackerbottomcathodedrifttime;
    pcdtracker_.sigma_bottom_cathode_drift_time_ = &ws_->pcdtrackersigmabottomcathodedrifttime;
    pcdtracker_.top_cathode_drift_time_ = &ws_->pcdtrackertopcathodedrifttime;
    pcdtracker_.sigma_top_cathode_drift_time_ = &ws_->pcdtrackersigmatopcathodedrifttime;
    pcdtracker_.is_delayed_ = &ws_->pcdtrackerisdelayed;


    pcdcalo_.nohits_ = pCD.calorimeter_hits().size();
    BOOST_FOREACH (const snemo::datamodel::PreCalibCalorimeterHitHdl& calo_handle, pCD.calorimeter_hits()) {
      if (!calo_handle.has_data()) continue;

      const snemo::datamodel::precalibrated_calorimeter_hit& pcd_calo_hit = calo_handle.get();

      ws_->pcdcaloid.push_back(pcd_calo_hit.get_hit_id());
      ws_->pcdcalomodule.push_back(pcd_calo_hit.get_geom_id().get(0));
      ws_->pcdcaloside.push_back(pcd_calo_hit.get_geom_id().get(1));

      if (pcd_calo_hit.get_geom_id().get_type() == calo_geom_type) {
        // CALO
        ws_->pcdcalowall.push_back(0);
        ws_->pcdcalocolumn.push_back(pcd_hit.get_geom_id().get(2));
        ws_->pcdcalorow.push_back(pcd_hit.get_geom_id().get(3));
        ws_->pcdcalotype.push_back(0);
      }
      if (pcd_calo_hit.get_geom_id().get_type() == xcalo_geom_type) {
        // XCALO
        ws_->pcdcalowall.push_back(pcd_hit.get_geom_id().get(2));
        ws_->pcdcalocolumn.push_back(pcd_hit.get_geom_id().get(3));
        ws_->pcdcalorow.push_back(pcd_hit.get_geom_id().get(0));
        ws_->pcdcalotype.push_back(1);
      }
      if (pcd_calo_hit.get_geom_id().get_type() == gveto_geom_type) {
        // GVETO
        ws_->pcdcalowall.push_back(pcd_calo_hit.get_geom_id().get(2));
        ws_->pcdcalocolumn.push_back(pcd_calo_hit.get_geom_id().get(3));
        ws_->pcdcalorow.push_back(0);
        ws_->pcdcalotype.push_back(2);
      }

      ws_.>pcdcaloamplitude.push_back(pcd_calo_hit.get_amplitude());
      ws_.>pcdcalosigmaamplitude.push_back(pcd_calo_hit.get_sigma_amplitude());
      ws_.>pcdcalocharge.push_back(pcd_calo_hit.get_charge());
      ws_.>pcdcalosigmacharge.push_back(pcd_calo_hit.get_sigma_charge());
      ws_.>pcdcalotime.push_back(pcd_calo_hit.get_time());
      ws_.>pcdcalosigmatime.push_back(pcd_calo_hit.get_sigma_time());
      ws_.>pcdcalobaseline.push_back(pcd_calo_hit.get_baseline());
      ws_.>pcdcalosigmabaseline.push_back(pcd_calo_hit.get_sigma_baseline());
      ws_.>pcdcalorisingtime.push_back(pcd_calo_hit.get_rising_time());
      ws_.>pcdcalosigmarisingtime.push_back(pcd_calo_hit.get_sigma_rising_time());
      ws_.>pcdcalofallingtime.push_back(pcd_calo_hit.get_falling_time());
      ws_.>pcdcalosigmafallingtime.push_back(pcd_calo_hit.get_sigma_falling_time());
      ws_.>pcdcalotimewidth.push_back(pcd_calo_hit.get_time_width());
      ws_.>pcdcalosigmatimewidth.push_back(pcd_calo_hit.get_sigma_time_width());
    }

    pcdcalo_.id_ = &ws_->pcdcaloid;
    pcdcalo_.type_ = &ws_->pcdcalotype;
    pcdcalo_.module_ = &ws_->pcdcalomodule;
    pcdcalo_.side_ = &ws_->pcdcaloside;
    pcdcalo_.column_ = &ws_->pcdcalocolumn;
    pcdcalo_.row_ = &ws_->pcdcalorow;
    pcdcalo_.wall_ = &ws_->pcdcalowall;
    pcdcalo_.amplitude_ = &ws_.>pcdcaloamplitude;
    pcdcalo_.sigma_amplitude_ = &ws_.>pcdcalosigmaamplitude;
    pcdcalo_.charge_ = &ws_.>pcdcalocharge;
    pcdcalo_.sigma_charge_ = &ws_.>pcdcalosigmacharge;
    pcdcalo_.time_ = &ws_.>pcdcalotime;
    pcdcalo_.sigma_time_ = &ws_.>pcdcalosigmatime;
    pcdcalo_.baseline_ = &ws_.>pcdcalobaseline;
    pcdcalo_.sigma_baseline_ = &ws_.>pcdcalosigmabaseline;
    pcdcalo_.rising_time_ = &ws_.>pcdcalorisingtime;
    pcdcalo_.sigma_rising_time_ = &ws_.>pcdcalosigmarisingtime;
    pcdcalo_.falling_time_ = &ws_.>pcdcalofallingtime;
    pcdcalo_.sigma_falling_time_ = &ws_.>pcdcalosigmafallingtime;
    pcdcalo_.time_width_ = &ws_.>pcdcalotimewidth;
    pcdcalo_.sigma_time_width_ = &ws_.>pcdcalosigmatimewidth;

  }


  // look for calibrated data
  if (workItem.has("CD")) {
    const snemo::datamodel::calibrated_data& CD =
      workItem.get<snemo::datamodel::calibrated_data>("CD");
    //      std::clog << "In process: found CD data bank " << std::endl;
    tracker_.nohits_ = CD.tracker_hits().size();
    BOOST_FOREACH (const snemo::datamodel::TrackerHitHdl& gg_handle, CD.tracker_hits()) {
      if (!gg_handle.has_data()) continue;

      const snemo::datamodel::calibrated_tracker_hit& sncore_gg_hit = gg_handle.get();

      ws_->trackerid.push_back(sncore_gg_hit.get_hit_id());
      ws_->trackermodule.push_back(sncore_gg_hit.get_geom_id().get(0));
      ws_->trackerside.push_back(sncore_gg_hit.get_geom_id().get(1));
      ws_->trackerlayer.push_back(sncore_gg_hit.get_geom_id().get(2));
      ws_->trackercolumn.push_back(sncore_gg_hit.get_geom_id().get(3));
      ws_->trackerx.push_back(sncore_gg_hit.get_x());
      ws_->trackery.push_back(sncore_gg_hit.get_y());
      ws_->trackerz.push_back(sncore_gg_hit.get_z());
      ws_->trackersigmaz.push_back(sncore_gg_hit.get_sigma_z());
      ws_->trackerr.push_back(sncore_gg_hit.get_r());
      ws_->trackersigmar.push_back(sncore_gg_hit.get_sigma_r());
      ws_->trackerdelayedtime.push_back(sncore_gg_hit.get_delayed_time());
      ws_->trackerdelayedtimeerror.push_back(sncore_gg_hit.get_delayed_time_error());
      ws_->isdelayed.push_back(sncore_gg_hit.is_delayed());
      ws_->isnoisy.push_back(sncore_gg_hit.is_noisy());
      ws_->isbottomcathodemissing.push_back(sncore_gg_hit.is_bottom_cathode_missing());
      ws_->istopcathodemissing.push_back(sncore_gg_hit.is_top_cathode_missing());
      ws_->trackertruehitid.push_back(sncore_gg_hit.get_id());

      // special infos about truth tracks:
      int truth_track_id = -1;
      if (sncore_gg_hit.get_auxiliaries().has_key(mctools::track_utils::TRACK_ID_KEY)) {
        truth_track_id =
          sncore_gg_hit.get_auxiliaries().fetch_integer(mctools::track_utils::TRACK_ID_KEY);
      }
      ws_->trackertruetrackid.push_back(truth_track_id);
      int truth_parent_track_id = -1;
      if (sncore_gg_hit.get_auxiliaries().has_key(mctools::track_utils::PARENT_TRACK_ID_KEY)) {
        truth_parent_track_id = sncore_gg_hit.get_auxiliaries().fetch_integer(
                                                                              mctools::track_utils::PARENT_TRACK_ID_KEY);
      }
      ws_->trackertrueparenttrackid.push_back(truth_parent_track_id);
    }
    tracker_.id_ = &ws_->trackerid;
    tracker_.module_ = &ws_->trackermodule;
    tracker_.side_ = &ws_->trackerside;
    tracker_.layer_ = &ws_->trackerlayer;
    tracker_.column_ = &ws_->trackercolumn;
    tracker_.x_ = &ws_->trackerx;
    tracker_.y_ = &ws_->trackery;
    tracker_.z_ = &ws_->trackerz;
    tracker_.sigmaz_ = &ws_->trackersigmaz;
    tracker_.r_ = &ws_->trackerr;
    tracker_.sigmar_ = &ws_->trackersigmar;
    tracker_.delayed_time_ = &ws_->trackerdelayedtime;
    tracker_.delayed_time_error_ = &ws_->trackerdelayedtimeerror;
    tracker_.is_delayed_ = &ws_->isdelayed;
    tracker_.is_noisy_ = &ws_->isnoisy;
    tracker_.is_bottom_cathode_missing_ = &ws_->isbottomcathodemissing;
    tracker_.is_top_cathode_missing_ = &ws_->istopcathodemissing;
    tracker_.truehitid_ = &ws_->trackertruehitid;
    tracker_.truetrackid_ = &ws_->trackertruetrackid;
    tracker_.trueparenttrackid_ = &ws_->trackertrueparenttrackid;

    calo_.nohits_ = CD.calorimeter_hits().size();
    BOOST_FOREACH (const snemo::datamodel::CalorimeterHitHdl& calo_handle, CD.calorimeter_hits()) {
      if (!calo_handle.has_data()) continue;

      const snemo::datamodel::calibrated_calorimeter_hit& the_calo_hit = calo_handle.get();

      ws_->caloid.push_back(the_calo_hit.get_hit_id());
      ws_->calomodule.push_back(the_calo_hit.get_geom_id().get(0));
      ws_->caloside.push_back(the_calo_hit.get_geom_id().get(1));

      if (the_calo_hit.get_geom_id().get_type() == calo_geom_type) {
        // CALO
        ws_->calowall.push_back(0);
        ws_->calocolumn.push_back(the_calo_hit.get_geom_id().get(2));
        ws_->calorow.push_back(the_calo_hit.get_geom_id().get(3));
        ws_->calotype.push_back(0);
      }
      if (the_calo_hit.get_geom_id().get_type() == xcalo_geom_type) {
        // XCALO
        ws_->calowall.push_back(the_calo_hit.get_geom_id().get(2));
        ws_->calocolumn.push_back(the_calo_hit.get_geom_id().get(3));
        ws_->calorow.push_back(the_calo_hit.get_geom_id().get(0));
        ws_->calotype.push_back(1);
      }
      if (the_calo_hit.get_geom_id().get_type() == gveto_geom_type) {
        // GVETO
        ws_->calowall.push_back(the_calo_hit.get_geom_id().get(2));
        ws_->calocolumn.push_back(the_calo_hit.get_geom_id().get(3));
        ws_->calorow.push_back(0);
        ws_->calotype.push_back(2);
      }

      ws_->calotime.push_back(the_calo_hit.get_time());
      ws_->calosigmatime.push_back(the_calo_hit.get_sigma_time());
      ws_->caloenergy.push_back(the_calo_hit.get_energy());
      ws_->calosigmaenergy.push_back(the_calo_hit.get_sigma_energy());
    }
    calo_.id_ = &ws_->caloid;
    calo_.type_ = &ws_->calotype;
    calo_.module_ = &ws_->calomodule;
    calo_.side_ = &ws_->caloside;
    calo_.column_ = &ws_->calocolumn;
    calo_.row_ = &ws_->calorow;
    calo_.wall_ = &ws_->calowall;
    calo_.time_ = &ws_->calotime;
    calo_.sigmatime_ = &ws_->calosigmatime;
    calo_.energy_ = &ws_->caloenergy;
    calo_.sigmaenergy_ = &ws_->calosigmaenergy;
  }

  // Look for event header
  if (workItem.has("EH")) {
    const snemo::datamodel::event_header& EH = workItem.get<snemo::datamodel::event_header>("EH");
    //      std::clog << "In process: found EH event header " << std::endl;
    header_.runnumber_ = EH.get_id().get_run_number();
    header_.eventnumber_ = EH.get_id().get_event_number();
    header_.date_ = 0;
    header_.runtype_ = 0;
    header_.simulated_ = (EH.is_simulated() ? true : false);
    header_.real_ = (EH.is_real() ? true : false);
  }

  tree_->Fill();

  // MUST return a status, see ref dpp::processing_status_flags_type
  return dpp::base_module::PROCESS_OK;
}

// Reset
void Things2Root::reset() {
  // Throw logic exception if we've not initialized this instance
  DT_THROW_IF(!this->is_initialized(), std::logic_error, "Things2Root not initialized");
  this->_set_initialized(false);
  if (ws_) {
    delete ws_;
    ws_ = 0;
  }
  if (hfile_) {
    // write the output, finished streaming
    hfile_->cd();
    tree_->Write();
    hfile_->Close();  //
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Finished conversion, file closed.");

    // clean up
    delete hfile_;
    tree_ = 0;
    hfile_ = 0;
  }
  geometry_manager_ = 0;
  filename_output_ = "things2root.default.root";
}
