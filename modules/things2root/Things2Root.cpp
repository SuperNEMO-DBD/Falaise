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

DPP_MODULE_REGISTRATION_IMPLEMENT(Things2Root,"Things2Root");

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
  std::vector<int> trackertruehitid;
  std::vector<int> trackertruetrackid;
  std::vector<int> trackertrueparenttrackid;

  // calibrated calorimeter data
  std::vector<int> caloid;
  std::vector<int> calomodule;
  std::vector<int> caloside;
  std::vector<int> calocolumn;
  std::vector<int> xcalocolumn;
  std::vector<int> gvetocolumn;
  std::vector<int> calorow;
  std::vector<int> xcalowall;
  std::vector<int> gvetowall;
  std::vector<double> calotime;
  std::vector<double> calosigmatime;
  std::vector<double> caloenergy;
  std::vector<double> calosigmaenergy;
  std::vector<int> calotype;
  std::vector<int> truecalohitid;

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
  std::vector<int> truexcalocolumn;
  std::vector<int> truegvetocolumn;
  std::vector<int> truecalorow;
  std::vector<int> truexcalowall;
  std::vector<int> truegvetowall;
  std::vector<double> truecalox;
  std::vector<double> truecaloy;
  std::vector<double> truecaloz;
  std::vector<double> truecalotime;
  std::vector<double> truecaloenergy;

  // for primary (true) particles
  std::vector<int> truepartid;
  std::vector<int> trueparttype;
  std::vector<double> truepartpx;
  std::vector<double> truepartpy;
  std::vector<double> truepartpz;
  std::vector<double> trueparttime;
  std::vector<double> truepartenergy;
  std::vector<double> truepartmass;

  // for clusters
  std::vector<int> clustersolutionid;
  std::vector<int> nounclusteredhits;
  std::vector<int> noclusters;
  //std::vector< std::vector <int> > clusterid;
  std::vector<int> clusterid;
  std::vector<int> nohits;
  std::vector<int> clustertrackerid;
  std::vector<double> catcharge;
  std::vector<double> catdetailedcharge;
  std::vector<double> catchi2s;
  //std::vector<double> catchi2sall;
  std::vector<double> catprobs;
  //std::vector<double> catprobsall;
  std::vector<double> catpx;
  std::vector<double> catpy;
  std::vector<double> catpz;
  std::vector<double> cathelixcharge;
  std::vector<double> cathelixlength;
  std::vector<double> cathelixlengtherror;
  std::vector<double> cathelixvertexx;
  std::vector<double> cathelixvertexxerror;
  std::vector<double> cathelixvertexy;
  std::vector<double> cathelixvertexyerror;
  std::vector<double> cathelixvertexz;
  std::vector<double> cathelixvertexzerror;
  std::vector<double> cathelixdecayvertexx;
  std::vector<double> cathelixdecayvertexxerror;
  std::vector<double> cathelixdecayvertexy;
  std::vector<double> cathelixdecayvertexyerror;
  std::vector<double> cathelixdecayvertexz;
  std::vector<double> cathelixdecayvertexzerror;
  std::vector<double> cattangentlength;
  std::vector<double> cattangentlengtherror;
  std::vector<double> cattangentvertexx;
  std::vector<double> cattangentvertexxerror;
  std::vector<double> cattangentvertexy;
  std::vector<double> cattangentvertexyerror;
  std::vector<double> cattangentvertexz;
  std::vector<double> cattangentvertexzerror;
  std::vector<double> cattangentdecayvertexx;
  std::vector<double> cattangentdecayvertexxerror;
  std::vector<double> cattangentdecayvertexy;
  std::vector<double> cattangentdecayvertexyerror;
  std::vector<double> cattangentdecayvertexz;
  std::vector<double> cattangentdecayvertexzerror;
  //std::vector<double> cathelixchi2s;

  // for trajectory data
  std::vector<int> trajectorysolutionid;
  std::vector<int> notrajectories;
  std::vector<int> nounfittedclusters;
  std::vector<int> nounfittedhits;
  std::vector<int> trajectoryid;
  std::vector<int> trajectoryclusterid;
  std::vector<double> chi2;
  std::vector<int> ndof;
  std::vector<double> trajectorylength;

  // for particle data
  std::vector<int> charge;
  std::vector<int> nocalohits;
  std::vector<double> energy;
  std::vector<double> particlecalotime;
  std::vector<int> novertices;
  std::vector<double> vertexx;
  std::vector<double> vertexy;
  std::vector<double> vertexz;
  std::vector<double> vertexxerror;
  std::vector<double> vertexyerror;
  std::vector<double> vertexzerror;
  std::vector<int> particletrajid;

  void clear();
};

