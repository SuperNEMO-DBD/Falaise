#include "SensitivityModule.h"

const double highEnergyLimit=0.150;// 150 keV
const double lowEnergyLimit=0.050; // 50 keV
//const double electronMass=0.5109989461; // From pdg
const double speedOfLight=299792458 * 1e-9 * 1000; // Millimeters per nanosecond

int mainWallHitType=1302;
int xWallHitType=1232;
int gammaVetoHitType=1252;

using namespace std;

DPP_MODULE_REGISTRATION_IMPLEMENT(SensitivityModule,"SensitivityModule")

SensitivityModule::SensitivityModule()
: dpp::base_module()
{
  filename_output_="sensitivity.root";
}

SensitivityModule::~SensitivityModule()
{
  if (is_initialized()) this->reset();
}

void SensitivityModule::initialize(const datatools::properties & myConfig,
                                   datatools::service_manager & flServices,
                                   dpp::module_handle_dict_type & /*moduleDict*/)
{

  // Look for services
  if (flServices.has("geometry")) {
    const geomtools::geometry_service& GS = flServices.get<geomtools::geometry_service> ("geometry");
    geometry_manager_ = &GS.get_geom_manager();
    DT_THROW_IF(!geometry_manager_,
                std::runtime_error,
                "Null pointer to geometry manager return by geometry_service");
  }
  // Extract the filename_out key from the supplied config, if
  // the key exists. datatools::properties throws an exception if
  // the key isn't in the config, so catch this if thrown and don't do
  // anything
  try {
    myConfig.fetch("filename_out",this->filename_output_);
  } catch (std::logic_error& e) {
  }

  // Use the method of PTD2ROOT to create a root file with just the branches we need for the sensitivity analysis


  hfile_ = new TFile(filename_output_.c_str(),"RECREATE","Output file of Simulation data");
  hfile_->cd();
  tree_ = new TTree("Sensitivity","Sensitivity");
  tree_->SetDirectory(hfile_);

  // Reconstructed quantities

  // Standard cuts
  tree_->Branch("reco.passes_two_calorimeters",&sensitivity_.passes_two_calorimeters_);
  tree_->Branch("reco.passes_two_plus_calos",&sensitivity_.passes_two_plus_calos_);
  tree_->Branch("reco.passes_two_clusters",&sensitivity_.passes_two_clusters_);
  tree_->Branch("reco.passes_two_tracks",&sensitivity_.passes_two_tracks_);
  tree_->Branch("reco.passes_associated_calorimeters",&sensitivity_.passes_associated_calorimeters_);

  // Some basic counts
  tree_->Branch("reco.calorimeter_hit_count",&sensitivity_.calorimeter_hit_count_);
  tree_->Branch("reco.cluster_count",&sensitivity_.cluster_count_);
  tree_->Branch("reco.track_count",&sensitivity_.track_count_);
  tree_->Branch("reco.associated_track_count",&sensitivity_.associated_track_count_);
  tree_->Branch("reco.small_cluster_count",&sensitivity_.small_cluster_count_);
  tree_->Branch("reco.delayed_hit_count",&sensitivity_.delayed_hit_count_);

  // Numbers of reconstructed particles
  tree_->Branch("reco.number_of_electrons",&sensitivity_.number_of_electrons_);
  tree_->Branch("reco.electron_charges",&sensitivity_.electron_charges_);
  tree_->Branch("reco.number_of_gammas",&sensitivity_.number_of_gammas_);
  tree_->Branch("reco.alpha_count",&sensitivity_.alpha_count_);

  // Energies
  tree_->Branch("reco.total_calorimeter_energy",&sensitivity_.total_calorimeter_energy_);
  tree_->Branch("reco.higher_electron_energy",&sensitivity_.higher_electron_energy_);
  tree_->Branch("reco.lower_electron_energy",&sensitivity_.lower_electron_energy_);
  tree_->Branch("reco.electron_energies",&sensitivity_.electron_energies_);
  tree_->Branch("reco.gamma_energies",&sensitivity_.gamma_energies_);
  tree_->Branch("reco.highest_gamma_energy",&sensitivity_.highest_gamma_energy_);

  // Electron track lengths
  tree_->Branch("reco.electron_track_lengths",&sensitivity_.electron_track_lengths_);
  tree_->Branch("reco.electron_hit_counts",&sensitivity_.electron_hit_counts_);

  // Vertex positions (max 2 tracks)
  tree_->Branch("reco.first_vertex_x",&sensitivity_.first_vertex_x_);
  tree_->Branch("reco.first_vertex_y",&sensitivity_.first_vertex_y_);
  tree_->Branch("reco.first_vertex_z",&sensitivity_.first_vertex_z_);
  tree_->Branch("reco.second_vertex_x",&sensitivity_.second_vertex_x_);
  tree_->Branch("reco.second_vertex_y",&sensitivity_.second_vertex_y_);
  tree_->Branch("reco.second_vertex_z",&sensitivity_.second_vertex_z_);
  tree_->Branch("reco.first_proj_vertex_y",&sensitivity_.first_proj_vertex_y_);
  tree_->Branch("reco.first_proj_vertex_z",&sensitivity_.first_proj_vertex_z_);
  tree_->Branch("reco.second_proj_vertex_y",&sensitivity_.second_proj_vertex_y_);
  tree_->Branch("reco.second_proj_vertex_z",&sensitivity_.second_proj_vertex_z_);
  tree_->Branch("reco.vertex_separation",&sensitivity_.vertex_separation_);
  tree_->Branch("reco.foil_projection_separation",&sensitivity_.foil_projection_separation_);
  tree_->Branch("reco.projection_distance_xy",&sensitivity_.projection_distance_xy_);
  tree_->Branch("reco.foil_vertex_count",&sensitivity_.foil_vertex_count_);
  //tree_->Branch("reco.vertices_on_foil",&sensitivity_.vertices_on_foil_);
  tree_->Branch("reco.vertices_in_tracker",&sensitivity_.vertices_in_tracker_);
  tree_->Branch("reco.electrons_from_foil",&sensitivity_.electrons_from_foil_);
  tree_->Branch("reco.electron_vertex_x",&sensitivity_.electron_vertex_x_); // vector
  tree_->Branch("reco.electron_vertex_y",&sensitivity_.electron_vertex_y_); // vector
  tree_->Branch("reco.electron_vertex_z",&sensitivity_.electron_vertex_z_); // vector
  tree_->Branch("reco.electron_dir_x",&sensitivity_.electron_dir_x_); // vector
  tree_->Branch("reco.electron_dir_y",&sensitivity_.electron_dir_y_); // vector
  tree_->Branch("reco.electron_dir_z",&sensitivity_.electron_dir_z_); // vector
  tree_->Branch("reco.electron_proj_vertex_x",&sensitivity_.electron_proj_vertex_x_); // vector
  tree_->Branch("reco.electron_proj_vertex_y",&sensitivity_.electron_proj_vertex_y_); // vector
  tree_->Branch("reco.electron_proj_vertex_z",&sensitivity_.electron_proj_vertex_z_); // vector
  tree_->Branch("reco.alpha_vertex_x",&sensitivity_.alpha_vertex_x_); // vector
  tree_->Branch("reco.alpha_vertex_y",&sensitivity_.alpha_vertex_y_); // vector
  tree_->Branch("reco.alpha_vertex_z",&sensitivity_.alpha_vertex_z_); // vector
  tree_->Branch("reco.alpha_dir_x",&sensitivity_.alpha_dir_x_); // vector
  tree_->Branch("reco.alpha_dir_y",&sensitivity_.alpha_dir_y_); // vector
  tree_->Branch("reco.alpha_dir_z",&sensitivity_.alpha_dir_z_); // vector
  tree_->Branch("reco.alpha_proj_vertex_x",&sensitivity_.alpha_proj_vertex_x_); // vector
  tree_->Branch("reco.alpha_proj_vertex_y",&sensitivity_.alpha_proj_vertex_y_); // vector
  tree_->Branch("reco.alpha_proj_vertex_z",&sensitivity_.alpha_proj_vertex_z_); // vector
  tree_->Branch("reco.alphas_from_foil",&sensitivity_.alphas_from_foil_);
  tree_->Branch("reco.edgemost_vertex",&sensitivity_.edgemost_vertex_);

  // Topologies
  tree_->Branch("reco.topology_1e1gamma",&sensitivity_.topology_1e1gamma_);
  tree_->Branch("reco.topology_1e1alpha",&sensitivity_.topology_1e1alpha_);
  tree_->Branch("reco.topology_1engamma",&sensitivity_.topology_1engamma_);
  tree_->Branch("reco.topology_2e",&sensitivity_.topology_2e_);
  tree_->Branch("reco.topology_1e",&sensitivity_.topology_1e_);


  // Multi-track topology info
  tree_->Branch("reco.angle_between_tracks",&sensitivity_.angle_between_tracks_);
  tree_->Branch("reco.same_side_of_foil",&sensitivity_.same_side_of_foil_);
  tree_->Branch("reco.first_track_direction_x",&sensitivity_.first_track_direction_x_);
  tree_->Branch("reco.first_track_direction_y",&sensitivity_.first_track_direction_y_);
  tree_->Branch("reco.first_track_direction_z",&sensitivity_.first_track_direction_z_);
  tree_->Branch("reco.second_track_direction_x",&sensitivity_.second_track_direction_x_);
  tree_->Branch("reco.second_track_direction_y",&sensitivity_.second_track_direction_y_);
  tree_->Branch("reco.second_track_direction_z",&sensitivity_.second_track_direction_z_);
  tree_->Branch("reco.internal_probability",&sensitivity_.internal_probability_);
  tree_->Branch("reco.external_probability",&sensitivity_.external_probability_);
  tree_->Branch("reco.foil_projected_internal_probability",&sensitivity_.foil_projected_internal_probability_);
  tree_->Branch("reco.foil_projected_external_probability",&sensitivity_.foil_projected_external_probability_);
  tree_->Branch("reco.calo_hit_time_separation",&sensitivity_.calo_hit_time_separation_);

  // Alpha finding
  tree_->Branch("reco.delayed_track_time",&sensitivity_.delayed_track_time_);
  tree_->Branch("reco.delayed_cluster_hit_count",&sensitivity_.delayed_cluster_hit_count_);
  tree_->Branch("reco.foil_alpha_count",&sensitivity_.foil_alpha_count_);
  tree_->Branch("reco.alpha_track_length",&sensitivity_.alpha_track_length_);
  tree_->Branch("reco.proj_track_length_alpha",&sensitivity_.proj_track_length_alpha_);
  tree_->Branch("reco.alpha_crosses_foil",&sensitivity_.alpha_crosses_foil_);


  // Calorimeter positions
  tree_->Branch("reco.electron_hits_mainwall",&sensitivity_.electron_hits_mainwall_);
  tree_->Branch("reco.electron_hits_xwall",&sensitivity_.electron_hits_xwall_);
  tree_->Branch("reco.electron_hits_gveto",&sensitivity_.electron_hits_gveto_);
  tree_->Branch("reco.gamma_hits_mainwall",&sensitivity_.gamma_hits_mainwall_);
  tree_->Branch("reco.gamma_hits_xwall",&sensitivity_.gamma_hits_xwall_);
  tree_->Branch("reco.gamma_hits_gveto",&sensitivity_.gamma_hits_gveto_);
  tree_->Branch("reco.gamma_fractions_mainwall",&sensitivity_.gamma_fractions_mainwall_);
  tree_->Branch("reco.gamma_fractions_xwall",&sensitivity_.gamma_fractions_xwall_);
  tree_->Branch("reco.gamma_fractions_gveto",&sensitivity_.gamma_fractions_gveto_);

  // Truth quantities (only applicable to simulation)
  tree_->Branch("true.highest_primary_energy",&sensitivity_.true_highest_primary_energy_);
  tree_->Branch("true.second_primary_energy",&sensitivity_.true_second_primary_energy_);
  tree_->Branch("true.higher_particle_type",&sensitivity_.true_higher_particle_type_);
  tree_->Branch("true.lower_particle_type",&sensitivity_.true_lower_particle_type_);
  tree_->Branch("true.total_energy",&sensitivity_.true_total_energy_);
  tree_->Branch("true.vertex_x",&sensitivity_.true_vertex_x_);
  tree_->Branch("true.vertex_y",&sensitivity_.true_vertex_y_);
  tree_->Branch("true.vertex_z",&sensitivity_.true_vertex_z_);


  this->_set_initialized(true);
}

