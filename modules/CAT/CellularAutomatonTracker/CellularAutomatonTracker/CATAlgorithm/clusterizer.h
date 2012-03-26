/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm___pattern_rec2___
#define __CATAlgorithm___pattern_rec2___

#include <CATAlgorithm/CAT_config.h>

#include <stdexcept>
#include <mybhep/gstore.h>
#include <mybhep/messenger.h>
#include <mybhep/event.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <mybhep/system_of_units.h>

// #ifdef CAT_WITH_DEVEL_ROOT 
// #include "TApplication.h"
// #include <TROOT.h>
// #include <TChain.h>
// #include "TH2.h"
// #include "TH1.h"
// #include "TGraph.h"
// #include "TStyle.h"
// #include "TCanvas.h"
// #include "TFile.h" 
// #include "TMath.h" 
// #include "TBox.h"
// #include "TMarker.h"
// #endif

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <mybhep/EventManager2.h>
//#include <CATUtils/NHistoManager2.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/Clock.h>
#include <CATAlgorithm/lt_utils.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/tracked_data.h>


namespace CAT{

  using namespace mybhep;
  using namespace std;
  
  typedef struct{
    float x;
    float z;
  } POINT;
  

  class clusterizer{

  public:
  
    clusterizer(void);

    clusterizer(mybhep::gstore);
  
    virtual ~clusterizer();

  protected:
    
    // 2012-03-24 FM : add core initialization method
    bool _initialize( void );

  public:
    bool initialize( void );
    bool initialize( mybhep::sstore store, mybhep::gstore gs, mybhep::EventManager2 *eman=0);
    void initializeHistos( void );
    bool execute(mybhep::event& evt, int ievent);
    bool finalize();
    void finalizeHistos( void );
    void FillYPositions( mybhep::event& evt );
    void FillYPositions( mybhep::particle* p );
    void FillYPosition( mybhep::hit* h );
    void FillTrueVertexes( mybhep::event& evt );
    void FillTrueVertexes( mybhep::particle* p );
    void FillHistos(mybhep::event& evt );
    bool read_event(mybhep::event& evt, topology::tracked_data & __tracked_data);
    bool prepare_event(topology::tracked_data & __tracked_data);
    void read_true_sequences(mybhep::event& evt);
    void read_nemo_sequences(mybhep::event& evt);
    void read_true_sequences();
    void read_nemo_sequences();
    void print_cells(void)const;
    void print_calos(void)const;
    void clusterize(topology::tracked_data & __tracked_data);
    void print_clusters(void) const;
    void print_true_sequences(void)const;
    void print_nemo_sequences(void)const;
    void readDstProper(mybhep::sstore global, mybhep::EventManager2 *eman);
    void readDstProper();
    void GenerateWires( void );
    double long_resolution(double Z, double d[3])const;
    double long_resolution_1cthd(double Zdist)const;
    double GetYError( double y, float tup, float tdown, double direction[3]);
    void order_cells();

    //! get cells
    const std::vector<topology::cell>& get_cells()const
    {
      return cells_;
    }

    //! set cells
    void set_cells(std::vector<topology::cell> cells)
    {
      cells_.clear();
      cells_ = cells;
    }

    //! get clusters
    const std::vector<topology::cluster>& get_clusters()const
    {
      return clusters_;
    }

    //! set clusters
    void set_clusters(std::vector<topology::cluster> clusters)
    {
      clusters_.clear();
      clusters_ = clusters;
    }

    //! get calorimeter_hits                                                                                                      
    const std::vector<topology::calorimeter_hit>& get_calorimeter_hits()const
    {
      return calorimeter_hits_;
    }

    //! set calorimeter_hits                                                                                                      
    void set_calorimeter_hits(std::vector<topology::calorimeter_hit> calorimeter_hits)
    {
      calorimeter_hits_.clear();
      calorimeter_hits_ = calorimeter_hits;
    }

  protected:

    void fill_fast_information( );
    void fill_fast_information( mybhep::event& evt );
    void fill_fast_information( mybhep::particle* p );
    void fill_fast_information( mybhep::hit* h );
    int cell_side( topology::cell c);
    size_t near_level( topology::cell c1, topology::cell c2 );
    std::vector<topology::cell> get_near_cells(topology::cell c);
    void setup_cells();
    void setup_clusters();
    topology::calorimeter_hit make_calo_hit(mybhep::hit ahit, size_t id);
    int get_effective_layer(mybhep::hit hit);
    bool select_true_tracks(topology::tracked_data & __tracked_data);
    void make_plots(topology::tracked_data & __tracked_data);

