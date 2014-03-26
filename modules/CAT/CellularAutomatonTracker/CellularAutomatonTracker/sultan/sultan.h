/* -*- mode: c++ -*- */
#ifndef SULTAN_SULTAN_H
#define SULTAN_SULTAN_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <boost/cstdint.hpp>

#include <mybhep/messenger.h>
#include <mybhep/utilities.h>

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
#include <sultan/experimental_line.h>
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
    bool assign_nodes_based_on_experimental_helix(topology::experimental_helix * b, std::vector<topology::experimental_helix> *helices);
    bool assign_nodes_based_on_experimental_helix(topology::experimental_helix * b, std::vector<size_t> *neighbouring_cells);
    bool form_triplets_from_cells();
    bool form_helices_from_triplets(std::vector<topology::experimental_helix> *the_helices, size_t icluster);
    void sequentiate_cluster_with_experimental_vector(size_t icluster);
    void sequentiate_cluster_with_experimental_vector_2(topology::cluster & cluster, size_t icluster);
    void sequentiate_cluster_with_experimental_vector_3(topology::cluster & cluster, size_t icluster);
    void sequentiate_cluster_with_experimental_vector_4(topology::cluster & cluster, size_t icluster);
    void reduce_cluster_based_on_endpoints(size_t icluster);
    void reduce_cluster__with_vector_of_clusters_of_endpoints(size_t icluster, std::vector<topology::cluster> * cs, std::vector<topology::cluster> clusters_of_endpoints, bool * cluster_is_finished );
    void reduce_cluster__with_2_clusters_of_endpoints(size_t icluster, bool * cluster_is_finished,  const std::vector<topology::node> &inodes,  const std::vector<topology::node> &jnodes, std::vector<topology::cluster> * cs );
    void reduce_cluster__with_2_endpoints(size_t icluster, bool * cluster_is_finished, std::vector<topology::cluster> * cs_given_endpoints, std::vector<topology::node>::const_iterator inode , std::vector<topology::node>::const_iterator jnode  ) ;
    void make_name(topology::sequence & seq);
    bool late();
    void print_sequences() const;
    void print_a_sequence(const topology::sequence & sequence) const;
    void print_scenarios() const;
    void print_a_scenario(const topology::scenario & scenario) const;
    bool make_scenarios(topology::tracked_data &td);
    bool check_continous_cells(topology::experimental_helix *b);
    bool get_longest_piece(topology::cluster * given_cluster, topology::node a, topology::node b, topology::cluster * longest_piece);
    std::vector<topology::sequence> clean_up(std::vector<topology::sequence> seqs);
    std::vector<topology::cluster> clean_up(std::vector<topology::cluster> clusters);
    void assign_nodes_of_clusters(std::vector<topology::cluster> clusters);
    std::vector<topology::cluster> get_clusters_of_cells_to_be_used_as_end_points();
    topology::cluster get_helix_cluster_from(topology::cell_triplet t, topology::experimental_helix helix);
    topology::cluster add_cells_to_helix_cluster_from(topology::cluster c, topology::cell_triplet t, topology::experimental_helix helix);
    void assign_nodes_of_cluster(topology::cluster c);
    topology::cluster get_line_cluster_from(topology::node a, topology::node b);
    topology::cluster add_cells_to_line_cluster_from(topology::experimental_line line,  size_t ida, size_t idb, topology::cluster cluster);
    void get_line_clusters_from(topology::node a, topology::node b, size_t icluster, bool *cluster_is_finished, std::vector<topology::cluster> * cs);
    void get_helix_clusters_from(topology::node a, topology::node b, size_t icluster, bool *cluster_is_finished, std::vector<topology::cluster> * cs);
    std::vector<topology::cluster> get_clusters_from(topology::node a, topology::node b, size_t icluster, bool * cluster_is_finished);
    void create_sequence_from_cluster(std::vector<topology::sequence> * sequences, const topology::cluster & c);
    void get_angle_of_point( topology::experimental_point * p, double * angle);
    std::vector<topology::cluster> make_unclustered_hits(topology::node * n );
    void make_sequences_from_clusters();
    void reset();
    bool line_is_near_cell(topology::experimental_line line, topology::experimental_double * DR, topology::experimental_double * DH, topology::node * node);
    bool helix_is_near_cell(topology::cell_triplet t, topology::experimental_helix helix, topology::experimental_double * DR, topology::experimental_double * DH, topology::node * node);
    topology::cluster get_best_cluster_from(std::vector<topology::cluster> cs);


    //! get clusters
    const std::vector<topology::cluster>& get_clusters()const
    {
      return clusters_;
    }

    //! get leftover_cluster
    const topology::cluster & get_leftover_cluster()const
    {
      return *leftover_cluster_;
    }

    //! get assigned_cluster
    const topology::cluster & get_assigned_cluster()const
    {
      return *assigned_cluster_;
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

    //! set leftover_cluster
    void set_leftover_cluster(topology::cluster c)
    {
      *leftover_cluster_ = c;
    }

    //! set assigned_cluster
    void set_assigned_cluster(topology::cluster c)
    {
      *assigned_cluster_ = c;
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

    bool check_if_cell_is_near_calo(topology::cell c);

    void reduce_clusters();

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

    void status();


  private:
    // vector of clusters of neighbouring cells (input)
    std::vector<topology::cluster> clusters_;

    // vector of scenarios returned by sultan (output)
    std::vector<topology::scenario> scenarios_;

    // vector of sequences to be returned
    std::vector<topology::sequence> sequences_;

    // vector of clusters to be returned
    std::vector<topology::cluster> made_clusters_;

    // all the gg cells in the event
    std::vector<topology::cell> cells_;

    // all the calo hits in the event
    std::vector<topology::calorimeter_hit> calos_;

    // cluster of neighbouring cells under study:
    topology::cluster * full_cluster_;

    // all the cell triplets under study
    std::vector<topology::cell_triplet> triplets_;

    // cluster of neighbouring cells under study: leftover hits
    topology::cluster * leftover_cluster_;

    // cluster of neighbouring cells under study: assigned hits
    topology::cluster * assigned_cluster_;


    double run_time;
    topology::experimental_legendre_vector * experimental_legendre_vector;
    TFile *root_file_;

  };

} // end of namespace SULTAN

#endif // SULTAN_SULTAN_H
