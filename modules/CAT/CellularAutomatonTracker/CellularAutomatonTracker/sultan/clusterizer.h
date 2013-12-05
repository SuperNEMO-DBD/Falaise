/* -*- mode: c++ -*- */
#ifndef __sultan___clusterizer___
#define __sultan___clusterizer___

#include <stdexcept>
#include <mybhep/messenger.h>

#include <iostream>
#include <vector>

#include <sultan/Clock.h>
#include <sultan/cell.h>
#include <sultan/experimental_double.h>
#include <sultan/cluster.h>
#include <sultan/sequence.h>
#include <sultan/tracked_data.h>


namespace SULTAN{

  /// The clusterizer algorithm
  class clusterizer{
    
  public:
    
    clusterizer(void);

    virtual ~clusterizer();

  public:
    bool initialize( void );
    bool finalize();
    void read_properties();
    bool prepare_event(topology::tracked_data & __tracked_data);
    void print_cells(void)const;
    void clusterize(topology::tracked_data & __tracked_data);
    void print_clusters(void) const;

    //! get cells
    const std::vector<topology::cell>& get_cells()const;

    //! set cells
    void set_cells(const std::vector<topology::cell> & cells);

    //! get clusters
    const std::vector<topology::cluster>& get_clusters()const;

    //! set clusters
    void set_clusters(const std::vector<topology::cluster> & clusters);

  protected:

    int cell_side( const topology::cell & c);
    std::vector<topology::cell> get_near_cells(const topology::cell & c);
    void setup_cells();
    void setup_clusters();

  protected:

    Clock clock;

    mybhep::prlevel level;

    mybhep::messenger m;
    int nevent;
    int event_number;
    int initial_events;
    int skipped_events;

    //geom param
    double vel, rad, len, cell_distance;
    double xsize,ysize,zsize; //only for plotting
    double calo_X, calo_Y, calo_Z;
    double inner_radius, outer_radius, foil_radius; //only for plotting
    double Emin;
    double bfield;

    //limits
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
    bool SuperNemoChannel; /** New initialization modeof the algorithm
                            *  for SuperNEMO and usage from Channel by
                            *  Falaise and Hereward.
                            */
    double max_time;

    int num_blocks;
    std::vector<double> planes_per_block ;
    std::vector<double> gaps_Z;
    double GG_GRND_diam;
    double GG_CELL_diam;
    double CHAMBER_X;
    double GG_BLOCK_X;
    int num_cells_per_plane;
    double SOURCE_thick;
    size_t lastlayer;


  private:
    std::string  _moduleNR;
    int     _MaxBlockSize;

    //histogram file
    std::string hfile;

  protected:
    void _set_defaults ();

  public:

    void compute_lastlayer();

    void set_GG_GRND_diam (double ggd);

    void set_GG_CELL_diam (double ggcd);

    void set_lastlayer(int ll_);

    void set_num_blocks(int nb);

    void set_planes_per_block(int block, int nplanes);

    void set_num_cells_per_plane(int ncpp);

    void set_SOURCE_thick(double st);

    // What is it ?
    void set_module_nr(const std::string &mID);

    // What is it ?
    int get_module_nr(void);

    void set_MaxBlockSize(int mbs);

    void set_Emin(double v);

    void set_max_time(double v);

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

    void set_cell_distance(double v);

    void set_SuperNemoChannel(bool v);

    void set_foil_radius(double v);

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

  };

}

#endif
