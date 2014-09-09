//! \file Things2Root
//! \brief User processing module for flreconstruct
//! \details Process a things object and convert data to ROOT file output.
#ifndef THINGS2ROOT_H
#define THINGS2ROOT_H

// Standard Library
#include <vector>
#include <iostream>
#include <string>

// Third Party

// - Bayeux
#include "bayeux/dpp/base_module.h"
#include "bayeux/mctools/simulated_data.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/datatools/properties.h"

// - Falaise
#include "falaise/snemo/datamodels/calibrated_data.h"
#include "falaise/snemo/datamodels/event_header.h"
#include "falaise/snemo/datamodels/tracker_clustering_data.h"
#include "falaise/snemo/datamodels/tracker_clustering_solution.h"
#include "falaise/snemo/datamodels/tracker_cluster.h"
#include "falaise/snemo/datamodels/tracker_trajectory_data.h"
#include "falaise/snemo/datamodels/tracker_trajectory_solution.h"
#include "falaise/snemo/datamodels/particle_track_data.h"
#include "falaise/snemo/datamodels/particle_track.h"

// Forward:
class TTree;
class TFile;

// This Project
typedef struct HeaderEventStorage{
  int runnumber_;
  int eventnumber_;
  int date_;
  int runtype_;
  bool simulated_;
} headereventstorage;

typedef struct TrackerEventStorage{
  int nohits_;
  std::vector<int>* id_;
  std::vector<int>* module_;
  std::vector<int>* side_;
  std::vector<int>* layer_;
  std::vector<int>* column_;
  std::vector<double>* x_;
  std::vector<double>* y_;
  std::vector<double>* z_;
  std::vector<double>* sigmaz_;
  std::vector<double>* r_;
  std::vector<double>* sigmar_;
  std::vector<int>* truehitid_;
  std::vector<int>* truetrackid_;
  std::vector<int>* trueparenttrackid_;

  TrackerEventStorage() :
      nohits_(0),
      id_(0),
      module_(0),
      side_(0),
      layer_(0),
      column_(0),
      x_(0),
      y_(0),
      z_(0),
      sigmaz_(0),
      r_(0),
      sigmar_(0),
      truehitid_(0),
      truetrackid_(0),
      trueparenttrackid_(0) {}
  } trackereventstorage;

typedef struct CaloEventStorage{
  int nohits_;
  std::vector<int>* id_;
  std::vector<int>* type_;
  std::vector<int>* module_;
  std::vector<int>* side_;
  std::vector<int>* column_;
  std::vector<int>* xcalocolumn_;
  std::vector<int>* gvetocolumn_;
  std::vector<int>* row_;
  std::vector<int>* xcalowall_;
  std::vector<int>* gvetowall_;
  std::vector<double>* time_;
  std::vector<double>* sigmatime_;
  std::vector<double>* energy_;
  std::vector<double>* sigmaenergy_;
  std::vector<int>* truecaloid_;

  CaloEventStorage() :
      nohits_(0),
      id_(0),
      type_(0),
      module_(0),
      side_(0),
      column_(0),
      xcalocolumn_(0),
      gvetocolumn_(0),
      row_(0),
      xcalowall_(0),
      gvetowall_(0),
      time_(0),
      sigmatime_(0),
      energy_(0),
      sigmaenergy_(0),
      truecaloid_(0) {}
} caloeventstorage;

typedef struct TrueVertexStorage{
  double x_;
  double y_;
  double z_;
  double time_;
} TrueVertexStorage;

typedef struct TrueParticleStorage{
  int noparticles_;
  std::vector<int>* id_;
  std::vector<int>* type_;
  std::vector<double>* px_;
  std::vector<double>* py_;
  std::vector<double>* pz_;
  std::vector<double>* time_;
  std::vector<double>* ke_;
  std::vector<double>* mass_;

  TrueParticleStorage() :
      noparticles_(0),
      id_(0),
      type_(0),
      px_(0),
      py_(0),
      pz_(0),
      time_(0),
      ke_(0),
      mass_(0) {}
} TrueParticleStorage;

