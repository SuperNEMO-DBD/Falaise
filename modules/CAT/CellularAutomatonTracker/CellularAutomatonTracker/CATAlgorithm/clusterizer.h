/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm___pattern_rec2___
#define __CATAlgorithm___pattern_rec2___

//#include <CATAlgorithm/CAT_config.h>

#include <stdexcept>
#include <mybhep/gstore.h>
#include <mybhep/messenger.h>
#include <mybhep/event.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <mybhep/system_of_units.h>

#include <iostream>
#include <vector>

#include <mybhep/EventManager2.h>
//#include <CATUtils/NHistoManager2.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/experimental_double.h>

#include <CATAlgorithm/Clock.h>
#include <CATAlgorithm/cell_base.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/sequence_base.h>
#include <CATAlgorithm/tracked_data_base.h>

namespace CAT {

typedef struct {
  float x;
  float z;
} POINT;

/// The clusterizer algorithm
class clusterizer {
 public:
  clusterizer(void);

  clusterizer(const mybhep::gstore&);

  virtual ~clusterizer();

 protected:
  bool _initialize(void);

 public:
  bool initialize(void);
  bool initialize(const mybhep::sstore& store, const mybhep::gstore& gs,
                  mybhep::EventManager2* eman = 0);
  void initializeHistos(void);
  bool execute(mybhep::event& evt, int ievent);
  bool finalize();
  void finalizeHistos(void);
  void FillYPositions(mybhep::event& evt);
  void FillYPositions(mybhep::particle* p);
  void FillYPosition(mybhep::hit* h);
  void FillTrueVertexes(mybhep::event& evt);
  void FillTrueVertexes(mybhep::particle* p);
  void FillHistos(mybhep::event& evt);
  bool read_event(mybhep::event& evt, topology::tracked_data& tracked_data_);
  bool prepare_event(topology::tracked_data& tracked_data_);
  void read_true_sequences(mybhep::event& evt);
  void read_nemo_sequences(mybhep::event& evt);
  void read_true_sequences();
  void read_nemo_sequences();
  void print_cells(void) const;
  void print_calos(void) const;
  void clusterize(topology::tracked_data& tracked_data_);
  void clusterize_after_sultan(topology::tracked_data& tracked_data_);
  void print_clusters(void) const;
  void print_true_sequences(void) const;
  void print_nemo_sequences(void) const;
  void readDstProper(const mybhep::sstore& global, mybhep::EventManager2* eman);
  void readDstProper();
  void GenerateWires(void);
  double long_resolution(double Z, double d[3]) const;
  double long_resolution_1cthd(double Zdist) const;
  double GetYError(double y, float tup, float tdown, double direction[3]);
  void order_cells();

  //! get cells
  const std::vector<topology::cell>& get_cells() const;

  //! set cells
  void set_cells(const std::vector<topology::cell>& cells);

  //! get clusters
  const std::vector<topology::cluster>& get_clusters() const;

  //! set clusters
  void set_clusters(const std::vector<topology::cluster>& clusters);

  //! get calorimeter_hits
  const std::vector<topology::calorimeter_hit>& get_calorimeter_hits() const;

  //! set calorimeter_hits
  void set_calorimeter_hits(const std::vector<topology::calorimeter_hit>& calorimeter_hits);

 protected:
  void fill_fast_information();
  void fill_fast_information(mybhep::event& evt);
  void fill_fast_information(mybhep::particle* p);
  void fill_fast_information(mybhep::hit* h);
  int cell_side(const topology::cell& c);
  size_t near_level(const topology::cell& c1, const topology::cell& c2);
  std::vector<topology::cell> get_near_cells(const topology::cell& c);
  void setup_cells();
  void setup_clusters();
  topology::calorimeter_hit make_calo_hit(const mybhep::hit& ahit, size_t id);
  int get_effective_layer(const mybhep::hit& hit);
  bool select_true_tracks(topology::tracked_data& __tracked_data);
  void make_plots(topology::tracked_data& __tracked_data);

 protected:
  //  NHistoManager2 hman;

  Clock clock;

  mybhep::prlevel level;

  mybhep::messenger m;
  int nevent;
  int event_number;
  int InitialEvents;
  int SkippedEvents;

  // geom param
  double vel, rad, len, CellDistance;
  double xsize, ysize, zsize;  // only for plotting
  double calo_X, calo_Y, calo_Z;
  double InnerRadius, OuterRadius, FoilRadius;  // only for plotting
  double pmax;
  double bfield;

  // limits
  double SmallRadius;
  double TangentPhi;
  double TangentTheta;
  double SmallNumber;
  double QuadrantAngle;
  double Ratio;
  double CompatibilityDistance;
  double MaxChi2;
  double probmin;
  size_t nofflayers;
  int first_event_number;

  // error parametrization
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
  mybhep::dvector<double> planes_per_block;
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
  std::string _moduleNR;
  int _MaxBlockSize;
  std::vector<mybhep::particle*> parts;

  // histogram file
  std::string hfile;
  bool is_good_couplet(topology::cell* mainc, const topology::cell& candidatec,
                       const std::vector<topology::cell>& nearmain);
  size_t get_true_hit_index(mybhep::hit& hit, bool print);
  size_t get_nemo_hit_index(mybhep::hit& hit, bool print);
  size_t get_calo_hit_index(const topology::calorimeter_hit& c);

 protected:
  void _set_defaults();

 public:
  void setDoDriftWires(bool ddw);

  void compute_lastlayer();

  void set_GG_GRND_diam(double ggd);

  void set_GG_CELL_diam(double ggcd);

  void set_lastlayer(int ll_);

  void set_num_blocks(int nb);

  void set_planes_per_block(int block, int nplanes);

  void set_num_cells_per_plane(int ncpp);

  void set_SOURCE_thick(double st);

  // What is it ?
  void set_module_nr(const std::string& mID);

  // What is it ?
  int get_module_nr(void);

  void set_MaxBlockSize(int mbs);

  void set_pmax(double v);

  void set_MaxTime(double v);

  void set_PrintMode(bool v);

  void set_SmallRadius(double v);

  void set_TangentPhi(double v);

  void set_TangentTheta(double v);

  void set_SmallNumber(double v);

  void set_QuadrantAngle(double v);

  void set_Ratio(double v);

  void set_CompatibilityDistance(double v);

  void set_MaxChi2(double v);

  void set_hfile(std::string v);

  void set_probmin(double v);

  void set_nofflayers(size_t v);

  void set_first_event(int v);

  void set_level(std::string v);

  void set_len(double v);

  void set_vel(double v);

  void set_rad(double v);

  void set_GG_CELL_pitch(double p);

  void set_CellDistance(double v);

  void set_SuperNemo(bool v);

  void set_SuperNemoChannel(bool v);

  void set_NemoraOutput(bool no);

  void set_N3_MC(bool v);

  void set_FoilRadius(double v);

  void set_xsize(double v);

  void set_ysize(double v);

  void set_zsize(double v);

  void set_bfield(double v);

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

}  // namespace CAT

#endif
