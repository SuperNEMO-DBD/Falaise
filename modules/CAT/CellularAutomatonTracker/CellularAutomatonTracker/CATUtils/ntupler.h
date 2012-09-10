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
  
  static const Int_t MAXNHITS = 1000;
  static const Int_t MAXNTRACKS = 50;
  static const Int_t MAXNVERTEX = 5;
  
  bool __has_mc_hits;
  
  //      PARAMETER (MAXMEC=30) 
  //      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC) 
  //     + ,NAMEC(MAXMEC),NSTEP ,PID,DESTEP,DESTEL,SAFETY,SLENG 
  //     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT
  typedef struct { 
    Int_t Ngg;
    Float_t Gg[MAXNHITS][9];
    
    Int_t Nggt;
    Float_t Ggtx[MAXNHITS];
    Float_t Ggty[MAXNHITS];
    Float_t Ggtz[MAXNHITS];
    Float_t Itt1[MAXNHITS];
    Float_t Itt2[MAXNHITS];
    
    Int_t Nsc;

    Int_t Nbr_tks;
    Int_t True_Nbr_tks;
    Int_t          Nbr_pts[MAXNTRACKS];
    Float_t Ptx[MAXNTRACKS][MAXNHITS];  // tangency points
    Float_t Pty[MAXNTRACKS][MAXNHITS];
    Float_t Ptz[MAXNTRACKS][MAXNHITS];
    Float_t HelixPtx[MAXNTRACKS][MAXNHITS];  // helix points
    Float_t HelixPty[MAXNTRACKS][MAXNHITS];
    Float_t HelixPtz[MAXNTRACKS][MAXNHITS];
    Int_t Ptid[MAXNTRACKS][MAXNHITS];
    
    
    Int_t     Ind_points[MAXNTRACKS][MAXNHITS];

    Float_t    Tk_le[MAXNTRACKS];
    Float_t    Q[MAXNTRACKS];
    Float_t    mom[MAXNTRACKS];

    Int_t          has_CAT_helix_vertex[MAXNTRACKS];
    Float_t         CAT_helix_vtx_cos_dir[MAXNTRACKS][MAXNVERTEX][3];
    Float_t         CAT_helix_decay_vtx_cos_dir[MAXNTRACKS][MAXNVERTEX][3];
    Float_t         CAT_helix_vtx_x[MAXNTRACKS][MAXNVERTEX];
    Float_t         CAT_helix_vtx_y[MAXNTRACKS][MAXNVERTEX];
    Float_t         CAT_helix_vtx_z[MAXNTRACKS][MAXNVERTEX];
    Int_t          has_CAT_tangency_vertex[MAXNTRACKS];
    Float_t         CAT_tangency_vtx_cos_dir[MAXNTRACKS][MAXNVERTEX][3];
    Float_t         CAT_tangency_decay_vtx_cos_dir[MAXNTRACKS][MAXNVERTEX][3];
    Float_t         CAT_tangency_vtx_x[MAXNTRACKS][MAXNVERTEX];
    Float_t         CAT_tangency_vtx_y[MAXNTRACKS][MAXNVERTEX];
    Float_t         CAT_tangency_vtx_z[MAXNTRACKS][MAXNVERTEX];
    Float_t         NEMOR_helix_vtx_x[MAXNTRACKS];
    Float_t         NEMOR_helix_vtx_y[MAXNTRACKS];
    Float_t         NEMOR_helix_vtx_z[MAXNTRACKS];
    
    Float_t    True_Tk_le[MAXNTRACKS];
    Int_t          True_Nbr_vtx[MAXNTRACKS];
    Float_t         True_Vtx_cos_dir[MAXNTRACKS][MAXNVERTEX][3];
    Float_t         True_Decay_Vtx_cos_dir[MAXNTRACKS][MAXNVERTEX][3];
    Float_t         True_Vtx_x[MAXNTRACKS][MAXNVERTEX];
    Float_t         True_Vtx_y[MAXNTRACKS][MAXNVERTEX];
    Float_t         True_Vtx_z[MAXNTRACKS][MAXNVERTEX];
    
    
  } event; 
  
  
  std::string __title;
  TFile *__f;
  TTree *__tree;
  event __event;
  TH1F *h1;
  TH1F *helix_chi2;
  TNtuple *__event_ntuple, *__hits_ntuple, *__vertexes_ntuple,  *__tracks_ntuple;

  CAT::topology::tracked_data tracked_data_;


 public: 
  
  void set_title (std::string title_);
  std::string  get_title();
  
  void initialize (void);
  void finalize (void) ;
  
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

