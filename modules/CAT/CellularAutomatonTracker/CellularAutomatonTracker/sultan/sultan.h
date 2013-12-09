/* -*- mode: c++ -*- */
#ifndef __sultan___sultan___
#define __sultan___sultan___

#include <mybhep/messenger.h>
#include <mybhep/utilities.h>
#include <boost/cstdint.hpp>

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sultan/cell.h>
#include <sultan/cluster.h>
#include <sultan/sequence.h>
#include <sultan/experimental_double.h>
#include <sultan/Clock.h>
#include <sultan/tracked_data.h>
#include <sultan/experimental_helix.h>
#include <sultan/scenario.h>
#include <sultan/cell_triplet.h>
#include <sultan/experimental_legendre_vector.h>


namespace SULTAN {
  class sultan{

  public:
    sultan(void);
    virtual ~sultan();

  protected:
    void _set_defaults ();


  public:
    bool initialize( );
    bool finalize();
    void print_clocks();
    void read_properties( void );

    bool sequentiate(topology::tracked_data & tracked_data);
    bool assign_nodes_based_on_experimental_helix(std::vector<topology::node> nodes, std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix * b, std::vector<topology::experimental_helix> *helices);
    bool calculate_helices(std::vector<topology::node> nodes, std::vector<topology::experimental_helix> *the_helices);
    void sequentiate_cluster_with_experimental_vector(topology::cluster & cluster);
    void make_name(topology::sequence & seq);
    bool late();



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


  public:

    void set_GG_GRND_diam (double ggd){
      GG_GRND_diam = ggd;
      return;
    }

    void set_GG_CELL_diam (double ggcd){
      GG_CELL_diam = ggcd;
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

    // module number (SuperNemo will be modular)
    void set_module_nr(std::string mID){
      _moduleNR=mID;
      return;
    }

    int get_module_nr(void){
      return _MaxBlockSize;
    }

    void set_MaxBlockSize(int mbs){
      _MaxBlockSize=mbs;
      return;
    }

    void set_Emin(double v){
      if ( v <= 0.0)
	{
	  Emin = std::numeric_limits<double>::quiet_NaN ();
        }
      else
	{
	  Emin = v;
	}
      return;
    }

    void set_Emax(double v){
      if ( v <= 0.0)
	{
	  Emax = std::numeric_limits<double>::quiet_NaN ();
        }
      else
	{
	  Emax = v;
	}
      return;
    }

    void set_max_time(double v){
      max_time = v;
      return;
    }

    void set_probmin(double v){
      probmin = v;
      return;
    }

    void set_nsigma_r(double v){
      nsigma_r = v;
      return;
    }

    void set_nsigma_z(double v){
      nsigma_z = v;
      return;
    }

    void set_nofflayers(size_t v){
      nofflayers = v;
      return;
    }

    void set_first_event(size_t v){
      first_event_number = v;
      return;
    }

    void set_min_ncells_in_cluster(size_t v){
      min_ncells_in_cluster = v;
      return;
    }

    void set_ncells_between_triplet_min(size_t v){
      ncells_between_triplet_min = v;
      return;
    }

    void set_ncells_between_triplet_range(size_t v){
      ncells_between_triplet_range = v;
      return;
    }

    void set_level(std::string v){
      level = mybhep::get_info_level(v);
      m = mybhep::messenger(level);
      return;
    }

    void set_cell_distance(double v){
      cell_distance = v;
      return;
    }

    void set_SuperNemoChannel(bool v){
      if (v)
        {
          SuperNemoChannel = true;
          set_MaxBlockSize (1);
        }
      else
        {
          SuperNemoChannel = false;
        }
      return;
    }

    void set_foil_radius(double v){
      foil_radius = v;
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

    void set_bfield(double v){
      bfield = v;
      return;
    }

    void set_gaps_Z(std::vector<double> v){
      gaps_Z.clear();
      for(size_t i=0; i<v.size(); i++)
	gaps_Z.push_back(v[i]);
    }


  protected:

    Clock clock;

    mybhep::prlevel level;

    mybhep::messenger m;
    int nevent;
    int event_number;
    int initial_events;
    int skipped_events;

    //geom param
    double cell_distance;
    double xsize,ysize,zsize; //only for plotting
    double inner_radius, outer_radius, foil_radius; //only for plotting
    double Emin;
    double Emax;

    //limits
    double probmin;
    int nofflayers;
    int first_event_number;
    size_t min_ncells_in_cluster;
    size_t ncells_between_triplet_min;
    size_t ncells_between_triplet_range;
    double nsigma_r, nsigma_z;

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
    double max_time;
    bool SuperNemoChannel; /** New initialization modeof the algorithm
                            *  for SuperNEMO and usage from Channel by
                            *  Falaise and Hereward.
                            */

    int num_blocks;
    std::vector<double> planes_per_block ;
    std::vector<double> gaps_Z;
    double GG_GRND_diam;
    double GG_CELL_diam;
    double CHAMBER_X;
    double GG_BLOCK_X;
    int num_cells_per_plane;
    double SOURCE_thick;
    double bfield;
    int cell_max_number;

    double dist_limit_inf;
    double dist_limit_sup;
    double Rmin, Rmax;

    //  size_t dp_mode;

    //----Modification for bar-module---
  private:
    std::string  _moduleNR;
    int     _MaxBlockSize;

    int NFAMILY, NCOPY;

    std::vector<topology::cluster> clusters_;
    std::vector<topology::sequence> sequences_;
    std::vector<topology::cell> cells_;
    std::vector<topology::scenario> scenarios_;


    void print_sequences() const;
    void print_a_sequence(const topology::sequence & sequence) const;
    int gap_number(const topology::cell &c);
    void print_families( void );
    void print_scenarios() const;
    void print_a_scenario(const topology::scenario & scenario) const;
    bool make_scenarios(topology::tracked_data &td);
    bool check_continous_cells(std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix *b);

  public:
    void SetModuleNR(std::string mID){
      _moduleNR=mID;
    };
    //----------------------------------------

    // variables of nemo3 standard analysis

    std::vector<int> run_list;
    double run_time;
    topology::experimental_legendre_vector * experimental_legendre_vector;


  };

} // end of namespace CAT

#endif
