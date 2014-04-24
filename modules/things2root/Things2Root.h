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

// - Falaise
#include "falaise/snemo/datamodels/calibrated_data.h"
#include "falaise/snemo/datamodels/event_header.h"

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
} trackereventstorage;

typedef struct CaloEventStorage{
  int nohits_;
  std::vector<int>* id_;
  std::vector<int>* type_;
  std::vector<int>* module_;
  std::vector<int>* side_;
  std::vector<int>* column_;
  std::vector<int>* row_;
  std::vector<int>* wall_;
  std::vector<double>* time_;
  std::vector<double>* sigmatime_;
  std::vector<double>* energy_;
  std::vector<double>* sigmaenergy_;
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
} TrueParticleStorage;

typedef struct TrueCaloStorage{
  int nohits_;
  std::vector<int>* id_;
  std::vector<int>* type_;
  std::vector<int>* module_;
  std::vector<int>* side_;
  std::vector<int>* column_;
  std::vector<int>* row_;
  std::vector<int>* wall_;
  std::vector<double>* x_;
  std::vector<double>* y_;
  std::vector<double>* z_;
  std::vector<double>* time_;
  std::vector<double>* energy_;
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
} TrueTrackerStorage;

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

  // Forward declaration for PIMPL:
  struct working_space;
  working_space * ws_;

  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(Things2Root);
};
#endif // THINGS2ROOT_H