typedef struct TrueCaloStorage{
  int nohits_;
  std::vector<int>* id_;
  std::vector<int>* type_;
  std::vector<int>* module_;
  std::vector<int>* side_;
  std::vector<int>* column_;
  std::vector<int>* xcalocolumn_;
  std::vector<int>* gvetocolumn_;
  std::vector<int>* row_;
  std::vector<int>* xcalowall_;
  std::vector<int>* gvetowall_;
  std::vector<double>* x_;
  std::vector<double>* y_;
  std::vector<double>* z_;
  std::vector<double>* time_;
  std::vector<double>* energy_;

  TrueCaloStorage() :
      nohits_(0),
      id_(0),
      type_(0),
      module_(0),
      side_(0),
      column_(0),
      xcalocolumn_(0),
      gvetocolumn_(0),
      row_(0),
      xcalowall_(0),
      gvetowall_(0),
      x_(0),
      y_(0),
      z_(0),
      time_(0),
      energy_(0) {}
} truecalostorage;

typedef struct TrueTrackerStorage{
  int nohits_;
  std::vector<int>* id_;
  std::vector<int>* module_;
  std::vector<int>* side_;
  std::vector<int>* layer_;
  std::vector<int>* column_;
  std::vector<double>* time_;
  std::vector<double>* xstart_;
  std::vector<double>* ystart_;
  std::vector<double>* zstart_;
  std::vector<double>* xstop_;
  std::vector<double>* ystop_;
  std::vector<double>* zstop_;
  std::vector<int>* trackid_;
  std::vector<int>* parenttrackid_;

  TrueTrackerStorage() :
      nohits_(0),
      id_(0),
      module_(0),
      side_(0),
      layer_(0),
      column_(0),
      time_(0),
      xstart_(0),
      ystart_(0),
      zstart_(0),
      xstop_(0),
      ystop_(0),
      zstop_(0),
      trackid_(0),
      parenttrackid_(0) {}
} TrueTrackerStorage;

typedef struct ClusterStorage{
  int nosolutions_;
  std::vector<int>* clustersolutionid_;
  std::vector<int>* nounclusteredhits_;
  std::vector<int>* noclusters_;
  //std::vector< std::vector <int> >* clusterid_;
  std::vector<int>* clusterid_;
  std::vector<int>* nohits_;
  std::vector<int>* hitid_;
  std::vector<double>* catcharge_;
  std::vector<double>* catdetailedcharge_;
  std::vector<double>* catchi2s_;
  //std::vector<double>* catchi2sall_;
  std::vector<double>* catprobs_;
  //std::vector<double>* catprobsall_;
  std::vector<double>* catpx_;
  std::vector<double>* catpy_;
  std::vector<double>* catpz_;
  std::vector<double>* cathelixcharge_;
  std::vector<double>* cathelixlength_;
  std::vector<double>* cathelixlengtherror_;
  std::vector<double>* cathelixvertexx_;
  std::vector<double>* cathelixvertexxerror_;
  std::vector<double>* cathelixvertexy_;
  std::vector<double>* cathelixvertexyerror_;
  std::vector<double>* cathelixvertexz_;
  std::vector<double>* cathelixvertexzerror_;
  std::vector<double>* cathelixdecayvertexx_;
  std::vector<double>* cathelixdecayvertexxerror_;
  std::vector<double>* cathelixdecayvertexy_;
  std::vector<double>* cathelixdecayvertexyerror_;
  std::vector<double>* cathelixdecayvertexz_;
  std::vector<double>* cathelixdecayvertexzerror_;
  //std::vector<double>* cathelixchi2s_;
  std::vector<double>* cattangentlength_;
  std::vector<double>* cattangentlengtherror_;
  std::vector<double>* cattangentvertexx_;
  std::vector<double>* cattangentvertexxerror_;
  std::vector<double>* cattangentvertexy_;
  std::vector<double>* cattangentvertexyerror_;
  std::vector<double>* cattangentvertexz_;
  std::vector<double>* cattangentvertexzerror_;
  std::vector<double>* cattangentdecayvertexx_;
  std::vector<double>* cattangentdecayvertexxerror_;
  std::vector<double>* cattangentdecayvertexy_;
  std::vector<double>* cattangentdecayvertexyerror_;
  std::vector<double>* cattangentdecayvertexz_;
  std::vector<double>* cattangentdecayvertexzerror_;
  
ClusterStorage() :
  nosolutions_(0),
    clustersolutionid_(0),
    nounclusteredhits_(0),
    noclusters_(0),
    clusterid_(0),
    nohits_(0),
    catcharge_(0),
    catdetailedcharge_(0),
    catchi2s_(0),
  //catchi2sall_(0),
    catprobs_(0),
  //catprobsall_(0),
    catpx_(0),
    catpy_(0),
    catpz_(0),
    cathelixcharge_(0),
    cathelixlength_(0),
    cathelixlengtherror_(0),
    cathelixvertexx_(0),
    cathelixvertexxerror_(0),
    cathelixvertexy_(0),
    cathelixvertexyerror_(0),
    cathelixvertexz_(0),
    cathelixvertexzerror_(0),
    cathelixdecayvertexx_(0),
    cathelixdecayvertexxerror_(0),
    cathelixdecayvertexy_(0),
    cathelixdecayvertexyerror_(0),
    cathelixdecayvertexz_(0),
    cathelixdecayvertexzerror_(0),
  //cathelixchi2s_(0),
    cattangentlength_(0),
    cattangentlengtherror_(0),
    cattangentvertexx_(0),
    cattangentvertexxerror_(0),
    cattangentvertexy_(0),
    cattangentvertexyerror_(0),
    cattangentvertexz_(0),
    cattangentvertexzerror_(0),
    cattangentdecayvertexx_(0),
    cattangentdecayvertexxerror_(0),
    cattangentdecayvertexy_(0),
    cattangentdecayvertexyerror_(0),
    cattangentdecayvertexz_(0),
    cattangentdecayvertexzerror_(0) {}
} ClusterStorage;

