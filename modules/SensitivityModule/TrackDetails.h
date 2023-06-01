#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"

// - Bayeux
#include "bayeux/dpp/base_module.h"
#include "bayeux/mctools/simulated_data.h"
#include "bayeux/genbb_help/primary_particle.h"
#include "bayeux/genbb_help/primary_event.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/line_3d.h"
#include "bayeux/geomtools/helix_3d.h"
#include "bayeux/geomtools/geomtools.h"

// - Falaise
#include "falaise/snemo/datamodels/calibrated_data.h"
#include "falaise/snemo/datamodels/tracker_clustering_data.h"
#include "falaise/snemo/datamodels/tracker_clustering_solution.h"
#include "falaise/snemo/datamodels/particle_track_data.h"


const double ELECTRON_MASS=0.5109989461; // From pdg, in MeV
const double LIGHT_SPEED=299792458 * 1e-9 * 1000; // Millimeters per nanosecond
const double FOIL_CELL_GAP=30.838; // From the foil to the first cell in mm

class TrackDetails{
  TVector3 foilmostVertex_ ;
  TVector3 direction_;
  bool vertexOnFoil_=false;
  bool vertexInTracker_=false;
  TVector3 projectedVertex_;
  enum Particle { ELECTRON, GAMMA, ALPHA, UNKNOWN };
  Particle particleType_=UNKNOWN;
  double mainwallFraction_=0;
  double xwallFraction_=0;
  double vetoFraction_=0;
  int firstHitType_=-1;
  double energy_=0;
  double energySigma_=0;
  double time_=0;
  double timeSigma_=0;
  int charge_=1;
  bool makesTrack_=false;
  snemo::datamodel::particle_track track_;
  double delayTime_=0 ;
  int trackerHitCount_= 0;
  double trackLength_= 0;
  double trackLengthSigma_=0;
  double projectedLength_=0;
  bool crossesFoil_=false;
  const geomtools::manager* geometry_manager_;

  
  bool hasTrack_=false;
  bool SetFoilmostVertex();
  bool SetDirection();
  bool SetProjectedVertex();
  bool PopulateCaloHits();
  double GetTotalTimeVariance(double thisTrackLength);
  
public:

  // SuperNEMO constants
  int MAINWALL=1302;
  int XWALL=1232;
  int GVETO=1252;
  
  
  double MAXY=2505.494; // This is the calo position but maybe it should be the end of the actual foils?
  double MAXZ=1400; // This is not exact! Get the real value!
  
  TrackDetails();
  TrackDetails(const geomtools::manager * geometry_manager_,
               snemo::datamodel::particle_track track);
  void Initialize(const geomtools::manager * geometry_manager_,
                  snemo::datamodel::particle_track track);
  bool Initialize();

  bool IsGamma();
  bool IsAlpha();
  bool IsElectron();
  bool IsNegativeElectron();
  bool IsPositron();
  bool MakesTrack();
  
  // Charge
  int GetCharge();
  
  // Where did it hit?
  bool HitMainwall();
  bool HitXwall();
  bool HitGammaVeto();
  int GetFirstHitType();

  // Energies
  double GetEnergy();
  double GetEnergySigma();
  double GetTime();
  double GetTimeSigma();
  double GetMainwallFraction();
  double GetXwallFraction();
  double GetVetoFraction();
  double GetBeta(); // Fraction of light speed
  double GetProjectedTimeVariance();
  double GetTotalTimeVariance();
  
  // Foilmost vertex
  double GetFoilmostVertexX();
  double GetFoilmostVertexY();
  double GetFoilmostVertexZ();
  TVector3 GetFoilmostVertex();
  bool HasFoilVertex();
  bool HasTrackerVertex();
  bool TrackCrossesFoil();
  
  // Direction at foilmost end
  double GetDirectionX();
  double GetDirectionY();
  double GetDirectionZ();
  TVector3 GetDirection();
  
  // Foil-projected vertex
  double GetProjectedVertexX();
  double GetProjectedVertexY();
  double GetProjectedVertexZ();
  TVector3 GetProjectedVertex();
  
  // For charged particle tracks
  double GetTrackLength();
  double GetTrackLengthSigma();
  double GetProjectedTrackLength();
  int GetTrackerHitCount();
  double GetDelayTime();
  
  // For gammas, we need an electron track to calculate an assumed length
  double GenerateGammaTrackLengths(TrackDetails *electronTrack);
  TVector3 GenerateGammaTrackDirection(TrackDetails *electronTrack);
  // And the same for alpha projections in a 1e1alpha topology
  bool GenerateAlphaProjections(TrackDetails *electronTrack);
};

