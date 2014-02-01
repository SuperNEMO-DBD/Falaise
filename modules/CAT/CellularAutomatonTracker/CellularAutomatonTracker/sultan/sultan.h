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

//#if CAT_WITH_DEVEL_ROOT == 1
#include "TApplication.h"
#include <TROOT.h>
#include "TFile.h"
#include "TTree.h"
//#endif

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
    bool assign_nodes_based_on_experimental_helix(std::vector<topology::node> nodes, std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix * b, std::vector<size_t> *neighbouring_cells);
    bool form_triplets_from_cells(std::vector<topology::node> nodes);
    bool form_helices_from_triplets(std::vector<topology::node> nodes, std::vector<topology::experimental_helix> *the_helices, size_t icluster);
    void sequentiate_cluster_with_experimental_vector(topology::cluster & cluster, size_t icluster);
    void sequentiate_cluster_with_experimental_vector_2(topology::cluster & cluster, size_t icluster);
    void sequentiate_cluster_with_experimental_vector_3(topology::cluster & cluster, size_t icluster);
    void sequentiate_cluster_with_experimental_vector_4(topology::cluster & cluster, size_t icluster);
    void make_name(topology::sequence & seq);
    bool late();
    void print_sequences() const;
    void print_a_sequence(const topology::sequence & sequence) const;
    void print_scenarios() const;
    void print_a_scenario(const topology::scenario & scenario) const;
    bool make_scenarios(topology::tracked_data &td);
    bool check_continous_cells(std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix *b);
    std::vector<topology::sequence> clean_up(std::vector<topology::sequence> seqs);

    //! get clusters
    const std::vector<topology::cluster>& get_clusters()const
    {
      return clusters_;
    }

    //! get triplets
    const std::vector<topology::cell_triplet>& get_triplets()const
    {
      return triplets_;
    }

    //! set clusters
    void set_clusters(std::vector<topology::cluster> clusters)
    {
      clusters_.clear();
      clusters_ = clusters;
    }

    //! set triplets
    void set_triplets(std::vector<topology::cell_triplet> triplets)
    {
      triplets_.clear();
      triplets_ = triplets;
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

    // module number (SuperNemo will be modular)
    void set_module_nr(std::string mID){
      _moduleNR=mID;
      return;
    }

    void SetModuleNR(std::string mID){
      _moduleNR=mID;
    };

    std::string get_module_nr(void){
      return _moduleNR;
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

    void set_print_event_display(bool v){
      print_event_display = v;
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

    void set_nsigmas(double v){
      nsigmas = v;
      return;
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
    double dist_limit_inf;
    double dist_limit_sup;
    double Rmin, Rmax;

    // Support numbers
    double execution_time;
    double max_time;
    bool SuperNemoChannel; /** New initialization modeof the algorithm
                            *  for SuperNEMO and usage from Channel by
                            *  Falaise and Hereward.
                            */

    double bfield;
    double nsigmas;

    std::string _moduleNR;

    bool print_event_display;
    void reset_triplets(){
      triplets_.clear();
    }


  private:
    std::vector<topology::cluster> clusters_;
    std::vector<topology::sequence> sequences_;
    std::vector<topology::cell> cells_;
    std::vector<topology::scenario> scenarios_;
    double run_time;
    topology::experimental_legendre_vector * experimental_legendre_vector;
    TFile *root_file_;
    std::vector<topology::cell_triplet> triplets_;

  };

} // end of namespace CAT

#endif
