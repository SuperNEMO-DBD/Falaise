/* -*- mode: c++ -*- */
#ifndef _event_display___
#define _event_display___

#include <mybhep/gstore.h>
#include <mybhep/messenger.h>
#include <mybhep/event.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <mybhep/system_of_units.h>
#include "TApplication.h"
#include <TROOT.h>
#include <TChain.h>
#include "TH2.h"
#include "TH1.h"
#include "TGraph.h"
#include "TLine.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h" 
#include "TPaveLabel.h"
#include "TPaveText.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TBox.h"
#include <mybhep/EventManager2.h>
#include "TMarker.h"
#include "CATAlgorithm/clusterizer.h"
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/cell_base.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/sequence_base.h>
#include <CATAlgorithm/helix.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <sultan/tracked_data.h>
#include <mybhep/utilities.h>
#include <CATAlgorithm/scenario.h>


typedef struct{
  float calorimeter;
  float foil;
  float AnodeWire;
  float GroundWire;
} FLAG;

typedef struct{
  CAT::POINT center;
  float radius;
} CIRCLE;

typedef struct{
  CIRCLE circle;
  float y;
  float erry;
  float tup;
  float tdown;
  float tdelay;
  int block;
  int plane;
  int id;
  int n3id;
} POSITION;

typedef struct{
  double pos[3];
  int gg_index;
} N3POINT;

typedef struct{
  double center[3];
  double radius;
  double pitch;
  double charge;
} N3HELIX;

typedef struct{
  double foil[3];
  std::vector<N3POINT> points;
  N3HELIX helix;
  double E;
} N3TRACK;

typedef struct{
  double vertex[3];
  std::vector<N3TRACK> tracks;
  Int_t date;
  Int_t run;
  Int_t time;
  Int_t nevga;
} N3EVENT;

typedef struct{
  double pos[3];
  double E;
  double t;
  double t_error;
} CALOHIT;

/*
typedef struct{
  size_t id;
  size_t iteration;
  size_t total;
  bool free;
  bool begun;
} LINK;

typedef struct{
  float x;
  float z;
} POINT;

typedef struct{
  POINT a;
  POINT b;
} LINE;

typedef struct{
  size_t cell[3];
  std::vector< std::vector< POINT > > segments;
} TRIPLET;

typedef struct{
  size_t id;
  std::vector<LINK> link;
  POINT point;
  bool free;
} NODE;

typedef struct{
  std::vector<NODE> node;
  bool free;
} SEQUENCE;

*/


class EventDisplay{

 public:

  TH2F *mapzx;
  TH2F *mapzy;
  TCanvas *canvas;
  std::vector<TGraph*> graphlist; 
  std::vector<TPaveText*> pavelist; 
  TLegend *leg_xz;
  TLegend *leg_yz;
  TLegend *leg_xz_true;
  TLegend *leg_yz_true;

  TMarker *FirstStarXY;
  bool FirstStarXY_used;

 public:
  
  EventDisplay(mybhep::gstore);
  EventDisplay(void);
  
  virtual ~EventDisplay();

  void fill_neighbours( std::vector<std::vector<POSITION> > sunami_position, std::vector< std::vector<size_t> > &neighbours );
  void DeleteDisplay(void);
  void EventDisplayExecute(mybhep::event& evt, int ievent );
  bool InitializeDisplayEvent( mybhep::sstore store, mybhep::gstore gs , mybhep::EventManager2 *eman);
  void SetPlutsMode( bool plutsmode );
  bool CheckCells( void );
  void readDstProper(mybhep::sstore, mybhep::EventManager2 *eman=0);
  void get_pos(mybhep::hit ahit, float pos[7]);
  void fill_sunami_positions(mybhep::event& evt, std::vector<std::vector<POSITION> > &sunami_position);
  void fill_true_positions(mybhep::event& evt);
  void DrawCircleXZ( void );
  std::string GetName(std::string name);
  void DrawHitXZ(mybhep::event& evt, std::string mode);
  void DrawCircleYZ( void );
  void DrawHitYZ(mybhep::event& evt, std::string mode);
  void GetCellId(mybhep::hit hit,int &block,int &plane,int &id, int &n3id);
  void GetPlotLimit( std::vector<CAT::topology::calorimeter_hit> calos );
  void DrawDetectorXZ( void );
  void DrawDetectorCircleXZ( double radius, size_t color );
  void DrawDetectorYZ( void );
  void GenerateWires( void );
  void EventDisplayXZ(mybhep::event& evt, std::string mode);
  void EventDisplayYZ(mybhep::event& evt, std::string mode);
  void Nemo3EventDisplayXZ(void);
  void Nemo3EventDisplayYZ(void);
  void DrawNemo3HitXZ( void );
  void DrawNemo3HitYZ( void );
  void fill_nemo3_event(mybhep::event& evt);
  bool same_cell( POSITION a, POSITION b );
  bool fill_parts(std::vector<mybhep::particle*> digi_parts);
  void fill_parts_display(  mybhep::event& evt );
  void fill_nemo3_parts_display(  mybhep::event& evt );
  float FastHit(POSITION position);
  void Finalize(void);
  void readDstProper();
  void initialize(void);