//! [SensitivityModule::Process]
dpp::base_module::process_status
SensitivityModule::process(datatools::things & workItem)
{


  // internal variables to mimic the ntuple variables, names are same but in camel case
  bool passesTwoCalorimeters=false;
  bool passesTwoPlusCalos=false;
  bool passesTwoClusters=false;
  bool passesTwoTracks=false;
  bool passesAssociatedCalorimeters=false;
  //uint numberOfElectrons=0;
  uint numberOfGammas=0;
  double totalCalorimeterEnergy=0;
  double higherElectronEnergy=0;
  double lowerElectronEnergy=0;
  int verticesOnFoil=0;
  int verticesInTracker=0;
  //int firstVerticesOnFoil=0;
  double timeDelay=-1;
  bool is2electron=false;
  bool is1electron=false;
  double internalProbability=-1;
  //double internalChiSquared=-1;
  //double externalChiSquared=-1;
  double externalProbability=-1;
  double foilProjectedExternalProbability=-1;
  double foilProjectedInternalProbability=-1;
  bool is1e1gamma=false;
  bool is1engamma=false;
  bool is1e1alpha=false;
  //double electrongammaInternalProbability=-1;
  //double electrongammaExternalProbability=-1;
  //double electrongammaProjInternalProb=-1;
  //double electrongammaProjExternalProb=-1;
  double higherTrueEnergy=0;
  double lowerTrueEnergy=0;
  double totalTrueEnergy=0;
  int higherTrueType=0;
  int lowerTrueType=0;
  double  trueVertexX=-9999;
  double  trueVertexY=-9999;
  double  trueVertexZ=-9999;
  int clusterCount=0;
  int trackCount=0;
  //int alphaCount=0;
  int delayedHitCount=0;
  int foilAlphaCount=0;
  //int associatedTrackCount=0;
  int smallClusterCount=0;
  //double delayedClusterHitCount=0;
  //double trackLengthAlpha=0;
  //double projectedTrackLengthAlpha=0;
  //double maxAlphaTime=-1;
  int caloHitCount=0;
  //int foilVertexCount=0;
  double highestGammaEnergy=0;
  double edgemostVertex=0;
  //double distanceBetweenFoilGeigerCell=30.838;

  std::vector<snemo::datamodel::particle_track> gammaCandidates;
  std::vector<snemo::datamodel::particle_track> electronCandidates;
  std::vector<snemo::datamodel::particle_track> alphaCandidates;
  std::vector<TrackDetails> gammaCandidateDetails;
  std::vector<TrackDetails> electronCandidateDetails;
  std::vector<TrackDetails> alphaCandidateDetails;


  std::vector<double> gammaEnergies;
  std::vector<double> electronEnergies;
  std::vector<int> electronCharges;
  std::vector<double> electronTrackLengths;
  std::vector<double> electronProjTrackLengths;
  std::vector<int> electronHitCounts;
  std::vector<bool> electronsFromFoil;
  std::vector<int> alphaHitCounts;
  std::vector<bool> alphasFromFoil;

  std::vector<int> electronCaloType; // will be translated to the vectors for each type at the end
  std::vector<int> gammaCaloType; // will be translated to the vectors for each type at the end

  // To calculate the fraction of the energy from each particle that is deposited in each of the calorimeter walls
  std::vector<double>electronMainwallFraction;
  std::vector<double>electronXwallFraction;
  std::vector<double>electronVetoFraction;
  std::vector<double>gammaMainwallFraction;
  std::vector<double>gammaXwallFraction;
  std::vector<double>gammaVetoFraction;

  std::vector<double> trajClDelayedTime;

  std::vector<TVector3> electronVertices;
  std::vector<TVector3> electronDirections;
  std::vector<TVector3> electronProjVertices;
  std::vector<TVector3> alphaVertices;
  std::vector<TVector3> alphaDirections;
  std::vector<TVector3> alphaProjVertices;


  TVector3 gammaDirection; // We are only calculating this for the highest-energy gamma right now
  gammaDirection.SetXYZ(0,0,0);

  //double angleBetweenTracks;
  //bool sameSideOfFoil=false;
  //bool edgemostJoinedElectron=false;
  double projectionDistanceXY=0;

  //uint highEnergyIndex = 0;
  // Grab calibrated data bank
  // Calibrated data will only be present in reconstructed files,
  // so wrap in a try block


  try {
    const snemo::datamodel::calibrated_data& calData = workItem.get<snemo::datamodel::calibrated_data>("CD");

    int nCalorimeterHits=0;
    int nCalHitsOverHighLimit=0;
    int nCalHitsOverLowLimit=0;

    //if (calData.has_calibrated_calorimeter_hits())
    {
#ifdef NEW_DATAMODEL_API
      const snemo::datamodel::CalorimeterHitHdlCollection & calHits=calData.calorimeter_hits();
#else
      const snemo::datamodel::calibrated_calorimeter_hit::collection_type& calHits = calData.calibrated_calorimeter_hits();
#endif

      for (const auto& iHit : calHits) {
        const snemo::datamodel::calibrated_calorimeter_hit & calHit = iHit.get();
        double energy=calHit.get_energy() ;
        totalCalorimeterEnergy += energy;

        if (timeDelay<0)//first hit
          {
            timeDelay=calHit.get_time();
          }
        else
          {
            timeDelay -=calHit.get_time(); // Get time between the first two hits: if there are more than two hits we will reject the event anyway
          }
        ++nCalorimeterHits;
        if (energy>=highEnergyLimit)++nCalHitsOverHighLimit;
        if (energy>=lowEnergyLimit)++nCalHitsOverLowLimit;

      }
    }

    caloHitCount=nCalHitsOverLowLimit;
    if (nCalorimeterHits==2 && nCalHitsOverHighLimit>=1 && nCalHitsOverLowLimit==2)
      {
        passesTwoCalorimeters=true;
      }
    if (nCalHitsOverHighLimit>=1 && nCalHitsOverLowLimit>=2)
      {
        passesTwoPlusCalos=true;
      }
    //if (calData.has_calibrated_tracker_hits())
    {
      // Count the delayed tracker hits by looping all the tracker hits and checking if they are delayed
#ifdef NEW_DATAMODEL_API
      const snemo::datamodel::TrackerHitHdlCollection& trackerHits = calData.tracker_hits();
#else
      const snemo::datamodel::calibrated_tracker_hit::collection_type& trackerHits = calData.calibrated_tracker_hits();
#endif

      for (const auto& iHit : trackerHits) {
        const snemo::datamodel::calibrated_tracker_hit& hit = iHit.get();
        if (hit.is_delayed()) delayedHitCount++;
      }

    }
  }
  catch (std::logic_error& e) {
    std::cerr << "failed to grab CD bank : " << e.what() << std::endl;
    return dpp::base_module::PROCESS_INVALID;
  }

  // Number of tracker clusters comes from the TCD databank
  // We want two clusters of three cells
  try {
    const snemo::datamodel::tracker_clustering_data& clusterData = workItem.get<snemo::datamodel::tracker_clustering_data>("TCD");

#ifdef NEW_DATAMODEL_API
    bool clusterHasDefaultSolution = clusterData.has_default();
#else
    bool clusterHasDefaultSolution = clusterData.has_default_solution();
#endif

    if (clusterHasDefaultSolution) // Looks as if there is a possibility of alternative solutions. Is it sufficient to use the default?
      {
#ifdef NEW_DATAMODEL_API
        snemo::datamodel::tracker_clustering_solution solution = clusterData.get_default() ;
        const snemo::datamodel::TrackerClusterHdlCollection& clusters=solution.get_clusters();
#else
        snemo::datamodel::tracker_clustering_solution solution = clusterData.get_default_solution();
        const snemo::datamodel::tracker_clustering_solution::cluster_col_type& clusters = solution.get_clusters();
#endif

        for (const auto& iCluster : clusters)
          {
            const snemo::datamodel::tracker_cluster & cluster = iCluster.get();

#ifdef NEW_DATAMODEL_API
            bool hasAtLeastMinHits = (cluster.size() >= minHitsInCluster);
#else
            bool hasAtLeastMinHits = (cluster.get_number_of_hits() >= minHitsInCluster);
#endif

            if (hasAtLeastMinHits) {
              ++clusterCount;
            }
            else {
              ++smallClusterCount;
            }
          }
      }
    if (clusterCount==2 )
      {
        passesTwoClusters=true;
      }
  }
  catch (std::logic_error& e) {
    std::cerr << "failed to grab TCD bank : " << e.what() << std::endl;
    return dpp::base_module::PROCESS_INVALID;
  }


  // Number of particle tracks PTD databank
  // We want two particle tracks to calculate 2e internal/external probability
  // If we have one track and a remote hit, we can calculate 1e1gamma probabilities

  try
    {
      const snemo::datamodel::particle_track_data& trackData = workItem.get<snemo::datamodel::particle_track_data>("PTD");


#ifdef NEW_DATAMODEL_API
      bool havePTDParticles = trackData.hasParticles();
#else
      bool havePTDParticles = trackData.has_particles();
#endif

      if (havePTDParticles)
        {
#ifdef NEW_DATAMODEL_API
          const snemo::datamodel::ParticleHdlCollection& particles = trackData.particles();
#else
          const snemo::datamodel::particle_track_data::particle_collection_type& particles = trackData.get_particles();
#endif

          for (const auto& iParticle : particles) {
            const snemo::datamodel::particle_track& track = iParticle.get();

            TrackDetails trackDetails(geometry_manager_, track);

            // Populate info for gammas
            if (trackDetails.IsGamma())
              {
                numberOfGammas++;
                int pos=InsertAndGetPosition(trackDetails.GetEnergy(), gammaEnergies, true); // Add energy to ordered list of gamma energies (highest first) and get where in the list it was added
                // Now add the type of the first hit to a vector
                InsertAt(trackDetails.GetFirstHitType(), gammaCaloType, pos);
                // And the fraction of the energy deposited in each wall
                InsertAt(trackDetails.GetMainwallFraction(), gammaMainwallFraction,pos);
                InsertAt(trackDetails.GetXwallFraction(), gammaXwallFraction,pos);
                InsertAt(trackDetails.GetVetoFraction(), gammaVetoFraction,pos);
                InsertAt(track,gammaCandidates,pos);
                InsertAt(trackDetails,gammaCandidateDetails,pos);
                continue;
              }

            if (trackDetails.MakesTrack()) trackCount++;
            else continue;


            // Now we have only charged particles remaining there are a few things we can do:
            // Identify electron candidates
            // Identify alpha candidates
            // Get edgemost inner vertex, regardless of whether they have associated calorimeters etc

            // First the vertex:

            // Count the number of vertices on the foil or on the wires (aka vertices in the tracker)
            if (trackDetails.HasTrackerVertex())verticesInTracker++;

            // Count the number of vertices on the foil only
            if (trackDetails.HasFoilVertex())verticesOnFoil++;

            // For all the tracks in the event, which one has its foilmost vertex nearest the tunnel/mountain
            // edge of the foil? We could use this to identify
            // Events so near the edge they can't make a 3-cell track

            double thisY = trackDetails.GetFoilmostVertexY();
            if (TMath::Abs(thisY) > TMath::Abs(edgemostVertex)) edgemostVertex = thisY;

            // For electron candidates, we need to store the energies
            if (trackDetails.IsElectron())
              {
                int pos=InsertAndGetPosition(trackDetails.GetEnergy(), electronEnergies, true);
                // Add energy to ordered list of electron energies (highest first)
                // and get where in the list it was added
                // Now add the type of the first hit to a vector (electrons are currently only allowed one hit)
                // If we allow clustered hits for an electron, we can easily add it in this framework
                InsertAt(trackDetails.GetFirstHitType(), electronCaloType, pos);
                // Vector of electron candidates is ordered
                InsertAt(track,electronCandidates,pos);
                InsertAt(trackDetails,electronCandidateDetails,pos);
                // And we also want a vector of electron charges (they might be positrons)
                InsertAt(trackDetails.GetCharge(),electronCharges,pos);
                // And whether or not they are from the foil only
                InsertAt(trackDetails.HasFoilVertex(),electronsFromFoil,pos);
                // Vertices, directions, and vertices if projected back to foil
                InsertAt(trackDetails.GetFoilmostVertex(),electronVertices,pos);
                InsertAt(trackDetails.GetProjectedVertex(),electronProjVertices,pos);
                InsertAt(trackDetails.GetDirection(),electronDirections,pos);
                InsertAt(trackDetails.GetTrackLength(),electronTrackLengths,pos);
                InsertAt(trackDetails.GetProjectedTrackLength(),electronProjTrackLengths,pos);
                InsertAt(trackDetails.GetTrackerHitCount(),electronHitCounts,pos);
              }

            // Now look for alpha candidates
            if (trackDetails.IsAlpha())
              {
                // Add delay time to ordered list of alpha times (highest first)
                // and get where in the list it was added
                int pos=InsertAndGetPosition(trackDetails.GetDelayTime(), trajClDelayedTime, true);
                // Now add rest of the properties to the list
                // Vector of electron candidates is ordered
                InsertAt(track,alphaCandidates,pos);
                InsertAt(trackDetails,alphaCandidateDetails,pos);
                // Vertex and direction info
                InsertAt(trackDetails.GetFoilmostVertex(),alphaVertices,pos);
                InsertAt(trackDetails.GetDirection(),alphaDirections,pos);
                InsertAt(trackDetails.GetProjectedVertex(),alphaProjVertices,pos);
                // And whether or not they are from the foil
                InsertAt(trackDetails.HasFoilVertex(),alphasFromFoil,pos);
                if (trackDetails.HasFoilVertex()) foilAlphaCount++;

                InsertAt(trackDetails.GetTrackerHitCount(),alphaHitCounts,pos);
              }
          } // end for each particle
        } // end if has particles

      //---------------------------------------
      // Now identify topologies
      //---------------------------------------
      if (electronCandidates.size()==2 && trackCount==2)
        { // at the moment - gammas allowed
          is2electron = true;
        }
      if (electronCandidates.size()==1 && numberOfGammas>=1 && trackCount==1)
        {
          is1engamma = true;
          if (numberOfGammas==1) is1e1gamma = true;
        }
      if (electronCandidates.size()==1 && numberOfGammas==0 && trackCount==1)
        {
          is1electron = true;
        }

      if (electronCandidates.size() ==1 && alphaCandidates.size() ==1 && trackCount==2)
        { // gammas allowed
          is1e1alpha = true;
        }


      //---------------------------------------
      // Combined info for the topologies
      //---------------------------------------

      // Calculate values for the 1e1alpha topology
      // Want to iterate over the tracks in the electronCandidate and alphaCandidate vectors
      if (is1e1alpha)
        {
          // Recalculate the projected track length based on the electron track
          alphaCandidateDetails.at(0).GenerateAlphaProjections(&electronCandidateDetails.at(0));
        }

      // For 2-electron and 1-e-n-gamma events, calculate some internal and external probablilities:
      // that the two particles originated at the same time from the foil (internal) or that
      // one went to the foil and ejected the other from it (external)
      if (is2electron || is1engamma)
        {
          std::vector<TrackDetails*> twoParticles;
          twoParticles.push_back(&electronCandidateDetails.at(0));
          if (is1engamma)
            {
              // Set the track length for the highest-energy gamma based on it sharing a vertex with the electron
              gammaCandidateDetails.at(0).GenerateGammaTrackLengths(&electronCandidateDetails.at(0));
              gammaDirection=gammaCandidateDetails.at(0).GenerateGammaTrackDirection(&electronCandidateDetails.at(0));
              twoParticles.push_back(&gammaCandidateDetails.at(0));
            }
          if (is2electron) // Second particle is the second electron
            {
              passesAssociatedCalorimeters=true;
              twoParticles.push_back(&electronCandidateDetails.at(1));
            }
          CalculateProbabilities(internalProbability, externalProbability,  twoParticles,  false);
          CalculateProbabilities(foilProjectedInternalProbability, foilProjectedExternalProbability,  twoParticles,  true);
        }// end if either 2e or 1e n gamma
    }// end try on PTD bank
  catch (std::logic_error& e) {
    std::cerr << "failed to grab PTD bank : " << e.what() << std::endl;
    return dpp::base_module::PROCESS_INVALID;
  } //end catch

  // From SD bank (simulated data - i.e. generator level):
  // Get (true) energy of two most energetic particles
  // Get (true) primary vertex position

  try
    {
      const mctools::simulated_data& simData = workItem.get<mctools::simulated_data>("SD");
      if (simData.has_data())
        {
          trueVertexX = simData.get_vertex().x();
          trueVertexY = simData.get_vertex().y();
          trueVertexZ= simData.get_vertex().z();
          mctools::simulated_data::primary_event_type primaryEvent=simData.get_primary_event ();

          for (size_t i=0;i<primaryEvent.get_number_of_particles();++i)// should be 2 particles for 0nubb
            {
              genbb::primary_particle trueParticle= primaryEvent.get_particle(i);
              double energy=trueParticle.get_kinetic_energy();
              double type=trueParticle.get_type();
              totalTrueEnergy += energy;
              // Populate the two highest true energies
              if (energy > higherTrueEnergy)
                {
                  lowerTrueEnergy=higherTrueEnergy;
                  lowerTrueType=higherTrueType;
                  higherTrueEnergy=energy;
                  higherTrueType=type;
                }
              else if (energy > lowerTrueEnergy)
                {
                  lowerTrueEnergy=energy;
                  lowerTrueType=type;
                }
            }
        }
    } // end try for SD bank
  catch (std::logic_error& e) {
    //std::cerr << "failed to grab SD bank : " << e.what() << std::endl;
    //return dpp::base_module::PROCESS_ERROR;
    // This is OK, if it's data there will be no SD bank
  }     // end catch for SD bank

  passesTwoTracks = (trackCount==2);
  // Sort the electron energies, largest first
  std::sort (electronEnergies.begin(), electronEnergies.end());
  std::reverse (electronEnergies.begin(), electronEnergies.end());
  // Sort the electron energies, largest first
  std::sort (gammaEnergies.begin(), gammaEnergies.end());
  std::reverse (gammaEnergies.begin(), gammaEnergies.end());

  higherElectronEnergy=0;
  lowerElectronEnergy=0;
  if (electronCandidates.size()>0) higherElectronEnergy=electronEnergies.at(0);
  if (electronCandidates.size()>1) lowerElectronEnergy=electronEnergies.at(1);

  highestGammaEnergy=0;
  if (gammaCandidates.size()>0) highestGammaEnergy=gammaEnergies.at(0);


  // Initialise variables that might not otherwise get set
  // It does not restart the vector for each entry so we have to do that manually
  ResetVars();

  // Cuts pass/fail
  sensitivity_.passes_two_calorimeters_ = passesTwoCalorimeters;
  sensitivity_.passes_two_plus_calos_ = passesTwoPlusCalos;
  sensitivity_.passes_two_clusters_ = passesTwoClusters;
  sensitivity_.passes_two_tracks_ = passesTwoTracks;
  sensitivity_.passes_associated_calorimeters_ = passesAssociatedCalorimeters;
  sensitivity_.number_of_electrons_=electronCandidates.size();
  sensitivity_.electron_energies_=electronEnergies;
  sensitivity_.gamma_energies_=gammaEnergies;
  sensitivity_.electron_charges_=electronCharges;


  // Reconstructed energies
  sensitivity_.lower_electron_energy_=lowerElectronEnergy;
  sensitivity_.higher_electron_energy_=higherElectronEnergy;
  sensitivity_.total_calorimeter_energy_ = totalCalorimeterEnergy;

  // "First" track is the higher energy one
  //uint highEnergyIndex =(calorimeterEnergy[0]>calorimeterEnergy[1] ? 0:1);
  //uint lowEnergyIndex = 1-highEnergyIndex;


  // And the new vertex vectors - we can rely on these all being the same size of vectors as we populate them all together



  for (size_t i=0;i<electronVertices.size();i++)
    {
      sensitivity_.electron_vertex_x_.push_back(electronVertices.at(i).X());
      sensitivity_.electron_vertex_y_.push_back(electronVertices.at(i).Y());
      sensitivity_.electron_vertex_z_.push_back(electronVertices.at(i).Z());
      sensitivity_.electron_proj_vertex_x_.push_back(electronProjVertices.at(i).X());
      sensitivity_.electron_proj_vertex_y_.push_back(electronProjVertices.at(i).Y());
      sensitivity_.electron_proj_vertex_z_.push_back(electronProjVertices.at(i).Z());
      sensitivity_.electron_dir_x_.push_back(electronDirections.at(i).X());
      sensitivity_.electron_dir_y_.push_back(electronDirections.at(i).Y());
      sensitivity_.electron_dir_z_.push_back(electronDirections.at(i).Z());
    }

  for (size_t i=0;i<alphaVertices.size();i++)
    {
      sensitivity_.alpha_vertex_x_.push_back(alphaVertices.at(i).X());
      sensitivity_.alpha_vertex_y_.push_back(alphaVertices.at(i).Y());
      sensitivity_.alpha_vertex_z_.push_back(alphaVertices.at(i).Z());
      sensitivity_.alpha_proj_vertex_x_.push_back(alphaProjVertices.at(i).X());
      sensitivity_.alpha_proj_vertex_y_.push_back(alphaProjVertices.at(i).Y());
      sensitivity_.alpha_proj_vertex_z_.push_back(alphaProjVertices.at(i).Z());
      sensitivity_.alpha_dir_x_.push_back(alphaDirections.at(i).X());
      sensitivity_.alpha_dir_y_.push_back(alphaDirections.at(i).Y());
      sensitivity_.alpha_dir_z_.push_back(alphaDirections.at(i).Z());
    }

  // Special vertex variables
  if (is2electron || is1e1alpha)
    // At the moment we only set these for these two topologies. This should possibly change
    {
      //First  vertex is the high-energy electron for 2e or the only electron for 1e1alpha
      sensitivity_.first_proj_vertex_y_ = electronProjVertices.at(0).Y();
      sensitivity_.first_proj_vertex_z_ = electronProjVertices.at(0).Z();
      sensitivity_.first_vertex_x_ = electronVertices.at(0).X();
      sensitivity_.first_vertex_y_ = electronVertices.at(0).Y();
      sensitivity_.first_vertex_z_ = electronVertices.at(0).Z();
      sensitivity_.first_track_direction_x_= electronDirections.at(0).X();
      sensitivity_.first_track_direction_y_= electronDirections.at(0).Y();
      sensitivity_.first_track_direction_z_= electronDirections.at(0).Z();
      projectionDistanceXY=(electronVertices.at(0)-electronProjVertices.at(0)).Perp();
    }
  if (is2electron ) // The second one is the lower-energy electron
    {
      sensitivity_.second_proj_vertex_y_ = electronProjVertices.at(1).Y();
      sensitivity_.second_proj_vertex_z_ = electronProjVertices.at(1).Z();
      sensitivity_.second_vertex_x_ = electronVertices.at(1).X();
      sensitivity_.second_vertex_y_ = electronVertices.at(1).Y();
      sensitivity_.second_vertex_z_ = electronVertices.at(1).Z();
      sensitivity_.second_track_direction_x_= electronDirections.at(1).X();
      sensitivity_.second_track_direction_y_= electronDirections.at(1).Y();
      sensitivity_.second_track_direction_z_= electronDirections.at(1).Z();

      sensitivity_.vertex_separation_= (electronVertices.at(0) - electronVertices.at(1)).Mag();
      sensitivity_.foil_projection_separation_= (electronProjVertices.at(0) - electronProjVertices.at(1)).Mag();
      sensitivity_.angle_between_tracks_= electronDirections.at(0).Angle(electronDirections.at(1));

      double thisProjectionDistance=(electronVertices.at(1)-electronProjVertices.at(1)).Perp();
      if (thisProjectionDistance > projectionDistanceXY)projectionDistanceXY=thisProjectionDistance;
    }

  if(is1engamma && (gammaDirection.Mag()>0))
    {
      sensitivity_.angle_between_tracks_= electronDirections.at(0).Angle(gammaDirection);
    }

  if(is1e1alpha)
    {
      sensitivity_.alpha_track_length_=alphaCandidateDetails.at(0).GetTrackLength();
      sensitivity_.proj_track_length_alpha_=alphaCandidateDetails.at(0).GetProjectedTrackLength();
      sensitivity_.alpha_crosses_foil_=alphaCandidateDetails.at(0).TrackCrossesFoil();

      // Second vertex is the alpha
      sensitivity_.second_proj_vertex_y_=alphaCandidateDetails.at(0).GetProjectedVertex().Y();
      sensitivity_.second_proj_vertex_z_=alphaCandidateDetails.at(0).GetProjectedVertex().Z();
      sensitivity_.second_vertex_x_= alphaVertices.at(0).X();
      sensitivity_.second_vertex_y_= alphaVertices.at(0).Y();
      sensitivity_.second_vertex_z_= alphaVertices.at(0).Z();
      sensitivity_.second_track_direction_x_= alphaDirections.at(0).X();
      sensitivity_.second_track_direction_y_= alphaDirections.at(0).Y();
      sensitivity_.second_track_direction_z_= alphaDirections.at(0).Z();

      // Some two-particle topology calculations
      sensitivity_.vertex_separation_=(electronVertices.at(0) - alphaVertices.at(0)).Mag();
      sensitivity_.foil_projection_separation_= (electronProjVertices.at(0) - alphaCandidateDetails.at(0).GetProjectedVertex()).Mag();
      sensitivity_.angle_between_tracks_= electronDirections.at(0).Angle(alphaDirections.at(0));

      double thisProjectionDistance=(alphaVertices.at(0)-alphaProjVertices.at(0)).Perp();
      if (thisProjectionDistance > projectionDistanceXY)projectionDistanceXY=thisProjectionDistance;

    }

  // Track direction
  if (is2electron || is1e1alpha) // This works in either case
    {
      sensitivity_.same_side_of_foil_= ((sensitivity_.first_track_direction_x_ * sensitivity_.second_track_direction_x_) > 0); // X components both positive or both negative
    }
  // Vertices

  sensitivity_.foil_vertex_count_=verticesOnFoil;
  sensitivity_.vertices_in_tracker_=verticesInTracker;
  sensitivity_.projection_distance_xy_=projectionDistanceXY;
  sensitivity_.foil_alpha_count_=foilAlphaCount;
  sensitivity_.alphas_from_foil_=alphasFromFoil;
  sensitivity_.delayed_cluster_hit_count_=alphaHitCounts;
  sensitivity_.electrons_from_foil_=electronsFromFoil;
  sensitivity_.electron_track_lengths_=electronTrackLengths;
  sensitivity_.electron_hit_counts_=electronHitCounts;


  // Timing
  sensitivity_.calo_hit_time_separation_=TMath::Abs(timeDelay);
  sensitivity_.delayed_track_time_= trajClDelayedTime;
  sensitivity_.internal_probability_=internalProbability;
  sensitivity_.external_probability_=externalProbability;
  sensitivity_.foil_projected_internal_probability_=foilProjectedInternalProbability;
  sensitivity_.foil_projected_external_probability_=foilProjectedExternalProbability;

  // Topology


  sensitivity_.topology_1engamma_=is1engamma;
  sensitivity_.topology_1e1gamma_=is1e1gamma;
  sensitivity_.topology_1e1alpha_=is1e1alpha;
  sensitivity_.topology_2e_=is2electron;
  sensitivity_.topology_1e_=is1electron;


  // Calorimeter walls: fractions of energy in each and vector of booleans
  // to say whether there are any hits in that wall
  sensitivity_.gamma_fractions_mainwall_ =  gammaMainwallFraction;
  sensitivity_.gamma_fractions_xwall_ = gammaXwallFraction;
  sensitivity_.gamma_fractions_gveto_ = gammaVetoFraction;
  PopulateWallVectors(electronCaloType, sensitivity_.electron_hits_mainwall_, sensitivity_.electron_hits_xwall_,  sensitivity_.electron_hits_gveto_ );
  PopulateWallVectors(gammaCaloType, sensitivity_.gamma_hits_mainwall_, sensitivity_.gamma_hits_xwall_,  sensitivity_.gamma_hits_gveto_ );

  // Debug information
  sensitivity_.calorimeter_hit_count_=caloHitCount;
  sensitivity_.small_cluster_count_=smallClusterCount;
  sensitivity_.cluster_count_=clusterCount;
  sensitivity_.highest_gamma_energy_=  highestGammaEnergy;
  sensitivity_.edgemost_vertex_=edgemostVertex;
  sensitivity_.number_of_gammas_=gammaCandidates.size();
  sensitivity_.track_count_=trackCount;
  sensitivity_.associated_track_count_=electronCandidates.size();
  sensitivity_.alpha_count_=alphaCandidates.size();
  sensitivity_.delayed_hit_count_=delayedHitCount;

  // Truth info, simulation only
  sensitivity_.true_highest_primary_energy_=higherTrueEnergy;
  sensitivity_.true_second_primary_energy_=lowerTrueEnergy;
  sensitivity_.true_total_energy_= totalTrueEnergy;
  sensitivity_.true_higher_particle_type_=higherTrueType;
  sensitivity_.true_lower_particle_type_=lowerTrueType;
  sensitivity_.true_vertex_x_=trueVertexX;
  sensitivity_.true_vertex_y_=trueVertexY;
  sensitivity_.true_vertex_z_=trueVertexZ;

  tree_->Fill();

  // MUST return a status, see ref dpp::processing_status_flags_type
  return dpp::base_module::PROCESS_OK;
}

