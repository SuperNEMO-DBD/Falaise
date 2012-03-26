/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm___pattern_rec___
#define __CATAlgorithm___pattern_rec___

#include <CATAlgorithm/CAT_config.h>

#include <mybhep/gstore.h>
#include <mybhep/messenger.h>
#include <mybhep/event.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <mybhep/system_of_units.h>
#include <boost/cstdint.hpp>

#if CAT_WITH_DEVEL_ROOT == 1
#include "TApplication.h"
#include <TROOT.h>
#include <TChain.h>
#include "TH2.h"
#include "TH1.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h" 
#include "TBox.h"
#include "TMarker.h"
#endif

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mybhep/EventManager2.h>
//#include <CATUtils/NHistoManager2.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/sequence.h>
#include <CATAlgorithm/logic_sequence.h>
#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/Clock.h>
#include <CATAlgorithm/plane.h>
#include <CATAlgorithm/tracked_data.h>
#include <CATAlgorithm/helix.h>
#include <CATAlgorithm/scenario.h>
#include <CATAlgorithm/logic_scenario.h>


namespace CAT {
  class sequentiator{

  public:
  
    sequentiator(mybhep::gstore);
  
    virtual ~sequentiator();

    bool initialize( mybhep::sstore store, mybhep::gstore gs, mybhep::EventManager2 *eman=0);
    void initializeHistos( void );
    bool execute(mybhep::event& evt, int ievent);
    void PrintInitialObjects(void);
    bool finalize();
    void finalizeHistos( void );
    void readDstProper(mybhep::sstore, mybhep::EventManager2 *eman=0);

    bool sequentiate(topology::tracked_data & __tracked_data);
    void sequentiate_cluster();
    void make_new_sequence(topology::node first_node);
    void make_copy_sequence(topology::node first_node);
    bool evolve(topology::sequence *sequence);
    void fill_links(topology::sequence *sequence);
    bool good_first_node(topology::node node, topology::node &first_node);
    void make_plots(topology::tracked_data __tracked_data);
    void plot_hard_scattering(topology::tracked_data __tracked_data);
    bool good_first_to_be_matched(topology::sequence& seq);
    bool match_gaps(void);


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

    //! get sequences
    const std::vector<topology::sequence>& get_sequences()const
    {
      return sequences_;
    }

    //! set sequences
    void set_sequences(std::vector<topology::sequence> sequences)
    {
      sequences_.clear();
      sequences_ = sequences;
    }

    bool late();

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
    int NOffLayers;

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
    bool NemoraOutput;
    bool N3_MC;
    double MaxTime;

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
    double bfield;
    int cell_max_number;


    //  size_t dp_mode;

    //----Modification for bar-module---
  private: 
    string  _moduleNR;
    int     _MaxBlockSize;
    std::vector<mybhep::particle*> parts;
    int NFAMILY, NCOPY;

    //histogram file
    string hfile;

    topology::cluster local_cluster_;

    std::vector<topology::cluster> clusters_;
    std::vector<topology::sequence> sequences_;

    // tables from switching from true to reco sequences
    std::vector<size_t> reco_sequence_of_true_;
    std::vector<size_t> true_sequence_of_reco_;
    std::vector<size_t> n_common_hits_for_reco_track_;
    std::vector<std::vector<size_t> > families_;
    std::vector<topology::scenario> scenarios_;


    void print_sequences();
    bool match_through_gaps();
    bool make_scenarios(topology::tracked_data &td);
    void interpret_physics(std::vector<topology::calorimeter_hit> calos);
    topology::plane get_foil_plane();
    void print_a_sequence(topology::sequence *sequence);
    void add_pair(topology::sequence *newsequence);
    bool clean_up_sequences();
    bool there_is_free_sequence_beginning_with(topology::cell c, size_t *index);
    void print_clocks();
    int gap_number(topology::cell c);
    int gap_number(topology::calorimeter_hit c);
    void make_table_of_true_and_reco_sequences(vector<topology::sequence> trueseqs);
    void rec_efficiency(vector<topology::sequence> trueseqs);
    size_t getCommonHits(topology::sequence tp, topology::sequence dp);
    void FillGGResiduals(topology::sequence tp, topology::sequence dp);
    void make_name(topology::sequence* seq);
    bool near(topology::cell c, topology::plane pl);
    double distance_from_foil(topology::experimental_point ep);
    bool direct_out_of_foil(void);
    void print_families( void );
    void make_families();
    bool can_add_family(topology::scenario sc, size_t* jmin, size_t* nfree, double* Chi2, size_t* noverlaps, int32_t* ndof, topology::tracked_data td);
    void print_scenarios();
    void print_a_scenario(topology::scenario *sc);
    size_t pick_best_scenario();
    bool can_be_linked(topology::sequence& p, bool inverted);
    bool can_match(topology::sequence s, size_t* jmin, bool& bestinvertA, bool& bestinvertB);
    bool select_nemo_tracks(topology::tracked_data & __tracked_data);


  public:
    void SetModuleNR(string mID){
      _moduleNR=mID;
    };
    //----------------------------------------

    // variables of nemo3 standard analysis

    std::vector<int> run_list;
    double run_time;
    bool first_event;


  };

} // end of namespace CAT 

#endif
