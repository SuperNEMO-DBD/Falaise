//! \file Things2Root
//! \brief User processing module for flreconstruct
//! \details Process a things object and convert data to ROOT file output.
#ifndef THINGS2ROOT_HH
#define THINGS2ROOT_HH

// Standard Library
#include <vector>
#include <iostream>
#include <string>

// Third Party
#include "TFile.h"
#include "TTree.h"

// - Bayeux
#include "bayeux/dpp/base_module.h"
#include "bayeux/mctools/simulated_data.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/geomtools/geometry_service.h"

// This Project
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

  TrueTrackerStorage truetracker_; // see typedefs
  TrueVertexStorage truevertex_; // see typedefs
  TrueParticleStorage trueparticle_; // see typedefs

  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(Things2Root);
};
#endif // Things2Root_HH