void SensitivityModule::CalculateProbabilities(double & internalProbability,
                                               double & externalProbability,
                                               std::vector<TrackDetails*> twoParticles,
                                               bool projected)
{
  double trackLengths[2];
  double theoreticalTimeOfFlight[2];
  double internalEmissionTime[2];
  double internalChiSquared;
  double externalChiSquared;
  for (int count=0;count<2;count++) // 2 particles
    {
      if (projected)
        {
          trackLengths[count]=twoParticles.at(count)->GetProjectedTrackLength();
        }
      else
        {
          trackLengths[count]=twoParticles.at(count)->GetTrackLength();
        }
      theoreticalTimeOfFlight[count] = trackLengths[count]/ (twoParticles.at(count)->GetBeta() * speedOfLight);
      internalEmissionTime[count] = twoParticles.at(count)->GetTime() - theoreticalTimeOfFlight[count];
    }
  // Calculate internal probability: both particles emitted at the same time
  // so time between the calo hits should be Time of flight 1 - Time of flight 2

  // Calculate external probability: one particle travels to foil then the other travels from foil
  // so time between the calo hits should be Time of flight  1 + Time of flight  2
  if (projected)
    {
      internalChiSquared = pow((internalEmissionTime[0] - internalEmissionTime[1]) ,2) / (twoParticles.at(0)->GetProjectedTimeVariance() + twoParticles.at(1)->GetProjectedTimeVariance()) ;

      externalChiSquared=pow(( TMath::Abs(twoParticles.at(0)->GetTime()-twoParticles.at(1)->GetTime()) - (theoreticalTimeOfFlight[0]+theoreticalTimeOfFlight[1]) ),2)/ (twoParticles.at(0)->GetProjectedTimeVariance() + twoParticles.at(1)->GetProjectedTimeVariance()) ;
    }else
    {

      internalChiSquared = pow((internalEmissionTime[0] - internalEmissionTime[1]) ,2) / (twoParticles.at(0)->GetTotalTimeVariance() + twoParticles.at(1)->GetTotalTimeVariance()) ;
      externalChiSquared=pow(( TMath::Abs(twoParticles.at(0)->GetTime()-twoParticles.at(1)->GetTime()) - (theoreticalTimeOfFlight[0]+theoreticalTimeOfFlight[1]) ),2)/(twoParticles.at(0)->GetTotalTimeVariance() + twoParticles.at(1)->GetTotalTimeVariance()) ;
    }
  // Integrate a chisquare distribution to the value if the internal/external chi square
  // to get the probability of it being an internal or external event
  internalProbability=this->ProbabilityFromChiSquared(internalChiSquared);
  externalProbability=this->ProbabilityFromChiSquared(externalChiSquared);
  return;
}

