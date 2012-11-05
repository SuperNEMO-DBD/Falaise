/* ntupler.h
 * Author (s) :  Federico Nova <nova@physics.utexas.edu>
 * Creation date: 2011-03-31
 * Last modified: 2011-03-31
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */


#ifndef __snlocal_tracking__ntupler_h
#define __snlocal_tracking__ntupler_h

#include <sstream>
#include <ostream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <mybhep/utilities.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/tracked_data.h>

#include <list>
#include <vector>
#include "TFile.h"
#include "TSystem.h"
#include "TMacro.h"
#include "Riostream.h"      
#include "TH1.h"      
#include "TNtuple.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TCanvas.h"


class ntupler
{
  
 public: 
  
  // ctor:
  ntupler ();
  
  // dtor:
  virtual ~ntupler ();
  
 private: 
  
  typedef struct SimulatedVsTrackedHitStorage{
    std::vector<int>* id_;
    std::vector<double>* simulated_x_;
    std::vector<double>* simulated_y_;
    std::vector<double>* simulated_z_;
    std::vector<double>* simulated_dx_;
    std::vector<double>* simulated_dz_;
    std::vector<double>* simulated_dr_;
    std::vector<double>* simulated_dphi_;
    std::vector<double>* tracked_tangency_x_;
    std::vector<double>* tracked_tangency_y_;
    std::vector<double>* tracked_tangency_z_;
    std::vector<double>* tracked_tangency_x_error_;
    std::vector<double>* tracked_tangency_y_error_;
    std::vector<double>* tracked_tangency_z_error_;
    std::vector<double>* tracked_tangency_dx_;
    std::vector<double>* tracked_tangency_dz_;
    std::vector<double>* tracked_tangency_dr_;
    std::vector<double>* tracked_tangency_dphi_;
    std::vector<double>* residual_tangency_x_;
    std::vector<double>* residual_tangency_y_;
    std::vector<double>* residual_tangency_z_;
    std::vector<double>* pull_tangency_x_;
    std::vector<double>* pull_tangency_y_;
    std::vector<double>* pull_tangency_z_;
    std::vector<double>* tracked_helix_x_;
    std::vector<double>* tracked_helix_y_;
    std::vector<double>* tracked_helix_z_;
    std::vector<double>* tracked_helix_x_error_;
    std::vector<double>* tracked_helix_y_error_;
    std::vector<double>* tracked_helix_z_error_;
    std::vector<double>* tracked_helix_dx_;
    std::vector<double>* tracked_helix_dz_;
    std::vector<double>* tracked_helix_dr_;
    std::vector<double>* tracked_helix_dphi_;
    std::vector<double>* residual_helix_x_;
    std::vector<double>* residual_helix_y_;
    std::vector<double>* residual_helix_z_;
    std::vector<double>* pull_helix_x_;
    std::vector<double>* pull_helix_y_;
    std::vector<double>* pull_helix_z_;
    std::vector<double>* simulated_length_;
    std::vector<double>* simulated_phi_;
    std::vector<double>* simulated_theta_;
    std::vector<double>* simulated_kink_hor_;
    std::vector<double>* simulated_kink_vert_;
    std::vector<double>* tracked_length_;
    std::vector<double>* tracked_phi_;
    std::vector<double>* tracked_theta_;
    std::vector<double>* tracked_kink_hor_;
    std::vector<double>* tracked_kink_vert_;
  } SimulatedVsTrackedHitStorage;
  
