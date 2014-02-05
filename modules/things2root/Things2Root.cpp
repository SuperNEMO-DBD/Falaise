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
// - A
// This Project
// Macro which automatically implements the interface needed
// to enable the module to be loaded at runtime
// The first argument is the typename
// The second is the string key used to access the module in pipeline
// scripts. This must be globally unique.

DPP_MODULE_REGISTRATION_IMPLEMENT(Things2Root,"Things2Root");

// Construct
Things2Root::Things2Root() : dpp::base_module() {
  filename_output_ = "things2root.default.root";
}

// Destruct
Things2Root::~Things2Root() {
  // MUST reset module at destruction
  this->reset();
}

// Initialize
void Things2Root::initialize(const datatools::properties& myConfig,
                                               datatools::service_manager& flServices,
                                               dpp::module_handle_dict_type& /*moduleDict*/) {
  // Throw logic exception if we've already initialized this instance
  DT_THROW_IF(this->is_initialized(),
              std::logic_error,
              "Things2Root already initialized");
  // Extract the filename_out key from the supplied config, if
  // the key exists. datatools::properties throws an exception if
  // the key isn't in the config, so catch this if thrown and don't do
  // anything
  try {
    myConfig.fetch("output_file",this->filename_output_);
  } catch (std::logic_error& e) {
  }

  // Look for services
  if (flServices.has("geometry")) {
    const geomtools::geometry_service& GS = flServices.get<geomtools::geometry_service>("geometry");

    // initialize geometry manager
    geometry_manager_ = &GS.get_geom_manager();
    DT_THROW_IF(!geometry_manager_,
                std::runtime_error,
                "Null pointer to geometry manager return by geometry_service");
  }

  // Next all root file output here
  hfile_ = new TFile(filename_output_.c_str(),"RECREATE","Output file of Simulation data");
  hfile_->cd();

  tree_ = new TTree("SimData", "SimData");
  // 2014-02-05, F.Mauger: Force affectation of the tree's current directory to
  // explicitly avoid the tree to be reaffcted to another concurrent TFile
  // (output_module & brio format):
  // TO BE CHECKED
  /* tree_->SetDirectory(hfile_); */
  tree_->Branch("truetracker.nohits",&truetracker_.nohits_);
  tree_->Branch("truetracker.id",&truetracker_.id_);
  tree_->Branch("truetracker.module",&truetracker_.module_);
  tree_->Branch("truetracker.side",&truetracker_.side_);
  tree_->Branch("truetracker.layer",&truetracker_.layer_);
  tree_->Branch("truetracker.column",&truetracker_.column_);
  tree_->Branch("truetracker.time",&truetracker_.time_);
  tree_->Branch("truetracker.xstart",&truetracker_.xstart_);
  tree_->Branch("truetracker.ystart",&truetracker_.ystart_);
  tree_->Branch("truetracker.zstart",&truetracker_.zstart_);
  tree_->Branch("truetracker.xstop",&truetracker_.xstop_);
  tree_->Branch("truetracker.ystop",&truetracker_.ystop_);
  tree_->Branch("truetracker.zstop",&truetracker_.zstop_);
  tree_->Branch("truetracker.trackid",&truetracker_.trackid_);
  tree_->Branch("truetracker.parenttrackid",&truetracker_.parenttrackid_);

  tree_->Branch("truevertex.x",&truevertex_.x_);
  tree_->Branch("truevertex.y",&truevertex_.y_);
  tree_->Branch("truevertex.z",&truevertex_.z_);
  tree_->Branch("truevertex.time",&truevertex_.time_);

  tree_->Branch("trueparticle.noparticles",&trueparticle_.noparticles_);
  tree_->Branch("trueparticle.id",&trueparticle_.id_);
  tree_->Branch("trueparticle.type",&trueparticle_.type_);
  tree_->Branch("trueparticle.px",&trueparticle_.px_);
  tree_->Branch("trueparticle.py",&trueparticle_.py_);
  tree_->Branch("trueparticle.pz",&trueparticle_.pz_);
  tree_->Branch("trueparticle.time",&trueparticle_.time_);
  tree_->Branch("trueparticle.kinenergy",&trueparticle_.ke_);

  this->_set_initialized(true);
}