// Calculate probabilities for an internal (both particles from the foil) and external (calo 1 -> foil -> calo 2) topology
void SensitivityModule::CalculateProbabilities(double &internalProbability,
                                               double &externalProbability,
                                               double* /*calorimeterEnergies*/,
                                               double *betas,
                                               double *trackLengths,
                                               double *calorimeterTimes,
                                               double *totalTimeVariances )
{
  double theoreticalTimeOfFlight[2];
  double internalEmissionTime[2];
  double internalChiSquared;
  double externalChiSquared;
  for (int count=0;count<2;count++)
    {
      //energies are in MeV
      theoreticalTimeOfFlight[count] = trackLengths[count]/ (betas[count] * speedOfLight);
      internalEmissionTime[count] = calorimeterTimes[count] - theoreticalTimeOfFlight[count];

    } // for each particle

  // Calculate internal probability: both particles emitted at the same time
  // so time between the calo hits should be Time of flight 1 - Time of flight 2

  internalChiSquared = pow((internalEmissionTime[0] - internalEmissionTime[1]) ,2) / (totalTimeVariances[0] + totalTimeVariances[1]) ;
  //double integralForProbability=0;
  internalProbability=this->ProbabilityFromChiSquared(internalChiSquared);

  // Calculate external probability: one particle travels to foil then the other travels from foil
  // so time between the calo hits should be Time of flight  1 + Time of flight  2
  externalChiSquared=pow(( TMath::Abs(calorimeterTimes[0]-calorimeterTimes[1]) - (theoreticalTimeOfFlight[0]+theoreticalTimeOfFlight[1]) ),2)/(totalTimeVariances[0] + totalTimeVariances[1]) ;
  externalProbability=this->ProbabilityFromChiSquared(externalChiSquared);
  return;
}