  typedef struct SimulatedVsTrackedTrackStorage{
    std::vector<int>* id_;
    std::vector<double>* simulated_vertex_x_;
    std::vector<double>* simulated_vertex_y_;
    std::vector<double>* simulated_vertex_z_;
    std::vector<double>* simulated_vertex_cos_;
    std::vector<double>* simulated_vertex_phi_;
    std::vector<double>* simulated_charge_;
    std::vector<double>* tracked_helix_vertex_x_;
    std::vector<double>* tracked_helix_vertex_y_;
    std::vector<double>* tracked_helix_vertex_z_;
    std::vector<double>* tracked_helix_vertex_cos_;
    std::vector<double>* tracked_helix_vertex_phi_;
    std::vector<double>* tracked_charge_;
    std::vector<double>* tracked_helix_charge_;
    std::vector<double>* tracked_helix_vertex_x_error_;
    std::vector<double>* tracked_helix_vertex_y_error_;
    std::vector<double>* tracked_helix_vertex_z_error_;
    std::vector<double>* tracked_helix_vertex_phi_error_;
    std::vector<double>* residual_helix_vertex_x_;
    std::vector<double>* residual_helix_vertex_y_;
    std::vector<double>* residual_helix_vertex_z_;
    std::vector<double>* residual_helix_vertex_phi_;
    std::vector<double>* pull_helix_vertex_x_;
    std::vector<double>* pull_helix_vertex_y_;
    std::vector<double>* pull_helix_vertex_z_;
    std::vector<double>* pull_helix_vertex_phi_;
    std::vector<double>* simulated_decay_vertex_x_;
    std::vector<double>* simulated_decay_vertex_y_;
    std::vector<double>* simulated_decay_vertex_z_;
    std::vector<double>* tracked_helix_decay_vertex_x_;
    std::vector<double>* tracked_helix_decay_vertex_y_;
    std::vector<double>* tracked_helix_decay_vertex_z_;
    std::vector<double>* tracked_helix_decay_vertex_phi_;
    std::vector<double>* tracked_helix_decay_vertex_x_error_;
    std::vector<double>* tracked_helix_decay_vertex_y_error_;
    std::vector<double>* tracked_helix_decay_vertex_z_error_;
    std::vector<double>* tracked_helix_decay_vertex_phi_error_;
    std::vector<double>* residual_helix_decay_vertex_x_;
    std::vector<double>* residual_helix_decay_vertex_y_;
    std::vector<double>* residual_helix_decay_vertex_z_;
    std::vector<double>* pull_helix_decay_vertex_x_;
    std::vector<double>* pull_helix_decay_vertex_y_;
    std::vector<double>* pull_helix_decay_vertex_z_;
    std::vector<double>* tracked_tangent_vertex_x_;
    std::vector<double>* tracked_tangent_vertex_y_;
    std::vector<double>* tracked_tangent_vertex_z_;
    std::vector<double>* tracked_tangent_vertex_cos_;
    std::vector<double>* tracked_tangent_vertex_phi_;
    std::vector<double>* tracked_tangent_vertex_x_error_;
    std::vector<double>* tracked_tangent_vertex_y_error_;
    std::vector<double>* tracked_tangent_vertex_z_error_;
    std::vector<double>* tracked_tangent_vertex_phi_error_;
    std::vector<double>* residual_tangent_vertex_x_;
    std::vector<double>* residual_tangent_vertex_y_;
    std::vector<double>* residual_tangent_vertex_z_;
    std::vector<double>* residual_tangent_vertex_phi_;
    std::vector<double>* pull_tangent_vertex_x_;
    std::vector<double>* pull_tangent_vertex_y_;
    std::vector<double>* pull_tangent_vertex_z_;
    std::vector<double>* pull_tangent_vertex_phi_;
    std::vector<double>* tracked_tangent_decay_vertex_x_;
    std::vector<double>* tracked_tangent_decay_vertex_y_;
    std::vector<double>* tracked_tangent_decay_vertex_z_;
    std::vector<double>* tracked_tangent_decay_vertex_phi_;
    std::vector<double>* tracked_tangent_decay_vertex_x_error_;
    std::vector<double>* tracked_tangent_decay_vertex_y_error_;
    std::vector<double>* tracked_tangent_decay_vertex_z_error_;
    std::vector<double>* tracked_tangent_decay_vertex_phi_error_;
    std::vector<double>* residual_tangent_decay_vertex_x_;
    std::vector<double>* residual_tangent_decay_vertex_y_;
    std::vector<double>* residual_tangent_decay_vertex_z_;
    std::vector<double>* pull_tangent_decay_vertex_x_;
    std::vector<double>* pull_tangent_decay_vertex_y_;
    std::vector<double>* pull_tangent_decay_vertex_z_;
    std::vector<double>* tracked_kink_x_;
    std::vector<double>* tracked_kink_y_;
    std::vector<double>* tracked_kink_z_;
    std::vector<double>* tracked_kink_x_error_;
    std::vector<double>* tracked_kink_y_error_;
    std::vector<double>* tracked_kink_z_error_;
    std::vector<double>* residual_kink_x_;
    std::vector<double>* residual_kink_y_;
    std::vector<double>* residual_kink_z_;
    std::vector<double>* pull_kink_x_;
    std::vector<double>* pull_kink_y_;
    std::vector<double>* pull_kink_z_;
  } SimulatedVsTrackedTrackStorage;
			

  TFile* __f;
  TTree* __tree;
  string __outfilename;
  CAT::topology::tracked_data tracked_data_;

 public: 
  
  
  SimulatedVsTrackedHitStorage simulated_vs_tracked_hit_;
  SimulatedVsTrackedTrackStorage simulated_vs_tracked_track_;
  SimulatedVsTrackedTrackStorage simulated_vs_tracked_n3track_;

  void initialize (void);
  void finalize (void) ;
  void set_outfilename (std::string title_);
  std::string  get_outfilename();

  bool is_valid () const ;
  void __fill();
  bool get_true_hit_of_reco_cell(CAT::topology::cell c, CAT::topology::node& n, size_t& index);
  size_t get_cell_index(CAT::topology::cell c);

  void set_tracked_data (const CAT::topology::tracked_data& tracked_data);
  const  CAT::topology::tracked_data & get_tracked_data () const ;
  
  void clear ();
  //   get_next_point(snemo::core::model::lt_track t_, size_t id_);
  
};   // end of class ntupler
 

#endif // __snlocal_tracking__ntupler_h

// end of ntupler.h