void Things2Root::working_space::clear()
{
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
  trackertruehitid.clear();
  trackertruetrackid.clear();
  trackertrueparenttrackid.clear();

  // clear calibrated calorimeter data
  caloid.clear();
  calomodule.clear();
  caloside.clear();
  calocolumn.clear();
  xcalocolumn.clear();
  gvetocolumn.clear();
  calorow.clear();
  xcalowall.clear();
  gvetowall.clear();
  calotime.clear();
  calosigmatime.clear();
  caloenergy.clear();
  calosigmaenergy.clear();
  calotype.clear();
  truecalohitid.clear();

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
  truexcalocolumn.clear();
  truegvetocolumn.clear();
  truecalorow.clear();
  truexcalowall.clear();
  truegvetowall.clear();
  truecalox.clear();
  truecaloy.clear();
  truecaloz.clear();
  truecalotime.clear();
  truecaloenergy.clear();

  // clear primary particles
  truepartid.clear();
  trueparttype.clear();
  truepartpx.clear();
  truepartpy.clear();
  truepartpz.clear();
  trueparttime.clear();
  truepartenergy.clear();
  truepartmass.clear();

  // clear clusters
  clustersolutionid.clear();
  nounclusteredhits.clear();
  noclusters.clear();
  clusterid.clear();
  nohits.clear();
  clustertrackerid.clear();
  catcharge.clear();
  catdetailedcharge.clear();
  catchi2s.clear();
  //catchi2sall.clear();
  catprobs.clear();
  //catprobsall.clear();
  catpx.clear();
  catpy.clear();
  catpz.clear();
  cathelixcharge.clear();
  cathelixlength.clear();
  cathelixlengtherror.clear();
  cathelixvertexx.clear();
  cathelixvertexxerror.clear();
  cathelixvertexy.clear();
  cathelixvertexyerror.clear();
  cathelixvertexz.clear();
  cathelixvertexzerror.clear();
  cathelixdecayvertexx.clear();
  cathelixdecayvertexxerror.clear();
  cathelixdecayvertexy.clear();
  cathelixdecayvertexyerror.clear();
  cathelixdecayvertexz.clear();
  cathelixdecayvertexzerror.clear();
  //cathelixchi2s.clear();
  cattangentlength.clear();
  cattangentlengtherror.clear();
  cattangentvertexx.clear();
  cattangentvertexxerror.clear();
  cattangentvertexy.clear();
  cattangentvertexyerror.clear();
  cattangentvertexz.clear();
  cattangentvertexzerror.clear();
  cattangentdecayvertexx.clear();
  cattangentdecayvertexxerror.clear();
  cattangentdecayvertexy.clear();
  cattangentdecayvertexyerror.clear();
  cattangentdecayvertexz.clear();
  cattangentdecayvertexzerror.clear();

  // clear trajectories
  trajectorysolutionid.clear();
  notrajectories.clear();
  nounfittedclusters.clear();
  nounfittedhits.clear();
  trajectoryid.clear();
  trajectoryclusterid.clear();
  chi2.clear();
  ndof.clear();
  trajectorylength.clear();

  // clear particles
  charge.clear();
  nocalohits.clear();
  energy.clear();
  particlecalotime.clear();
  novertices.clear();
  vertexx.clear();
  vertexy.clear();
  vertexz.clear();
  vertexxerror.clear();
  vertexyerror.clear();
  vertexzerror.clear();
  particletrajid.clear();
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
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Create TFile...");

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
  tree_->Branch("calo.xcalocolumn",&calo_.xcalocolumn_);
  tree_->Branch("calo.gvetocolumn",&calo_.gvetocolumn_);
  tree_->Branch("calo.row",&calo_.row_);
  tree_->Branch("calo.xcalowall",&calo_.xcalowall_);
  tree_->Branch("calo.gvetowall",&calo_.gvetowall_);
  tree_->Branch("calo.time",&calo_.time_);
  tree_->Branch("calo.sigmatime",&calo_.sigmatime_);
  tree_->Branch("calo.energy",&calo_.energy_);
  tree_->Branch("calo.sigmaenergy",&calo_.sigmaenergy_);
  tree_->Branch("calo.type",&calo_.type_);
  tree_->Branch("calo.truecaloid",&calo_.truecaloid_);

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
  tree_->Branch("truecalo.xcalowall",&truecalo_.xcalowall_);
  tree_->Branch("truecalo.gvetowall",&truecalo_.gvetowall_);
  tree_->Branch("truecalo.column",&truecalo_.column_);
  tree_->Branch("truecalo.xcalocolumn",&truecalo_.xcalocolumn_);
  tree_->Branch("truecalo.gvetocolumn",&truecalo_.gvetocolumn_);
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
  tree_->Branch("trueparticle.mass",&trueparticle_.mass_);

  // cluster data
  tree_->Branch("cluster.nosolutions",&cluster_.nosolutions_);
  tree_->Branch("cluster.clustersolutionid",&cluster_.clustersolutionid_);
  tree_->Branch("cluster.nounclusteredhits",&cluster_.nounclusteredhits_);
  tree_->Branch("cluster.noclusters",&cluster_.noclusters_);
  tree_->Branch("cluster.clusterid",&cluster_.clusterid_);
  tree_->Branch("cluster.nohits",&cluster_.nohits_);
  tree_->Branch("cluster.hitid",&cluster_.hitid_);
  tree_->Branch("cluster.catcharge",&cluster_.catcharge_);
  tree_->Branch("cluster.catdetailedcharge",&cluster_.catdetailedcharge_);
  tree_->Branch("cluster.catchi2s",&cluster_.catchi2s_);
  //tree_->Branch("cluster.catchi2sall",&cluster_.catchi2sall_);
  tree_->Branch("cluster.catprobs",&cluster_.catprobs_);
  //tree_->Branch("cluster.catprobsall",&cluster_.catprobsall_);
  tree_->Branch("cluster.catpx",&cluster_.catpx_);
  tree_->Branch("cluster.catpy",&cluster_.catpy_);
  tree_->Branch("cluster.catpz",&cluster_.catpz_);
  tree_->Branch("cluster.cathelixcharge",&cluster_.cathelixcharge_);
  tree_->Branch("cluster.cathelixlength",&cluster_.cathelixlength_);
  tree_->Branch("cluster.cathelixlengtherror",&cluster_.cathelixlengtherror_);
  tree_->Branch("cluster.cathelixvertexx",&cluster_.cathelixvertexx_);
  tree_->Branch("cluster.cathelixvertexxerror",&cluster_.cathelixvertexxerror_);
  tree_->Branch("cluster.cathelixvertexy",&cluster_.cathelixvertexy_);
  tree_->Branch("cluster.cathelixvertexyerror",&cluster_.cathelixvertexyerror_);
  tree_->Branch("cluster.cathelixvertexz",&cluster_.cathelixvertexz_);
  tree_->Branch("cluster.cathelixvertexzerror",&cluster_.cathelixvertexzerror_);
  tree_->Branch("cluster.cathelixdecayvertexx",&cluster_.cathelixdecayvertexx_);
  tree_->Branch("cluster.cathelixdecayvertexxerror",&cluster_.cathelixdecayvertexxerror_);
  tree_->Branch("cluster.cathelixdecayvertexy",&cluster_.cathelixdecayvertexy_);
  tree_->Branch("cluster.cathelixdecayvertexyerror",&cluster_.cathelixdecayvertexyerror_);
  tree_->Branch("cluster.cathelixdecayvertexz",&cluster_.cathelixdecayvertexz_);
  tree_->Branch("cluster.cathelixdecayvertexzerror",&cluster_.cathelixdecayvertexzerror_);
  //tree_->Branch("cluster.cathelixchi2s",&cluster_.cathelixchi2s_);
  tree_->Branch("cluster.cattangentlength",&cluster_.cattangentlength_);
  tree_->Branch("cluster.cattangentlengtherror",&cluster_.cattangentlengtherror_);
  tree_->Branch("cluster.cattangentvertexx",&cluster_.cattangentvertexx_);
  tree_->Branch("cluster.cattangentvertexxerror",&cluster_.cattangentvertexxerror_);
  tree_->Branch("cluster.cattangentvertexy",&cluster_.cattangentvertexy_);
  tree_->Branch("cluster.cattangentvertexyerror",&cluster_.cattangentvertexyerror_);
  tree_->Branch("cluster.cattangentvertexz",&cluster_.cattangentvertexz_);
  tree_->Branch("cluster.cattangentvertexzerror",&cluster_.cattangentvertexzerror_);
  tree_->Branch("cluster.cattangentdecayvertexx",&cluster_.cattangentdecayvertexx_);
  tree_->Branch("cluster.cattangentdecayvertexxerror",&cluster_.cattangentdecayvertexxerror_);
  tree_->Branch("cluster.cattangentdecayvertexy",&cluster_.cattangentdecayvertexy_);
  tree_->Branch("cluster.cattangentdecayvertexyerror",&cluster_.cattangentdecayvertexyerror_);
  tree_->Branch("cluster.cattangentdecayvertexz",&cluster_.cattangentdecayvertexz_);
  tree_->Branch("cluster.cattangentdecayvertexzerror",&cluster_.cattangentdecayvertexzerror_);

  // trajectory data
  tree_->Branch("trajectory.nosolutions",&trajectory_.nosolutions_);
  tree_->Branch("trajectory.trajectorysolutionid",&trajectory_.trajectorysolutionid_);
  tree_->Branch("trajectory.notrajectories",&trajectory_.notrajectories_);
  tree_->Branch("trajectory.nounfittedclusters",&trajectory_.nounfittedclusters_);
  tree_->Branch("trajectory.nounfittedhits",&trajectory_.nounfittedhits_);
  tree_->Branch("trajectory.trajectoryid",&trajectory_.trajectoryid_);
  tree_->Branch("trajectory.clusterid",&trajectory_.clusterid_);
  tree_->Branch("trajectory.chi2",&trajectory_.chi2_);
  tree_->Branch("trajectory.ndof",&trajectory_.ndof_);
  tree_->Branch("trajectory.length",&trajectory_.length_);

  // particle data
  tree_->Branch("particle.noparticles",&particle_.noparticles_);
  tree_->Branch("particle.charge",&particle_.charge_);
  tree_->Branch("particle.nocalohits",&particle_.nocalohits_);
  tree_->Branch("particle.energy",&particle_.energy_);
  tree_->Branch("particle.calotime",&particle_.calotime_);
  tree_->Branch("particle.novertices",&particle_.novertices_);
  tree_->Branch("particle.vertexx",&particle_.vertexx_);
  tree_->Branch("particle.vertexy",&particle_.vertexy_);
  tree_->Branch("particle.vertexz",&particle_.vertexz_);
  tree_->Branch("particle.vertexxerror",&particle_.vertexxerror_);
  tree_->Branch("particle.vertexyerror",&particle_.vertexyerror_);
  tree_->Branch("particle.vertexzerror",&particle_.vertexzerror_);
  tree_->Branch("particle.trajectoryid",&particle_.trajectoryid_);

  this->_set_initialized(true);
}