/* Functions for sorting vectors of calorimeter wall positions */

// Insert a value into a vector (of the same type), at a particular position (0 is first etc)
// If you pick a position past the end of the vector, stick it on the end
// Position of -1 also sticks it at the end
template <typename T>
void SensitivityModule::InsertAt(T toInsert, std::vector<T> & vec, int position)
{
  if (position>static_cast<int>(vec.size()) || position==-1 )
    {
      vec.push_back(toInsert);
      return;
    }
  else
    {
      typename std::vector<T>::iterator it=vec.begin();
      vec.insert(std::next(it,position),toInsert);
    }
  return;
}

// Fill in the 3 vectors of wall booleans based on the calo wall that got the first hit
void SensitivityModule::PopulateWallVectors(std::vector<int> & calotypes,
                                            std::vector<bool> & mainVec,
                                            std::vector<bool> & xVec,
                                            std::vector<bool> & vetoVec)
{
  mainVec.clear();
  xVec.clear();
  vetoVec.clear();
  for (size_t i=0;i<calotypes.size();i++)
    {
      mainVec.push_back(calotypes.at(i)==mainWallHitType);
      xVec.push_back(calotypes.at(i)==xWallHitType);
      vetoVec.push_back(calotypes.at(i)==gammaVetoHitType);
    }
  return;
}