  void set_PlotTopView(bool aValue) { PlotTopView = aValue; }
  void set_PlotSideView(bool aValue) { PlotSideView = aValue; }
  void set_PlotInitialHits(bool aValue) { PlotInitialHits = aValue; }
  void set_PlotTangents(bool aValue) { PlotTangents = aValue; }
  void set_PlotTriplets(bool aValue) { PlotTriplets = aValue; }
  void set_PlotTrueTracks(bool aValue) { PlotTrueTracks = aValue; }
  void set_PlotCats(bool aValue) { PlotCats = aValue; }
  void set_PlotSultan(bool aValue) { PlotSultan = aValue; }
  void set_PlotNemoraTracks(bool aValue) { PlotNemoraTracks = aValue; }
  void set_PlotNemoTracks(bool aValue) { PlotNemoTracks = aValue; }
  void set_PlotCellIds(bool aValue) { PlotCellIds = aValue; }
  void set_PlotCellChi2s(bool aValue) { PlotCellChi2s = aValue; }
  void set_PlotCalos(bool aValue) { PlotCalos = aValue; }
  void set_UseCalosInZoom(bool aValue) { UseCalosInZoom = aValue; }
  void set_PlotCaloEnergy(bool aValue) { PlotCaloEnergy = aValue; }
  void set_PlotHelices(bool aValue) { PlotHelices = aValue; }
  void set_PlotLegend(bool aValue) { PlotLegend = aValue; }
  void set_PlotFormat(std::string aValue) { PlotFormat = aValue; }
  
  
 protected:
    
  mybhep::prlevel level;
    
  mybhep::messenger m;
  
  bool PlutsMode;

  //geom param
  double CellDistance;
  double xsize,ysize,zsize; //only for plotting 
  double InnerRadius, OuterRadius, FoilRadius; //only for plotting 
  double calo_X, calo_Y, calo_Z;
  int PlotTopView;
  int PlotSideView;
  int PlotInitialHits;
  int PlotTangents;
  int PlotTriplets;
  int PlotCats;
  int PlotSultan;
  int PlotTrueTracks;
  int PlotNemoraTracks;
  int PlotNemoTracks;
  int PlotCellIds;
  int PlotCellChi2s;
  int PlotCalos;
  int UseCalosInZoom;
  int PlotCaloEnergy;
  int PlotHelices;
  int PlotLegend;
  int icanvas;
  std::string PlotFormat;
  int first_event_number;
  int event_number;

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

  // std::vector of sunami positions
  N3EVENT n3event;
  std::vector<std::vector<POSITION> > sunami_position;
  std::vector<std::vector<POSITION> > true_position;
  std::vector<CALOHIT> calo_position;
  std::vector<FLAG> true_flags;

  // Support numbers
  double execution_time;
  bool PrintMode;
  bool SuperNemo;
  bool NemoraOutput;
  bool N3_MC;

  size_t Nbins;
  std::vector<size_t> PurityTracks;
  std::vector<double> AveragePurity;
  std::vector<size_t> EfficiencyTracks;
  std::vector<double> AverageEfficiency;
  std::vector<size_t> ResTracks;
  std::vector<double> AverageResx;
  std::vector<double> AverageResz;

  size_t FlagTracks[2];
  double AverageCalorimeter[2];
  double AverageFoil[2];
  double AverageAW[2];
  double AverageGW[2];
  double AverageAll[2];

  mybhep::EventManager2* eman;

  std::vector<CAT::POINT> DriftWires;

  int num_blocks;
  mybhep::dvector<double> planes_per_block ;
  mybhep::dvector<double> gaps_Z;
  double GG_CELL_pitch;
  double GG_GRND_diam;
  double GG_CELL_diam;
  double CHAMBER_X;
  double CHAMBER_height; 
  double GG_BLOCK_X; 
  double GG_BLOCK_Y;
  int num_cells_per_plane;
  double SOURCE_thick;
      

  Color_t color_detector;
  Color_t color_cells;
  Color_t color_fast_cells;
  Color_t color_cats;
  Color_t color_sultan;
  Color_t color_nemora;
  Color_t color_true_track;
  Color_t color_tangent;
  Color_t color_triplet;
  Color_t color_helix;
  Color_t color_calos;
  float Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;