  protected:
    
    //  NHistoManager2 hman;

    Clock clock;

    mybhep::prlevel level;
    
    mybhep::messenger m;
    int nevent;
    int InitialEvents;
    int SkippedEvents;
  
    //geom param
    double vel, rad, len, CellDistance;
    double xsize,ysize,zsize; //only for plotting 
    double calo_X, calo_Y, calo_Z;
    double InnerRadius, OuterRadius, FoilRadius; //only for plotting 
    double pmax;

    //limits
    double SmallRadius;
    double TangentPhi;
    double TangentTheta;
    double SmallNumber;
    double QuadrantAngle;
    double Ratio;
    double CompatibilityDistance;
    double MaxChi2;
    double nsigma;
    size_t nofflayers;
 
    //error parametrization
    double sigma0;
    double k0;
    double k1;
    double k2;
    double k3;

    double th0;
    double th1;
    double th2;
    double th3;
    
    double pnob;
    double pnot;
    double pnobt;
  
    double l0;
    double l1;

    // Support numbers
    double execution_time;
    bool PrintMode;
    bool SuperNemo;
    bool SuperNemoChannel; /** New initialization modeof the algorithm
                            *  for SuperNEMO and usage from Channel by
                            *  Falaise and Hereward.
                            *  Use the GG_CELL_pitch as the main geoemtry parameter 
                            *  of a GG cell, do not use 'rad' or 'CellDistance'
                            */
    bool NemoraOutput;
    bool N3_MC;
    double MaxTime;

    bool doDriftWires; 
    std::vector<POINT> DriftWires;

    mybhep::EventManager2* eman;

    int num_blocks;
    mybhep::dvector<double> planes_per_block ;
    mybhep::dvector<double> gaps_Z;
    double GG_CELL_pitch;
    double GG_GRND_diam;
    double GG_CELL_diam;
    double CHAMBER_X;
    double GG_BLOCK_X; 
    int num_cells_per_plane;
    double SOURCE_thick;
    size_t lastlayer;

    //  size_t dp_mode;

    //----Modification for bar-module---
  private: 
    string  _moduleNR;
    int     _MaxBlockSize;
    std::vector<mybhep::particle*> parts;

    //histogram file
    string hfile;
    bool is_good_couplet(topology::cell mainc, topology::cell candidatec, std::vector<topology::cell> nearmain);
    size_t get_true_hit_index(mybhep::hit& hit, bool print);
    size_t get_nemo_hit_index(mybhep::hit& hit, bool print);
    size_t get_calo_hit_index(topology::calorimeter_hit c);

  protected:
    void _set_defaults ();
    
  public:

    void setDoDriftWires(bool ddw){
      doDriftWires=ddw;
      return;
    }
    
    void compute_lastlayer(){
      lastlayer = 0;
      for(size_t i=0; i<planes_per_block.size(); i++){
        lastlayer += (int)planes_per_block[i];
      }
      return;
    }

    void set_GG_GRND_diam (double ggd){
      GG_GRND_diam = ggd;
      return;
    }

    void set_GG_CELL_diam (double ggcd){
      GG_CELL_diam = ggcd;
      return;
    }

    void set_lastlayer(int ll_){
      lastlayer = ll_;
      return;
    }

    void set_num_blocks(int nb){
      if (nb > 0)
        {       
          num_blocks = nb;
          planes_per_block.assign (num_blocks, 1);
        }
      else
        {
          std::cerr << "WARNING: CAT::clusterizer::set_num_blocks: " 
                    << "Invalid number of GG layer blocks !" << std::endl;
          planes_per_block.clear ();
          num_blocks = -1; // invalid value
        }
      return;
    }

    void set_planes_per_block(int block, int nplanes){
      if (block< 0 || block>=planes_per_block.size())
        {
          throw std::range_error ("CAT::clusterizer::set_planes_per_block: Invalid GG layer block index !");
        }
      if (nplanes > 0)
        {       
          planes_per_block.at (block) = nplanes;
        }
      else
        {
          throw std::range_error ("CAT::clusterizer::set_planes_per_block: Invalid number of GG layers in block !");
        }
      return;
    }