// Find the position to insert a value into a sorted vector
int SensitivityModule::InsertAndGetPosition(double toInsert,
                                            std::vector<double> &vec,
                                            bool highestFirst)
{
  std::vector<double>::iterator it;
  int len=vec.size();

  it=vec.begin();
  for (int i=0;i<len;i++)
    {
      if ((highestFirst && (toInsert > vec.at(i))) || (!highestFirst && (toInsert < vec.at(i))))
        {
          vec.insert(std::next(it,i),toInsert);
          return i;
        }
    }
  vec.push_back(toInsert);
  return -1; // It needs adding at the end
}

// Convert a chi-squared value to a probability by integrating the chi square distribution up to that limit
double SensitivityModule::ProbabilityFromChiSquared(double chiSquared)
{
  // To get probability from a chi squared value, integrate distribution to our chisq limit
  // We have one degree of freedom
  // *** suspect memory leak on heap ***
  //   TF1 *function_to_integrate = new TF1("Chi-square function", "pow(x,-0.5) * exp(-0.5 * x)", 0, chiSquared);
  //   double * params = 0;
  //   if (chiSquared>3000) chiSquared=3000; // The integral appears to not work properly at values bigger than this, eventually tending to 0 rather than root2pi and thus giving a misleading probability of 1 when it should be almost 0.
  //   double integral=function_to_integrate->Integral(0,chiSquared,1e-6);
  //   double result = (1. - 1./TMath::Sqrt(2.*TMath::Pi()) * integral);
  // Fix rounding errors where result can be a tiny negative number
  //   if (result < 0 ) return 0 ;
  double result = TMath::Prob(chiSquared, 1);
  return result;
}