  /////// topological tracking 
public:
  void execute(mybhep::event& evt, size_t ievent, CAT::topology::tracked_data & __tracked_data);
  void execute(size_t ievent, CAT::topology::tracked_data & __CAT_tracked_data, SULTAN::topology::tracked_data & __SULTAN_tracked_data);

  //! get cells 
  const std::vector<CAT::topology::cell>& get_cells()const
  {
    return cells_;
  }

  //! set cells 
  void set_cells(std::vector<CAT::topology::cell> cells)
  {
    cells_.clear();
    cells_ = cells;
  }


  //! get clusters 
  const std::vector<CAT::topology::cluster>& get_clusters()const
  {
    return clusters_;
  }

  //! set clusters 
  void set_clusters(std::vector<CAT::topology::cluster> clusters)
  {
    clusters_.clear();
    clusters_ = clusters;
  }


  //! get sequences
  const std::vector<CAT::topology::sequence>& get_CAT_sequences()const
  {
    return CAT_sequences_;
  }

  //! get nemo sequences
  const std::vector<CAT::topology::sequence>& get_nemo_sequences()const
  {
    return nemo_sequences_;
  }

  //! set sequences
  void set_CAT_sequences(std::vector<CAT::topology::sequence> sequences)
  {
    CAT_sequences_.clear();
    CAT_sequences_ = sequences;
  }

  //! set nemo sequences
  void set_nemo_sequences(std::vector<CAT::topology::sequence> sequences)
  {
    nemo_sequences_.clear();
    nemo_sequences_ = sequences;
  }


public:

  void set_level(std::string v){
    level = mybhep::get_info_level(v);
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

  void set_FoilRadius(double v){
    FoilRadius = v;
    return;
  }

  void set_InnerRadius(double aValue) { InnerRadius = aValue; }
  void set_OuterRadius(double aValue) { OuterRadius = aValue; }

  void set_calo_X(double aValue) { calo_X = aValue; }
  void set_calo_Y(double aValue) { calo_Y = aValue; }
  void set_calo_Z(double aValue) { calo_Z = aValue; }

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


private:
  void event_display_xz(std::string mode, CAT::topology::tracked_data td);
  void draw_circle_xz( double x0, double z0, double radius, size_t color, size_t thickness, double phi1, double phi2);
  void draw_initial_hits_xz( void );
  void draw_calos_xz( std::vector<CAT::topology::calorimeter_hit> calos );
  void draw_sine_yz( double y0, double z0, double radius, double pitch, size_t color, size_t thickness, double phi1 , double phi2);
  void draw_cats_xz(std::string mode, std::vector<CAT::topology::sequence> true_seqs);
  void event_display_yz(std::string mode, CAT::topology::tracked_data td);
  void draw_calos_yz( std::vector<CAT::topology::calorimeter_hit> calos );
  void draw_initial_hits_yz( void );
  void draw_cats_yz(std::string mode, std::vector<CAT::topology::sequence> true_seqs);
  void print_cells(void);
  void draw_tangents_xz( void );
  void draw_tangents_yz( void );
  void draw_helices_xz( std::string mode );
  void draw_helices_yz( std::string mode );
  void draw_triplets_xz( void );
  void draw_triplets_yz( void );
  double get_x_ndc(double x);
  double get_y_ndc(double y);
  double get_z_ndc(double z);
  std::string get_name(std::string name){
    size_t i1 = name.find("_");
    name = name.substr(0,i1);
    return name;
  }
  int getCalWalli( CAT::topology::calorimeter_hit h );
  int quadrant_xz( CAT::topology::cell c );
  int quadrant_yz( CAT::topology::cell c );
  void init_quadrant_counters();
  void locate_legend_xz();
  void locate_legend_yz();
  size_t least_occupied_quadrant();

  std::vector<CAT::topology::cell> cells_;
  std::vector<CAT::topology::cluster> clusters_;
  std::vector<CAT::topology::sequence> CAT_sequences_;
  std::vector<CAT::topology::sequence> nemo_sequences_;

  size_t n_cells_quad[4];


  //  size_t dp_mode;

//----Modification for bar-module---
private: 
  std::string  _moduleNR;
  int     _MaxBlockSize;
  std::vector<mybhep::particle*> parts;
  std::vector<mybhep::particle*> nemo3_parts;
    
public:
  void SetModuleNR(std::string mID){
    _moduleNR=mID;
  };

  std::vector<Int_t> run_list;
  double run_time;
  bool first_event;



};



#endif