typedef struct TrajectoryStorage{
  int nosolutions_;
  std::vector<int>* trajectorysolutionid_;
  std::vector<int>* notrajectories_;
  std::vector<int>* nounfittedclusters_;
  std::vector<int>* nounfittedhits_;
  std::vector<int>* trajectoryid_;
  std::vector<int>* clusterid_;
  std::vector<double>* chi2_;
  std::vector<int>* ndof_;
  std::vector<double>* length_;

TrajectoryStorage() :
  nosolutions_(0),
    trajectorysolutionid_(0),
    notrajectories_(0),
    nounfittedclusters_(0),
    nounfittedhits_(0),
    trajectoryid_(0),
    clusterid_(0),
    chi2_(0),
    ndof_(0),
    length_(0) {}
} TrajectoryStorage;

typedef struct ParticleStorage{
  int noparticles_;
  std::vector<int>* charge_;
  std::vector<int>* nocalohits_;
  std::vector<double>* energy_;
  std::vector<double>* calotime_;
  std::vector<int>* novertices_;
  std::vector<double>* vertexx_;
  std::vector<double>* vertexy_;
  std::vector<double>* vertexz_;
  std::vector<double>* vertexxerror_;
  std::vector<double>* vertexyerror_;
  std::vector<double>* vertexzerror_;
  std::vector<int>* trajectoryid_;

ParticleStorage() :
  noparticles_(0),
    charge_(0),
    nocalohits_(0),
    energy_(0),
    calotime_(0),
    novertices_(0),
    vertexx_(0),
    vertexy_(0),
    vertexz_(0),
    vertexxerror_(0),
    vertexyerror_(0),
    vertexzerror_(0),
    trajectoryid_(0) {}
}ParticleStorage;


class Things2Root : public dpp::base_module {
 public:
  //! Construct module
  Things2Root();

  //! Destructor
  virtual ~Things2Root();

  //! Configure the module
  virtual void initialize(const datatools::properties& myConfig,
                          datatools::service_manager& flServices,
                          dpp::module_handle_dict_type& what);

  //! Process supplied data record
  virtual dpp::base_module::process_status process(datatools::things& workItem);

  //! Reset the module
  virtual void reset();

 private:
  // configurable data member
  std::string filename_output_;

  // geometry service
  const geomtools::manager* geometry_manager_; //!< The geometry manager

  // Variables
  TFile* hfile_;
  TTree* tree_;

  TrueCaloStorage truecalo_; // see typedefs
  TrueTrackerStorage truetracker_; // see typedefs
  TrueVertexStorage truevertex_; // see typedefs
  TrueParticleStorage trueparticle_; // see typedefs

  HeaderEventStorage header_;
  TrackerEventStorage tracker_;
  CaloEventStorage calo_;
  ClusterStorage cluster_;
  TrajectoryStorage trajectory_;
  ParticleStorage particle_;

  // Forward declaration for PIMPL:
  struct working_space;
  working_space * ws_;

  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(Things2Root);
};
#endif // THINGS2ROOT_H