void SensitivityModule::ResetVars()
{
  sensitivity_.electron_track_lengths_.clear();
  sensitivity_.electron_vertex_x_.clear();
  sensitivity_.electron_vertex_y_.clear();
  sensitivity_.electron_vertex_z_.clear();
  sensitivity_.electron_proj_vertex_x_.clear();
  sensitivity_.electron_proj_vertex_y_.clear();
  sensitivity_.electron_proj_vertex_z_.clear();
  sensitivity_.electron_dir_x_.clear();
  sensitivity_.electron_dir_y_.clear();
  sensitivity_.electron_dir_z_.clear();
  sensitivity_.alpha_vertex_x_.clear();
  sensitivity_.alpha_vertex_y_.clear();
  sensitivity_.alpha_vertex_z_.clear();
  sensitivity_.alpha_proj_vertex_x_.clear();
  sensitivity_.alpha_proj_vertex_y_.clear();
  sensitivity_.alpha_proj_vertex_z_.clear();
  sensitivity_.alpha_dir_x_.clear();
  sensitivity_.alpha_dir_y_.clear();
  sensitivity_.alpha_dir_z_.clear();
  sensitivity_.delayed_track_time_.clear();
  sensitivity_.alphas_from_foil_.clear();
  sensitivity_.delayed_cluster_hit_count_.clear();

  // And initialize the rest, what a drag
  sensitivity_.first_proj_vertex_y_ = -9999;
  sensitivity_.first_proj_vertex_z_ = -9999;
  sensitivity_.first_vertex_x_ = -9999;
  sensitivity_.first_vertex_y_ = -9999;
  sensitivity_.first_vertex_z_ = -9999;
  sensitivity_.first_track_direction_x_= -9999;
  sensitivity_.first_track_direction_y_= -9999;
  sensitivity_.first_track_direction_z_= -9999;
  sensitivity_.second_proj_vertex_y_ = -9999;
  sensitivity_.second_proj_vertex_z_ = -9999;
  sensitivity_.second_vertex_x_ = -9999;
  sensitivity_.second_vertex_y_ = -9999;
  sensitivity_.second_vertex_z_ = -9999;
  sensitivity_.second_track_direction_x_= -9999;
  sensitivity_.second_track_direction_y_= -9999;
  sensitivity_.second_track_direction_z_= -9999;
  sensitivity_.vertex_separation_= -9999;
  sensitivity_.foil_projection_separation_= -9999;
  sensitivity_.angle_between_tracks_= -9999;
  sensitivity_.alpha_track_length_=-9999;
  sensitivity_.proj_track_length_alpha_=-9999;
  sensitivity_.alpha_crosses_foil_=false;
  sensitivity_.foil_alpha_count_=0;
  sensitivity_.alpha_count_=0;
  return;
}

//! [SensitivityModule::reset]
void SensitivityModule::reset()
{
  hfile_->cd();
  tree_->Write();
  hfile_->Close(); //
  std::cout << "In reset: finished conversion, file closed " << std::endl;

  // clean up
  delete hfile_;
  filename_output_ = "sensitivity.root";
  this->_set_initialized(false);

  return;
}