// Process
dpp::base_module::process_status Things2Root::process(datatools::things& workItem) {
  // Local variables

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

  // for primary particles
  std::vector<int> partid;
  std::vector<int> parttype;
  std::vector<double> partpx;
  std::vector<double> partpy;
  std::vector<double> partpz;
  std::vector<double> parttime;
  std::vector<double> partenergy;

  // Access the workItem
  if(workItem.has("SD"))
    {
      const mctools::simulated_data & SD = workItem.get<mctools::simulated_data>("SD");

      truevertex_.x_ = SD.get_vertex().x();
      truevertex_.y_ = SD.get_vertex().y();
      truevertex_.z_ = SD.get_vertex().z();
      truevertex_.time_ = SD.get_primary_event ().get_time ();

      int count = 0;
      const genbb::primary_event primev = SD.get_primary_event();
      const std::list<genbb::primary_particle> prcoll = primev.get_particles();
      trueparticle_.noparticles_ = prcoll.size();

      for (std::list<genbb::primary_particle>::const_iterator it = prcoll.begin() ; it != prcoll.end(); ++it) {
        genbb::primary_particle the_particle = *it;
        partid.push_back(count);
        parttype.push_back(the_particle.get_type());
        partpx.push_back(the_particle.get_momentum().x());
        partpy.push_back(the_particle.get_momentum().y());
        partpz.push_back(the_particle.get_momentum().z());
        parttime.push_back(the_particle.get_time());
        partenergy.push_back(the_particle.get_kinetic_energy());
        count++;
      }

      trueparticle_.id_ = &partid;
      trueparticle_.type_ = &parttype;
      trueparticle_.px_ = &partpx;
      trueparticle_.py_ = &partpy;
      trueparticle_.pz_ = &partpz;
      trueparticle_.time_ = &parttime;
      trueparticle_.ke_ = &partenergy;

      if (SD.has_step_hits("gg")) {
        int nggtruehits = SD.get_number_of_step_hits("gg");
        truetracker_.nohits_ = nggtruehits;

        // this needs the geometry manager
        static int gid_gg_module_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("module");

        static int gid_gg_side_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("side");

        static int gid_gg_layer_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("layer");

        static int gid_gg_row_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("row");

        // this is the event loop
        for (int i = 0; i < nggtruehits; ++i) {
          const mctools::base_step_hit& gg_true_hit = SD.get_step_hit("gg",i);
          truetrackerid.push_back(gg_true_hit.get_hit_id());
          truetrackermodule.push_back(gg_true_hit.get_geom_id().get(gid_gg_module_index));
          truetrackerside.push_back(gg_true_hit.get_geom_id().get(gid_gg_side_index));
          truetrackerlayer.push_back(gg_true_hit.get_geom_id().get (gid_gg_layer_index));
          truetrackercolumn.push_back(gg_true_hit.get_geom_id().get (gid_gg_row_index));

          truetrackertime.push_back(gg_true_hit.get_time_start() / CLHEP::ns);
          truetrackerxstart.push_back(gg_true_hit.get_position_start().x() / CLHEP::mm);
          truetrackerystart.push_back(gg_true_hit.get_position_start().y() / CLHEP::mm);
          truetrackerzstart.push_back(gg_true_hit.get_position_start().z() / CLHEP::mm);
          truetrackerxstop.push_back(gg_true_hit.get_position_stop().x() / CLHEP::mm);
          truetrackerystop.push_back(gg_true_hit.get_position_stop().y() / CLHEP::mm);
          truetrackerzstop.push_back(gg_true_hit.get_position_stop().z() / CLHEP::mm);
          truetrackertrackid.push_back(gg_true_hit.get_track_id());
          truetrackerparenttrackid.push_back(gg_true_hit.get_parent_track_id());
        }
      }

      truetracker_.id_ = &truetrackerid;
      truetracker_.module_ = &truetrackermodule;
      truetracker_.side_ = &truetrackerside;
      truetracker_.layer_ = &truetrackerlayer;
      truetracker_.column_ = &truetrackercolumn;
      truetracker_.time_ = &truetrackertime;
      truetracker_.xstart_ = &truetrackerxstart;
      truetracker_.ystart_ = &truetrackerystart;
      truetracker_.zstart_ = &truetrackerzstart;
      truetracker_.xstop_ = &truetrackerxstop;
      truetracker_.ystop_ = &truetrackerystop;
      truetracker_.zstop_ = &truetrackerzstop;
      truetracker_.trackid_ = &truetrackertrackid;
      truetracker_.parenttrackid_ = &truetrackerparenttrackid;

      tree_->Fill();
    }

  // MUST return a status, see ref dpp::processing_status_flags_type
  return dpp::base_module::PROCESS_OK;
}

// Reset
void Things2Root::reset() {
  // write the output, finished streaming
  hfile_->cd();
  tree_->Write();
  hfile_->Close(); //

  // clean up
  delete hfile_;
  filename_output_ = "things2root.default.root";
  this->_set_initialized(false);
}