    void set_num_cells_per_plane(int ncpp){
      if (ncpp <= 0)
        {   
          num_cells_per_plane = -1; // invalid value
        }    
      else
        {
          num_cells_per_plane = ncpp;
        }
      return;
    }

    void set_SOURCE_thick(double st){
      if (st <= 0.0)
        {
          SOURCE_thick = std::numeric_limits<double>::quiet_NaN ();
        }    
      else
        {
          SOURCE_thick = st;
        }
      return;
    }
    
    // What is it ?
    void set_module_nr(string mID){
      _moduleNR=mID;
      return;
    }

    // What is it ?
    int get_module_nr(void){
      return _MaxBlockSize;
    }

    void set_MaxBlockSize(int mbs){
      _MaxBlockSize=mbs;
      return;
    }

    void set_pmax(double v){
     if ( v <= 0.0)
       {
         pmax = std::numeric_limits<double>::quiet_NaN ();
        }    
     else
       {
         pmax = v;
       }
      return;
    }

    void set_MaxTime(double v){
      MaxTime = v;
      return;
    }

    void set_PrintMode(bool v){
      PrintMode = v;
      return;
    }

    void set_SmallRadius(double v){
      SmallRadius = v;
      return;
    }

    void set_TangentPhi(double v){
      TangentPhi = v;
      return;
    }

    void set_TangentTheta(double v){
      TangentTheta = v;
      return;
    }

    void set_SmallNumber(double v){
      SmallNumber = v;
      return;
    }

    void set_QuadrantAngle(double v){
      QuadrantAngle = v;
      return;
    }

    void set_Ratio(double v){
      Ratio = v;
      return;
    }

    void set_CompatibilityDistance(double v){
      CompatibilityDistance = v;
      return;
    }

    void set_MaxChi2(double v){
      MaxChi2 = v;
      return;
    }

    void set_hfile(string v){
      hfile = v;
      return;
    }

    void set_nsigma(double v){
      nsigma = v;
      return;
    }

    void set_nofflayers(size_t v){
      nofflayers = v;
      return;
    }

    void set_level(string v){
      level = mybhep::get_info_level(v);
      m = mybhep::messenger(level);
      return;
    }

    void set_len(double v){
      len = v;
      return;
    }

    void set_vel(double v){
      vel = v;
      return;
    }

    void set_rad(double v){
      rad = v;
      return;
    }

    void set_GG_CELL_pitch (double p){
      GG_CELL_pitch = p;
      set_rad (GG_CELL_pitch / cos(M_PI/8.));
      set_GG_CELL_diam (rad);
      set_CellDistance (rad);
      return;
    }

    void set_CellDistance(double v){
      CellDistance = v;
      return;
    }

    void set_SuperNemo(bool v){
      SuperNemo = v;
      return;
    }

    void set_SuperNemoChannel(bool v){
      if (v)
        {
          set_SuperNemo (true);
          SuperNemoChannel = true;
          set_NemoraOutput (false);
          set_N3_MC (false);
          setDoDriftWires (false);
          set_MaxBlockSize (1);
        }
      else
        {
          SuperNemoChannel = false;
        }
      return;
    }

    void set_NemoraOutput(bool no){
       NemoraOutput = no;
      return;
    }
    
    void set_N3_MC(bool v){
       N3_MC = v;
      return;
    }

    void set_FoilRadius(double v){
      FoilRadius = v;
      return;
    }

    void set_xsize(double v){
      xsize = v;
      return;
    }

    void set_ysize(double v){
      ysize = v;
      return;
    }

    void set_zsize(double v){
      zsize = v;
      return;
    }

    //----------------------------------------

    // variables of NEMO3 standard analysis

    std::vector<int> run_list;
    double run_time;
    bool first_event;


  private:

    std::vector<topology::cell> cells_;
    std::vector<topology::cluster> clusters_;
    std::vector<topology::calorimeter_hit> calorimeter_hits_;
    std::vector<topology::sequence> true_sequences_;
    std::vector<topology::sequence> nemo_sequences_;

  };

}

#endif
