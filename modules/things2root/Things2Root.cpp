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
  // calibrated tracker data
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

  // calibrated calorimeter data
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
  // clear calibrated tracker data
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
  trackertrueparenttrackid.clear()  ;

  // clear calibrated calorimeter data
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

  // look for calibrated data
  if (workItem.has("CD")) {
    // Geometry categories for the scintillator blocks:
    unsigned int calo_geom_type = 1302;
    unsigned int xcalo_geom_type = 1232;
    unsigned int gveto_geom_type = 1252;

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
    BOOST_FOREACH (const snemo::datamodel::CalorimeterHitHdl& the_calo_hit_handle,
                   CD.calorimeter_hits()) {
      if (!the_calo_hit_handle.has_data()) continue;

      const snemo::datamodel::calibrated_calorimeter_hit& the_calo_hit = the_calo_hit_handle.get();

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
  // look for event header
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