// Process
dpp::base_module::process_status Things2Root::process(datatools::things& workItem) {
  // Local variables
  if (! ws_) {
    ws_ = new working_space;
  } else {
    ws_->clear();
  }


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
        ws_->truepartid.push_back(count);
        ws_->trueparttype.push_back(the_particle.get_type());
        ws_->truepartpx.push_back(the_particle.get_momentum().x());
        ws_->truepartpy.push_back(the_particle.get_momentum().y());
        ws_->truepartpz.push_back(the_particle.get_momentum().z());
        ws_->trueparttime.push_back(the_particle.get_time());
        ws_->truepartenergy.push_back(the_particle.get_kinetic_energy());
        ws_->truepartmass.push_back(the_particle.get_mass());
        count++;
      }

      trueparticle_.id_ = &ws_->truepartid;
      trueparticle_.type_ = &ws_->trueparttype;
      trueparticle_.px_ = &ws_->truepartpx;
      trueparticle_.py_ = &ws_->truepartpy;
      trueparticle_.pz_ = &ws_->truepartpz;
      trueparticle_.time_ = &ws_->trueparttime;
      trueparticle_.ke_ = &ws_->truepartenergy;
      trueparticle_.mass_ = &ws_->truepartmass;

      // tracker truth hits
      if (SD.has_step_hits("gg")) {
        int nggtruehits = SD.get_number_of_step_hits("gg");
        truetracker_.nohits_ = nggtruehits;
        //std::cout << SD.get_number_of_step_hits("gg") << " - " << nggtruehits << " - " << truetracker_.nohits_ << std::endl;

        // this needs the geometry manager
        static int gid_gg_module_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("module");

        static int gid_gg_side_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("side");

        static int gid_gg_layer_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("layer");

        static int gid_gg_row_index = geometry_manager_->get_id_mgr().get_category_info("drift_cell_core").get_subaddress_index("row");

        // this is the event loop
        for (int i = 0; i < nggtruehits; ++i) {
          const mctools::base_step_hit& gg_true_hit = SD.get_step_hit("gg",i);
          //ws_->truetrackerid.push_back(gg_true_hit.get_hit_id());
          ws_->truetrackerid.push_back(i);
          ws_->truetrackermodule.push_back(gg_true_hit.get_geom_id().get(gid_gg_module_index));
          ws_->truetrackerside.push_back(gg_true_hit.get_geom_id().get(gid_gg_side_index));
          ws_->truetrackerlayer.push_back(gg_true_hit.get_geom_id().get (gid_gg_layer_index));
          ws_->truetrackercolumn.push_back(gg_true_hit.get_geom_id().get (gid_gg_row_index));

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

      if (SD.has_step_hits ("calo"))
        {
          truecalo_.nohits_ += SD.get_number_of_step_hits("calo");
          for (int ihit = 0; ihit < truecalo_.nohits_; ihit++)
            {
              const mctools::base_step_hit& the_scin_hit= SD.get_step_hit ("calo", ihit);

              static int gid_calo_module_index
                = geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("module");
              static int gid_calo_side_index
                = geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("side");
              static int gid_calo_column_index
                = geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("column");
              static int gid_calo_row_index
                = geometry_manager_->get_id_mgr ().get_category_info ("calorimeter_block").get_subaddress_index ("row");

              ws_->truecaloid.push_back(the_scin_hit.get_hit_id());
              ws_->truecalox.push_back(the_scin_hit.get_position_start ().x () / CLHEP::cm);
              ws_->truecaloy.push_back(the_scin_hit.get_position_start ().y () / CLHEP::cm);
              ws_->truecaloz.push_back(the_scin_hit.get_position_start ().z () / CLHEP::cm);
              ws_->truecalotime.push_back(the_scin_hit.get_time_start ()     / CLHEP::ns);
              ws_->truecaloenergy.push_back(the_scin_hit.get_energy_deposit () / CLHEP::MeV);

              ws_->truecalotype.push_back(0);
              //ws_->truecalowall.push_back(0);
              ws_->truecalomodule.push_back(the_scin_hit.get_geom_id ().get (gid_calo_module_index));
              ws_->truecaloside.push_back(the_scin_hit.get_geom_id ().get (gid_calo_side_index));
              ws_->truecalocolumn.push_back(the_scin_hit.get_geom_id ().get (gid_calo_column_index));
              ws_->truecalorow.push_back(the_scin_hit.get_geom_id ().get (gid_calo_row_index));
            }
        }
      //std::cout << truecalo_.nohits_ << " side calo hits" << std::endl;

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
              ws_->truecaloid.push_back(the_scin_hit.get_hit_id());

              ws_->truecalox.push_back(the_scin_hit.get_position_start ().x () / CLHEP::cm);
              ws_->truecaloy.push_back(the_scin_hit.get_position_start ().y () / CLHEP::cm);
              ws_->truecaloz.push_back(the_scin_hit.get_position_start ().z () / CLHEP::cm);
              ws_->truecalotime.push_back(the_scin_hit.get_time_start ()     / CLHEP::ns);
              ws_->truecaloenergy.push_back(the_scin_hit.get_energy_deposit () / CLHEP::MeV);

              ws_->truecalotype.push_back(1);
              ws_->truecalomodule.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_module_index));
              ws_->truecaloside.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_side_index));
              ws_->truexcalowall.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_wall_index));
              ws_->truexcalocolumn.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_column_index));
              ws_->truecalorow.push_back(the_scin_hit.get_geom_id ().get (gid_xcalo_row_index));
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

              ws_->truecaloid.push_back(the_scin_hit.get_hit_id());
              ws_->truecalox.push_back(the_scin_hit.get_position_start ().x () / CLHEP::cm);
              ws_->truecaloy.push_back(the_scin_hit.get_position_start ().y () / CLHEP::cm);
              ws_->truecaloz.push_back(the_scin_hit.get_position_start ().z () / CLHEP::cm);
              ws_->truecalotime.push_back(the_scin_hit.get_time_start ()     / CLHEP::ns);
              ws_->truecaloenergy.push_back(the_scin_hit.get_energy_deposit () / CLHEP::MeV);

              ws_->truecalotype.push_back(2);
              //ws_->truecalorow.push_back(0);
              ws_->truecalomodule.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_module_index));
              ws_->truecaloside.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_side_index));
              ws_->truegvetowall.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_wall_index));
              ws_->truegvetocolumn.push_back(the_scin_hit.get_geom_id ().get (gid_gveto_column_index));
            }
        }

      truecalo_.id_ = &ws_->truecaloid;
      truecalo_.type_ = &ws_->truecalotype;
      truecalo_.module_ = &ws_->truecalomodule;
      truecalo_.side_ = &ws_->truecaloside;
      truecalo_.column_ = &ws_->truecalocolumn;
      truecalo_.xcalocolumn_ = &ws_->truexcalocolumn;
      truecalo_.gvetocolumn_ = &ws_->truegvetocolumn;
      truecalo_.row_ = &ws_->truecalorow;
      truecalo_.xcalowall_ = &ws_->truexcalowall;
      truecalo_.gvetowall_ = &ws_->truegvetowall;
      truecalo_.x_ = &ws_->truecalox;
      truecalo_.y_ = &ws_->truecaloy;
      truecalo_.z_ = &ws_->truecaloz;
      truecalo_.time_ = &ws_->truecalotime;
      truecalo_.energy_ = &ws_->truecaloenergy;

    }

  // look for calibrated data
  if(workItem.has("CD"))
    {
      // Geometry categories for the scintillator blocks:
      unsigned int calo_geom_type = 1302;
      unsigned int xcalo_geom_type = 1232;
      unsigned int gveto_geom_type = 1252;

      const snemo::datamodel::calibrated_data & CD = workItem.get<snemo::datamodel::calibrated_data>("CD");
      //      std::clog << "In process: found CD data bank " << std::endl;
      tracker_.nohits_ = CD.calibrated_tracker_hits ().size ();
      BOOST_FOREACH (const snemo::datamodel::calibrated_data::tracker_hit_handle_type & gg_handle, CD.calibrated_tracker_hits ())
        {
          if (! gg_handle.has_data ()) continue;

          const snemo::datamodel::calibrated_tracker_hit & sncore_gg_hit = gg_handle.get ();

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

          for (int truegghit= sncore_gg_hit.get_hit_id(); truegghit < ws_->truetrackerid.size(); ++truegghit)
            {
              if(sncore_gg_hit.get_geom_id().get(0) == ws_->truetrackermodule[truegghit]
                 && sncore_gg_hit.get_geom_id().get(1) == ws_->truetrackerside[truegghit]
                 && sncore_gg_hit.get_geom_id().get(2) == ws_->truetrackerlayer[truegghit]
                 && sncore_gg_hit.get_geom_id().get(3) == ws_->truetrackercolumn[truegghit])
                {
                  ws_->trackertruehitid.push_back(ws_->truetrackerid[truegghit]);
                  break;
                }
            }

          //ws_->trackertruehitid.push_back(sncore_gg_hit.get_id());

          // special infos about truth tracks:
          int truth_track_id = -1;
          if (sncore_gg_hit.get_auxiliaries().has_key(mctools::track_utils::TRACK_ID_KEY)) {
            truth_track_id = sncore_gg_hit.get_auxiliaries().fetch_integer(mctools::track_utils::TRACK_ID_KEY);
          }
          ws_->trackertruetrackid.push_back(truth_track_id);
          int truth_parent_track_id = -1;
          if (sncore_gg_hit.get_auxiliaries().has_key(mctools::track_utils::PARENT_TRACK_ID_KEY)) {
            truth_parent_track_id = sncore_gg_hit.get_auxiliaries().fetch_integer(mctools::track_utils::PARENT_TRACK_ID_KEY);
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
      tracker_.truehitid_ = &ws_->trackertruehitid;
      tracker_.truetrackid_ = &ws_->trackertruetrackid;
      tracker_.trueparenttrackid_ = &ws_->trackertrueparenttrackid;


      calo_.nohits_ = CD.calibrated_calorimeter_hits ().size ();
      BOOST_FOREACH (const snemo::datamodel::calibrated_data::calorimeter_hit_handle_type & the_calo_hit_handle, CD.calibrated_calorimeter_hits ())
        {
          if(! the_calo_hit_handle.has_data() ) continue;

          const snemo::datamodel::calibrated_calorimeter_hit & the_calo_hit = the_calo_hit_handle.get ();

          ws_->caloid.push_back(the_calo_hit.get_hit_id ());
          ws_->calomodule.push_back(the_calo_hit.get_geom_id ().get (0));
          ws_->caloside.push_back(the_calo_hit.get_geom_id ().get (1));

          if (the_calo_hit.get_geom_id ().get_type () == calo_geom_type)
            {
              // CALO
              ws_->calocolumn.push_back(the_calo_hit.get_geom_id ().get (2));
              ws_->calorow.push_back(the_calo_hit.get_geom_id ().get (3));
              ws_->calotype.push_back(0);
              for (int ntruecalohit = the_calo_hit.get_hit_id (); ntruecalohit < ws_->truecaloid.size(); ++ntruecalohit)
                {
                  if (ws_->truecalotype[ntruecalohit] == 0
                      && the_calo_hit.get_geom_id ().get (0) == ws_->truecalomodule[ntruecalohit]
                      && the_calo_hit.get_geom_id ().get (1) == ws_->truecaloside[ntruecalohit]
                      && the_calo_hit.get_geom_id ().get (2) == ws_->truecalocolumn[ntruecalohit]
                      && the_calo_hit.get_geom_id ().get (3) == ws_->truecalorow[ntruecalohit])
                    {
                      ws_->truecalohitid.push_back(ws_->truecaloid[ntruecalohit]);
                      break;
                    }
                }
            }
          if (the_calo_hit.get_geom_id ().get_type () == xcalo_geom_type)
            {
              // XCALO
              ws_->xcalowall.push_back(the_calo_hit.get_geom_id ().get (2));
              ws_->xcalocolumn.push_back(the_calo_hit.get_geom_id ().get (3));
              ws_->calorow.push_back(the_calo_hit.get_geom_id ().get (4));
              ws_->calotype.push_back(1);
              //std::cout << "there's an xcalo..." << std::endl;
              for (int ntruecalohit = the_calo_hit.get_hit_id (); ntruecalohit < ws_->truecaloid.size(); ++ntruecalohit)
                {
                  if (ws_->truecalotype[ntruecalohit] == 1
                      && the_calo_hit.get_geom_id ().get (0) == ws_->truecalomodule[ntruecalohit]
                      && the_calo_hit.get_geom_id ().get (1) == ws_->truecaloside[ntruecalohit]
                      && the_calo_hit.get_geom_id ().get (4) == ws_->truecalorow[ntruecalohit])
                    {
                      //std::cout << "xcalo passed the first if!" << std::endl;
                      for (int ntruexcalohit = 0; ntruexcalohit < ws_->truexcalowall.size(); ++ntruexcalohit)
                        {
                          //std::cout << "xcalo iteration " << ntruexcalohit  << std::endl;
                          if(the_calo_hit.get_geom_id ().get (2) == ws_->truexcalowall[ntruexcalohit]
                             && the_calo_hit.get_geom_id ().get (3) == ws_->truexcalocolumn[ntruexcalohit])
                            {
                              //std::cout << "xcalo passed the second if!" << std::endl;
                              ws_->truecalohitid.push_back(ws_->truecaloid[ntruecalohit]);
                              break; 
                            }
                        }
                    }
                }
            }
          if (the_calo_hit.get_geom_id ().get_type () == gveto_geom_type)
            {
              // GVETO
              ws_->gvetowall.push_back(the_calo_hit.get_geom_id ().get (2));
              ws_->gvetocolumn.push_back(the_calo_hit.get_geom_id ().get (3));
              //ws_->calorow.push_back(0);
              ws_->calotype.push_back(2);
              for (int ntruecalohit = the_calo_hit.get_hit_id (); ntruecalohit < ws_->truecaloid.size(); ++ntruecalohit)
                {
                  if (ws_->truecalotype[ntruecalohit] == 2
                      && the_calo_hit.get_geom_id ().get (0) == ws_->truecalomodule[ntruecalohit]
                      && the_calo_hit.get_geom_id ().get (1) == ws_->truecaloside[ntruecalohit])
                    {
                      for (int ntruegvetohit = 0; ntruegvetohit < ws_->truegvetowall.size(); ++ntruegvetohit)
                        {
                          if (the_calo_hit.get_geom_id ().get (2) == ws_->truegvetowall[ntruegvetohit]
                              && the_calo_hit.get_geom_id ().get (3) == ws_->truegvetocolumn[ntruegvetohit])
                            {
                              ws_->truecalohitid.push_back(ws_->truecaloid[ntruecalohit]);
                              break;
                            }
                        }
                    }
                }
            }

          ws_->calotime.push_back(the_calo_hit.get_time());
          ws_->calosigmatime.push_back(the_calo_hit.get_sigma_time());
          ws_->caloenergy.push_back(the_calo_hit.get_energy());
          ws_->calosigmaenergy.push_back(the_calo_hit.get_sigma_energy());
        }
      calo_.id_          = &ws_->caloid;
      calo_.type_        = &ws_->calotype;
      calo_.module_      = &ws_->calomodule;
      calo_.side_        = &ws_->caloside;
      calo_.column_      = &ws_->calocolumn;
      calo_.xcalocolumn_      = &ws_->xcalocolumn;
      calo_.gvetocolumn_      = &ws_->gvetocolumn;
      calo_.row_         = &ws_->calorow;
      calo_.xcalowall_        = &ws_->xcalowall;
      calo_.gvetowall_        = &ws_->gvetowall;
      calo_.time_        = &ws_->calotime;
      calo_.sigmatime_   = &ws_->calosigmatime;
      calo_.energy_      = &ws_->caloenergy;
      calo_.sigmaenergy_ = &ws_->calosigmaenergy;
      calo_.truecaloid_ = &ws_->truecalohitid;

    }
  // look for event header
  if(workItem.has("EH"))
    {
      const snemo::datamodel::event_header & EH = workItem.get<snemo::datamodel::event_header>("EH");
      //      std::clog << "In process: found EH event header " << std::endl;
      header_.runnumber_ = EH.get_id ().get_run_number ();
      header_.eventnumber_ = EH.get_id ().get_event_number ();
      header_.date_ = 0;
      header_.runtype_ = 0;
      header_.simulated_ = (EH.is_simulated () ? true : false);
    }

  // look for cluster data
  if(workItem.has("TCD"))
    {
      const snemo::datamodel::tracker_clustering_data & TCD = workItem.get<snemo::datamodel::tracker_clustering_data>("TCD");
      if(TCD.has_solutions())
        {
          cluster_.nosolutions_ = TCD.get_number_of_solutions();
          int nsolutions = TCD.get_number_of_solutions();
          for (int i = 0; i < nsolutions; ++i)
            {
              const snemo::datamodel::tracker_clustering_solution & solution = TCD.get_solution(i);
              ws_->clustersolutionid.push_back(solution.get_solution_id());
              ws_->nounclusteredhits.push_back(solution.get_unclustered_hits().size());
              int nclusters = solution.get_clusters() .size();
              ws_->noclusters.push_back(nclusters);
              if (nclusters == 0) continue;
              for (int j = 0; j < nclusters; ++j)
                {
                  const snemo::datamodel::tracker_cluster & cluster = solution.get_clusters()[j].get();
                  ws_->clusterid.push_back(cluster.get_cluster_id());
                  ws_->nohits.push_back(cluster.get_number_of_hits());
                  int nhits = cluster.get_number_of_hits();

                  const datatools::properties & aux = cluster.get_auxiliaries();
                  if(aux.fetch_boolean_scalar("CAT_has_charge")) ws_->catcharge.push_back(aux.fetch_real_scalar("CAT_charge"));
                  if(aux.fetch_boolean_scalar("CAT_has_detailed_charge")) ws_->catdetailedcharge.push_back(aux.fetch_real_scalar("CAT_detailed_charge"));
                  if(aux.fetch_boolean_scalar("CAT_has_momentum"))
                    {
                      ws_->catpx.push_back(aux.fetch_real_scalar("CAT_momentum_x"));
                      ws_->catpy.push_back(aux.fetch_real_scalar("CAT_momentum_y"));
                      ws_->catpz.push_back(aux.fetch_real_scalar("CAT_momentum_z"));
                    }
                  for(int k = 0; k < nhits; ++k)
                    {
                      ws_->catchi2s.push_back(aux.fetch_real("CAT_chi2s",k));
                      ws_->catprobs.push_back(aux.fetch_real("CAT_probs",k));
                      //if(aux.fetch_real("CAT_chi2s_all",k) != NULL) ws_->catchi2sall.push_back(aux.fetch_real("CAT_chi2s_all",k));
                      //if(aux.fetch_real("CAT_probs_all",k) != NULL) ws_->catprobsall.push_back(aux.fetch_real("CAT_probs_all",k));

                      //if(aux.fetch_real("CAT_helix_chi2s",k) != NULL) ws_->cathelixchi2s.push_back(aux.fetch_real("CAT_helix_chi2s",k));
                      ws_->clustertrackerid.push_back(cluster.get_hit(k).get_id());
                    }

                  if(aux.fetch_boolean_scalar("CAT_has_helix_charge")) ws_->cathelixcharge.push_back(aux.fetch_real_scalar("CAT_helix_charge"));
                  if(aux.fetch_boolean_scalar("CAT_has_helix_length"))
                    {
                      ws_->cathelixlength.push_back(aux.fetch_real_scalar("CAT_helix_length"));
                      ws_->cathelixlengtherror.push_back(aux.fetch_real_scalar("CAT_helix_length_error"));
                    }
                  if(aux.fetch_boolean_scalar("CAT_has_helix_vertex"))
                    {
                      ws_->cathelixvertexx.push_back(aux.fetch_real_scalar("CAT_helix_vertex_x"));
                      ws_->cathelixvertexxerror.push_back(aux.fetch_real_scalar("CAT_helix_vertex_x_error"));
                      ws_->cathelixvertexy.push_back(aux.fetch_real_scalar("CAT_helix_vertex_y"));
                      ws_->cathelixvertexyerror.push_back(aux.fetch_real_scalar("CAT_helix_vertex_y_error"));
                      ws_->cathelixvertexz.push_back(aux.fetch_real_scalar("CAT_helix_vertex_z"));
                      ws_->cathelixvertexzerror.push_back(aux.fetch_real_scalar("CAT_helix_vertex_z_error"));
                    }
                  if(aux.fetch_boolean_scalar("CAT_has_helix_decay_vertex"))
                    {
                      ws_->cathelixdecayvertexx.push_back(aux.fetch_real_scalar("CAT_helix_decay_vertex_x"));
                      ws_->cathelixdecayvertexxerror.push_back(aux.fetch_real_scalar("CAT_helix_decay_vertex_x_error"));
                      ws_->cathelixdecayvertexy.push_back(aux.fetch_real_scalar("CAT_helix_decay_vertex_y"));
                      ws_->cathelixdecayvertexyerror.push_back(aux.fetch_real_scalar("CAT_helix_decay_vertex_y_error"));
                      ws_->cathelixdecayvertexz.push_back(aux.fetch_real_scalar("CAT_helix_decay_vertex_z"));
                      ws_->cathelixdecayvertexzerror.push_back(aux.fetch_real_scalar("CAT_helix_decay_vertex_z_error"));
                    }

                  if(aux.fetch_boolean_scalar("CAT_has_tangent_length"))
                    {
                      ws_->cattangentlength.push_back(aux.fetch_real_scalar("CAT_tangent_length"));
                      ws_->cattangentlengtherror.push_back(aux.fetch_real_scalar("CAT_tangent_length_error"));
                    }
                  if(aux.fetch_boolean_scalar("CAT_has_tangent_vertex"))
                    {
                      ws_->cattangentvertexx.push_back(aux.fetch_real_scalar("CAT_tangent_vertex_x"));
                      ws_->cattangentvertexxerror.push_back(aux.fetch_real_scalar("CAT_tangent_vertex_x_error"));
                      ws_->cattangentvertexy.push_back(aux.fetch_real_scalar("CAT_tangent_vertex_y"));
                      ws_->cattangentvertexyerror.push_back(aux.fetch_real_scalar("CAT_tangent_vertex_y_error"));
                      ws_->cattangentvertexz.push_back(aux.fetch_real_scalar("CAT_tangent_vertex_z"));
                      ws_->cattangentvertexzerror.push_back(aux.fetch_real_scalar("CAT_tangent_vertex_z_error"));
                    }
                  if(aux.fetch_boolean_scalar("CAT_has_tangent_decay_vertex"))
                    {
                      ws_->cattangentdecayvertexx.push_back(aux.fetch_real_scalar("CAT_tangent_decay_vertex_x"));
                      ws_->cattangentdecayvertexxerror.push_back(aux.fetch_real_scalar("CAT_tangent_decay_vertex_x_error"));
                      ws_->cattangentdecayvertexy.push_back(aux.fetch_real_scalar("CAT_tangent_decay_vertex_y"));
                      ws_->cattangentdecayvertexyerror.push_back(aux.fetch_real_scalar("CAT_tangent_decay_vertex_y_error"));
                      ws_->cattangentdecayvertexz.push_back(aux.fetch_real_scalar("CAT_tangent_decay_vertex_z"));
                      ws_->cattangentdecayvertexzerror.push_back(aux.fetch_real_scalar("CAT_tangent_decay_vertex_z_error"));
                    }
                  //cluster_.clusterid_ = &ws_-> clusterid[j];
                }
            }
          cluster_.clustersolutionid_ = &ws_-> clustersolutionid;
          cluster_.nounclusteredhits_ = &ws_-> nounclusteredhits;
          cluster_.noclusters_ = &ws_-> noclusters;
          cluster_.clusterid_ = &ws_-> clusterid;
          cluster_.nohits_ = &ws_-> nohits;
          cluster_.hitid_ = &ws_->clustertrackerid;
          cluster_.catcharge_ = &ws_-> catcharge;
          cluster_.catdetailedcharge_ = &ws_-> catdetailedcharge;
          cluster_.catchi2s_ = &ws_-> catchi2s;
          //cluster_.catchi2sall_ = &ws_-> catchi2sall;
          cluster_.catprobs_ = &ws_-> catprobs;
          //cluster_.catprobsall_ = &ws_-> catprobsall;
          cluster_.catpx_ = &ws_-> catpx;
          cluster_.catpy_ = &ws_-> catpy;
          cluster_.catpz_ = &ws_-> catpz;
          cluster_.cathelixcharge_ = &ws_-> cathelixcharge;
          cluster_.cathelixlength_ = &ws_-> cathelixlength;
          cluster_.cathelixlengtherror_ = &ws_-> cathelixlengtherror;
          cluster_.cathelixvertexx_ = &ws_-> cathelixvertexx;
          cluster_.cathelixvertexxerror_ = &ws_-> cathelixvertexxerror;
          cluster_.cathelixvertexy_ = &ws_-> cathelixvertexy;
          cluster_.cathelixvertexyerror_ = &ws_-> cathelixvertexyerror;
          cluster_.cathelixvertexz_ = &ws_-> cathelixvertexz;
          cluster_.cathelixvertexzerror_ = &ws_-> cathelixvertexzerror;
          cluster_.cathelixdecayvertexx_ = &ws_-> cathelixdecayvertexx;
          cluster_.cathelixdecayvertexxerror_ = &ws_-> cathelixdecayvertexxerror;
          cluster_.cathelixdecayvertexy_ = &ws_-> cathelixdecayvertexy;
          cluster_.cathelixdecayvertexyerror_ = &ws_-> cathelixdecayvertexyerror;
          cluster_.cathelixdecayvertexz_ = &ws_-> cathelixdecayvertexz;
          cluster_.cathelixdecayvertexzerror_ = &ws_-> cathelixdecayvertexzerror;
          cluster_.cattangentvertexx_ = &ws_-> cattangentvertexx;
          cluster_.cattangentvertexxerror_ = &ws_-> cattangentvertexxerror;
          cluster_.cattangentvertexy_ = &ws_-> cattangentvertexy;
          cluster_.cattangentvertexyerror_ = &ws_-> cattangentvertexyerror;
          cluster_.cattangentvertexz_ = &ws_-> cattangentvertexz;
          cluster_.cattangentvertexzerror_ = &ws_-> cattangentvertexzerror;
          cluster_.cattangentdecayvertexx_ = &ws_-> cattangentdecayvertexx;
          cluster_.cattangentdecayvertexxerror_ = &ws_-> cattangentdecayvertexxerror;
          cluster_.cattangentdecayvertexy_ = &ws_-> cattangentdecayvertexy;
          cluster_.cattangentdecayvertexyerror_ = &ws_-> cattangentdecayvertexyerror;
          cluster_.cattangentdecayvertexz_ = &ws_-> cattangentdecayvertexz;
          cluster_.cattangentdecayvertexzerror_ = &ws_-> cattangentdecayvertexzerror;
          //cluster_.cathelixchi2s_ = &ws_-> cathelixchi2s;
          cluster_.cattangentlength_ = &ws_-> cattangentlength;
          cluster_.cattangentlengtherror_ = &ws_-> cattangentlengtherror;
        }
    }

  // look for trajectory data
  if(workItem.has("TTD"))
    {
      const snemo::datamodel::tracker_trajectory_data & TTD = workItem.get<snemo::datamodel::tracker_trajectory_data>("TTD");
      if(TTD.has_solutions())
        {
          trajectory_.nosolutions_ = TTD.get_number_of_solutions();
          int nsolutions = TTD.get_number_of_solutions();
          for (int i = 0; i < nsolutions; ++i)
            {
              const snemo::datamodel::tracker_trajectory_solution & solution = TTD.get_solution(i);
              ws_->trajectorysolutionid.push_back(solution.get_solution_id());
              ws_->notrajectories.push_back(solution.get_trajectories().size());
              int ntrajectories = solution.get_trajectories().size();
              ws_->nounfittedclusters.push_back(solution.get_unfitted_clusters().size());
              for (int j = 0; j < ntrajectories; ++j)
                {
                  const snemo::datamodel::tracker_trajectory & traj = solution.get_trajectories()[j].get();
                  const datatools::properties & aux = traj.get_auxiliaries();
                  if(traj.has_trajectory_id()) ws_->trajectoryid.push_back(traj.get_trajectory_id());
                  ws_->chi2.push_back(aux.fetch_real_scalar("chi2"));
                  ws_->ndof.push_back(aux.fetch_integer_scalar("ndof"));
                  ws_->trajectorylength.push_back(traj.get_pattern().get_shape().get_length());
                  ws_->nounfittedhits.push_back(traj.get_orphans().size());
                  ws_->trajectoryclusterid.push_back(traj.get_cluster().get_cluster_id());
                }
            }
          trajectory_.trajectorysolutionid_ = &ws_-> trajectorysolutionid;
          trajectory_.notrajectories_ = &ws_-> notrajectories;
          trajectory_.nounfittedclusters_ = &ws_-> nounfittedclusters;
          trajectory_.nounfittedhits_ = &ws_-> nounfittedhits;
          trajectory_.trajectoryid_ = &ws_-> trajectoryid;
          trajectory_.clusterid_ = &ws_-> trajectoryclusterid;
          trajectory_.chi2_ = &ws_-> chi2;
          trajectory_.ndof_ = &ws_-> ndof;
          trajectory_.length_ = &ws_-> trajectorylength;
        }
    }

  //Look for charged particle tracker data
  if(workItem.has("PTD"))
    {
      const snemo::datamodel::particle_track_data & PTD = workItem.get<snemo::datamodel::particle_track_data>("PTD");
      particle_.noparticles_ = PTD.get_number_of_particles();
      int nparticles = PTD.get_number_of_particles();
      for(int i = 0; i < nparticles; ++i)
        {
          const snemo::datamodel::particle_track & the_particle = PTD.get_particle(i);
          ws_-> charge.push_back(the_particle.get_charge());
          ws_-> nocalohits.push_back(the_particle.get_associated_calorimeter_hits().size());
          int ncalohits = the_particle.get_associated_calorimeter_hits().size();
          for (int j = 0; j < ncalohits; ++j)
            {
              const snemo::datamodel::calibrated_calorimeter_hit & calo_hit = the_particle.get_associated_calorimeter_hits()[j].get();
              ws_-> energy.push_back(calo_hit.get_energy());
              ws_-> particlecalotime.push_back(calo_hit.get_time());
            }
          ws_-> novertices.push_back(the_particle.get_vertices().size());
          int nvertices = the_particle.get_vertices().size();
          for (int j = 0; j < nvertices; ++j)
            {
              const geomtools::blur_spot vertex = the_particle.get_vertices()[j].get();
              ws_-> vertexx.push_back(vertex.get_position().getX());
              ws_-> vertexxerror.push_back(vertex.get_x_error());
              ws_-> vertexy.push_back(vertex.get_position().y());
              ws_-> vertexyerror.push_back(vertex.get_y_error());
              ws_-> vertexz.push_back(vertex.get_position().z());
              ws_-> vertexzerror.push_back(vertex.get_z_error());
            }
          ws_-> particletrajid.push_back(the_particle.get_trajectory().get_trajectory_id());
        }
      particle_.charge_ = &ws_-> charge;
      particle_.nocalohits_ = &ws_-> nocalohits;
      particle_.energy_ = &ws_-> energy;
      particle_.calotime_ = &ws_-> particlecalotime;
      particle_.novertices_ = &ws_-> novertices;
      particle_.vertexx_ = &ws_-> vertexx;
      particle_.vertexy_ = &ws_-> vertexy;
      particle_.vertexz_ = &ws_-> vertexz;
      particle_.vertexxerror_ = &ws_-> vertexxerror;
      particle_.vertexyerror_ = &ws_-> vertexyerror;
      particle_.vertexzerror_ = &ws_-> vertexzerror;
      particle_.trajectoryid_ = &ws_-> particletrajid;
    }
  
  tree_->Fill();

  // MUST return a status, see ref dpp::processing_status_flags_type 
  return dpp::base_module::PROCESS_OK;
}

// Reset
void Things2Root::reset() {
  // Throw logic exception if we've not initialized this instance
  DT_THROW_IF(!this->is_initialized(),
              std::logic_error,
              "Things2Root not initialized");
  this->_set_initialized(false);
  if (ws_) {
    delete ws_;
    ws_ = 0;
  }
  if (hfile_) {
    // write the output, finished streaming
    hfile_->cd();
    tree_->Write();
    hfile_->Close(); //
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Finished conversion, file closed.");

    // clean up
    delete hfile_;
    tree_ = 0;
    hfile_ = 0;
  }
  geometry_manager_ = 0;
  filename_output_ = "things2root.default.root";
}
