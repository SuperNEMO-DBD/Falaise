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
  std::cout << "In INIT: create TFile " << std::endl;

  // Next all root file output here
  hfile_ = new TFile(filename_output_.c_str(),"RECREATE","Output file of Simulation data");
  hfile_->cd();

  tree_ = new TTree("SimData", "SimData");
  // 2014-02-05, F.Mauger: Force affectation of the tree's current directory to
  // explicitly avoid the tree to be reaffcted to another concurrent TFile
  // (output_module & brio format):
  // TO BE CHECKED
  // 2014-02-11, YR, seems to work fine
  tree_->SetDirectory(hfile_);

  // header data
  tree_->Branch("header.runnumber",&header_.runnumber_);
  tree_->Branch("header.eventnumber",&header_.eventnumber_);
  tree_->Branch("header.date",&header_.date_);
  tree_->Branch("header.runtype",&header_.runtype_);
  tree_->Branch("header.simulated",&header_.simulated_);

  // calibrated tracker data
  tree_->Branch("tracker.nohits",&tracker_.nohits_);
  tree_->Branch("tracker.id",&tracker_.id_);
  tree_->Branch("tracker.module",&tracker_.module_);
  tree_->Branch("tracker.side",&tracker_.side_);
  tree_->Branch("tracker.layer",&tracker_.layer_);
  tree_->Branch("tracker.column",&tracker_.column_);
  tree_->Branch("tracker.x",&tracker_.x_);
  tree_->Branch("tracker.y",&tracker_.y_);
  tree_->Branch("tracker.z",&tracker_.z_);
  tree_->Branch("tracker.sigmaz",&tracker_.sigmaz_);
  tree_->Branch("tracker.r",&tracker_.r_);
  tree_->Branch("tracker.sigmar",&tracker_.sigmar_);
  tree_->Branch("tracker.truehitid",&tracker_.truehitid_);

  // calibrated calorimeter data
  tree_->Branch("calo.nohits",&calo_.nohits_);
  tree_->Branch("calo.id",&calo_.id_);
  tree_->Branch("calo.module",&calo_.module_);
  tree_->Branch("calo.side",&calo_.side_);
  tree_->Branch("calo.column",&calo_.column_);
  tree_->Branch("calo.row",&calo_.row_);
  tree_->Branch("calo.wall",&calo_.wall_);
  tree_->Branch("calo.time",&calo_.time_);
  tree_->Branch("calo.sigmatime",&calo_.sigmatime_);
  tree_->Branch("calo.energy",&calo_.energy_);
  tree_->Branch("calo.sigmaenergy",&calo_.sigmaenergy_);
  tree_->Branch("calo.type",&calo_.type_);

  // truth tracker data
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

  // truth calorimeter data
  tree_->Branch("truecalo.nohits",&truecalo_.nohits_);
  tree_->Branch("truecalo.id",&truecalo_.id_);
  tree_->Branch("truecalo.type",&truecalo_.type_);
  tree_->Branch("truecalo.x",&truecalo_.x_);
  tree_->Branch("truecalo.y",&truecalo_.y_);
  tree_->Branch("truecalo.z",&truecalo_.z_);
  tree_->Branch("truecalo.time",&truecalo_.time_);
  tree_->Branch("truecalo.energy",&truecalo_.energy_);
  tree_->Branch("truecalo.module",&truecalo_.module_);
  tree_->Branch("truecalo.side",&truecalo_.side_);
  tree_->Branch("truecalo.wall",&truecalo_.wall_);
  tree_->Branch("truecalo.column",&truecalo_.column_);
  tree_->Branch("truecalo.row",&truecalo_.row_);

  // truth vertex data
  tree_->Branch("truevertex.x",&truevertex_.x_);
  tree_->Branch("truevertex.y",&truevertex_.y_);
  tree_->Branch("truevertex.z",&truevertex_.z_);
  tree_->Branch("truevertex.time",&truevertex_.time_);

  // truth primary particle data
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
  std::vector<int> trackertruehitid;

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

      // tracker truth hits
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

      // calorimeter truth hits
      truecalo_.nohits_ = 0;

      if (SD.has_step_hits ("calorimeter"))
	{
	  truecalo_.nohits_ += SD.get_number_of_step_hits ("calorimeter");
	  for (int ihit = 0; ihit < truecalo_.nohits_; ihit++)
	    {
	      const mctools::base_step_hit& the_scin_hit= SD.get_step_hit ("calorimeter", ihit);

	      static int gid_calo_module_index
		= geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("module");
	      static int gid_calo_side_index
		= geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("side");
	      static int gid_calo_column_index
		= geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("column");
	      static int gid_calo_row_index
		= geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("row");

	      truecaloid.push_back(the_scin_hit.get_hit_id());
	      truecalox.push_back(the_scin_hit.get_position_start ().x () / CLHEP::cm);
	      truecaloy.push_back(the_scin_hit.get_position_start ().y () / CLHEP::cm);
	      truecaloz.push_back(the_scin_hit.get_position_start ().z () / CLHEP::cm);
	      truecalotime.push_back(the_scin_hit.get_time_start ()     / CLHEP::ns);
	      truecaloenergy.push_back(the_scin_hit.get_energy_deposit () / CLHEP::MeV);

	      truecalotype.push_back(0);
	      truecalowall.push_back(0);
	      truecalomodule.push_back(the_scin_hit.get_geom_id ().get (gid_calo_module_index));
	      truecaloside.push_back(the_scin_hit.get_geom_id ().get (gid_calo_side_index));
	      truecalocolumn.push_back(the_scin_hit.get_geom_id ().get (gid_calo_column_index));
	      truecalorow.push_back(the_scin_hit.get_geom_id ().get (gid_calo_row_index));
	    }
	}

      if (SD.has_step_hits ("xcalo"))
	{
	  truecalo_.nohits_ += SD.get_number_of_step_hits ("xcalo");
	  for (unsigned int ihit = 0; ihit < SD.get_number_of_step_hits ("xcalo"); ihit++)
	    {
	      const mctools::base_step_hit& the_scin_hit= SD.get_step_hit ("xcalo", ihit);

	      static int gid_xcalo_module_index
		= geometry_manager_->get_id_mgr ().get_category_info ("xcalo_block").get_subaddress_index ("module");
	      static int gid_xcalo_side_index
		= geometry_manager_->get_id_mgr ().get_category_info ("xcalo_block").get_subaddress_index ("side");
	      static int gid_xcalo_wall_index
		= geometry_manager_->get_id_mgr ().get_category_info ("xcalo_block").get_subaddress_index ("wall");
	      static int gid_xcalo_column_index
		= geometry_manager_->get_id_mgr ().get_category_info ("xcalo_block").get_subaddress_index ("column");
	      static int gid_xcalo_row_index
		= geometry_manager_->get_id_mgr ().get_category_info ("xcalo_block").get_subaddress_index ("row");
	      truecaloid.push_back(the_scin_hit.get_hit_id());

	      truecalox.push_back(the_scin_hit.get_position_start ().x () / CLHEP::cm);
	      truecaloy.push_back(the_scin_hit.get_position_start ().y () / CLHEP::cm);
	      truecaloz.push_back(the_scin_hit.get_position_start ().z () / CLHEP::cm);
	      truecalotime.push_back(the_scin_hit.get_time_start ()     / CLHEP::ns);
	      truecaloenergy.push_back(the_scin_hit.get_energy_deposit () / CLHEP::MeV);

	      truecalotype.push_back(1);
	      truecalomodule.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_module_index));
	      truecaloside.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_side_index));
	      truecalowall.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_wall_index));
	      truecalocolumn.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_column_index));
	      truecalorow.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_row_index));
	    }
	}

      if (SD.has_step_hits ("gveto"))
	{
	  truecalo_.nohits_ += SD.get_number_of_step_hits ("gveto");
	  for (unsigned int ihit = 0; ihit < SD.get_number_of_step_hits ("gveto"); ihit++)
	    {
	      const mctools::base_step_hit& the_scin_hit = SD.get_step_hit ("gveto", ihit);

	      static int gid_gveto_module_index
		= geometry_manager_->get_id_mgr ().get_category_info ("gveto_block").get_subaddress_index ("module");
	      static int gid_gveto_side_index
		= geometry_manager_->get_id_mgr ().get_category_info ("gveto_block").get_subaddress_index ("side");
	      static int gid_gveto_wall_index
		= geometry_manager_->get_id_mgr ().get_category_info ("gveto_block").get_subaddress_index ("wall");
	      static int gid_gveto_column_index
		= geometry_manager_->get_id_mgr ().get_category_info ("gveto_block").get_subaddress_index ("column");

	      truecaloid.push_back(the_scin_hit.get_hit_id());
	      truecalox.push_back(the_scin_hit.get_position_start ().x () / CLHEP::cm);
	      truecaloy.push_back(the_scin_hit.get_position_start ().y () / CLHEP::cm);
	      truecaloz.push_back(the_scin_hit.get_position_start ().z () / CLHEP::cm);
	      truecalotime.push_back(the_scin_hit.get_time_start ()     / CLHEP::ns);
	      truecaloenergy.push_back(the_scin_hit.get_energy_deposit () / CLHEP::MeV);

	      truecalotype.push_back(2);
	      truecalorow.push_back(0);
	      truecalomodule.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_module_index));
	      truecaloside.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_side_index));
	      truecalowall.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_wall_index));
	      truecalocolumn.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_column_index));
	    }
	}

      truecalo_.id_ = &truecaloid;
      truecalo_.type_ = &truecalotype;
      truecalo_.module_ = &truecalomodule;
      truecalo_.side_ = &truecaloside;
      truecalo_.column_ = &truecalocolumn;
      truecalo_.row_ = &truecalorow;
      truecalo_.wall_ = &truecalowall;
      truecalo_.x_ = &truecalox;
      truecalo_.y_ = &truecaloy;
      truecalo_.z_ = &truecaloz;
      truecalo_.time_ = &truecalotime;
      truecalo_.energy_ = &truecaloenergy;

    }

  // look for calibrated data
  if(workItem.has("CD"))
    {
      const snemo::datamodel::calibrated_data & CD = workItem.get<snemo::datamodel::calibrated_data>("CD");
      //      std::cout << "In process: found CD data bank " << std::endl;
      tracker_.nohits_ = CD.calibrated_tracker_hits ().size ();
      BOOST_FOREACH (const snemo::datamodel::calibrated_data::tracker_hit_handle_type & gg_handle, CD.calibrated_tracker_hits ())
	{
	  if (! gg_handle.has_data ()) continue;

	  const snemo::datamodel::calibrated_tracker_hit & sncore_gg_hit = gg_handle.get ();

	  trackerid.push_back(sncore_gg_hit.get_hit_id());
	  trackermodule.push_back(sncore_gg_hit.get_geom_id().get(0));
	  trackerside.push_back(sncore_gg_hit.get_geom_id().get(1));
	  trackerlayer.push_back(sncore_gg_hit.get_geom_id().get(2));
	  trackercolumn.push_back(sncore_gg_hit.get_geom_id().get(3));
	  trackerx.push_back(sncore_gg_hit.get_x());
	  trackery.push_back(sncore_gg_hit.get_y());
	  trackerz.push_back(sncore_gg_hit.get_z());
	  trackersigmaz.push_back(sncore_gg_hit.get_sigma_z());
	  trackerr.push_back(sncore_gg_hit.get_r());
	  trackersigmar.push_back(sncore_gg_hit.get_sigma_r());
	  trackertruehitid.push_back(sncore_gg_hit.get_id());
	}
      tracker_.id_ = &trackerid;
      tracker_.module_ = &trackermodule;
      tracker_.side_ = &trackerside;
      tracker_.layer_ = &trackerlayer;
      tracker_.column_ = &trackercolumn;
      tracker_.x_ = &trackerx;
      tracker_.y_ = &trackery;
      tracker_.z_ = &trackerz;
      tracker_.sigmaz_ = &trackersigmaz;
      tracker_.r_ = &trackerr;
      tracker_.sigmar_ = &trackersigmar;
      tracker_.truehitid_ = &trackertruehitid;

      calo_.nohits_ = CD.calibrated_calorimeter_hits ().size ();
      BOOST_FOREACH (const snemo::datamodel::calibrated_data::calorimeter_hit_handle_type & the_calo_hit_handle, CD.calibrated_calorimeter_hits ())
	{
	  if(! the_calo_hit_handle.has_data() ) continue;

	  const snemo::datamodel::calibrated_calorimeter_hit & the_calo_hit = the_calo_hit_handle.get ();

	  caloid.push_back(the_calo_hit.get_hit_id ());
	  calomodule.push_back(the_calo_hit.get_geom_id ().get (0));
	  caloside.push_back(the_calo_hit.get_geom_id ().get (1));

	  if (the_calo_hit.get_geom_id ().get_type () == 1302)
	    {
	      // CALO
	      calowall.push_back(0);
	      calocolumn.push_back(the_calo_hit.get_geom_id ().get (2));
	      calorow.push_back(the_calo_hit.get_geom_id ().get (3));
	      calotype.push_back(0);
	    }
	  if (the_calo_hit.get_geom_id ().get_type () == 1232)
	    {
	      // XCALO
	      calowall.push_back(the_calo_hit.get_geom_id ().get (2));
	      calocolumn.push_back(the_calo_hit.get_geom_id ().get (3));
	      calorow.push_back(the_calo_hit.get_geom_id ().get (0));
	      calotype.push_back(1);
	    }
	  if (the_calo_hit.get_geom_id ().get_type () == 1252)
	    {
	      // GVETO
	      calowall.push_back(the_calo_hit.get_geom_id ().get (2));
	      calocolumn.push_back(the_calo_hit.get_geom_id ().get (3));
	      calorow.push_back(0);
	      calotype.push_back(2);
	    }

	  calotime.push_back(the_calo_hit.get_time());
	  calosigmatime.push_back(the_calo_hit.get_sigma_time());
	  caloenergy.push_back(the_calo_hit.get_energy());
	  calosigmaenergy.push_back(the_calo_hit.get_sigma_energy());
	}
      calo_.id_ = &caloid;
      calo_.type_ = &calotype;
      calo_.module_ = &calomodule;
      calo_.side_ = &caloside;
      calo_.column_ = &calocolumn;
      calo_.row_ = &calorow;
      calo_.wall_ = &calowall;
      calo_.time_ = &calotime;
      calo_.sigmatime_ = &calosigmatime;
      calo_.energy_ = &caloenergy;
      calo_.sigmaenergy_ = &calosigmaenergy;

    }
  // look for event header
  if(workItem.has("EH"))
    {
      const snemo::datamodel::event_header & EH = workItem.get<snemo::datamodel::event_header>("EH");
      //      std::cout << "In process: found EH event header " << std::endl;
      header_.runnumber_ = EH.get_id ().get_run_number ();
      header_.eventnumber_ = EH.get_id ().get_event_number ();
      header_.date_ = 0;
      header_.runtype_ = 0;
      header_.simulated_ = (EH.is_simulated () ? true : false);
    }

  tree_->Fill();

  // MUST return a status, see ref dpp::processing_status_flags_type
  return dpp::base_module::PROCESS_OK;
}

// Reset
void Things2Root::reset() {
  // write the output, finished streaming
  hfile_->cd();
  tree_->Write();
  hfile_->Close(); //
  std::cout << "In reset: finished conversion, file closed " << std::endl;

  // clean up
  delete hfile_;
  filename_output_ = "things2root.default.root";
  this->_set_initialized(false);
}
