/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ISEQUENCE
#define __CATAlgorithm__ISEQUENCE
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <boost/cstdint.hpp>

#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/helix.h>
#include <CATAlgorithm/plane.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/calorimeter_hit.h>



namespace CAT {
  namespace topology{



    class sequence : public tracking_object{

      // a sequence is composed of a list of nodes

    protected:
      std::string appname_;

      // list of all chi2 values found in the construction
      // of the sequence (some of them discarded, kept only for plotting)
      std::vector<double> chi2s_all_;

      // list of all prob values
      std::vector<double> probs_all_;

      // list of helix_chi2 values
      std::vector<double> helix_chi2s_;

      // fitted helix
      helix helix_;

      experimental_double helix_length_;

      experimental_double tangent_length_;

      experimental_double charge_;

      experimental_double helix_charge_;

      experimental_double detailed_charge_;

      experimental_vector momentum_;

      bool primary_;

    public:

      // list of nodes
      std::vector<node> nodes_;

      // status of sequence
      bool free_;

      // sequence's name(s)
      std::vector<std::string> names_;

      bool has_momentum_;
      bool has_helix_;

      bool has_charge_;
      bool has_helix_charge_;
      bool has_detailed_charge_;

      bool has_helix_length_;
      bool has_tangent_length_;

      // sequence's vertex: from helix and from tangents
      bool has_helix_vertex_;
      experimental_point helix_vertex_;
      std::string helix_vertex_type_;
      size_t helix_vertex_id_;
      bool has_tangent_vertex_;
      experimental_point tangent_vertex_;
      std::string tangent_vertex_type_;
      size_t tangent_vertex_id_;


      // sequence's decay_vertex
      bool has_decay_helix_vertex_;
      experimental_point decay_helix_vertex_;
      std::string decay_helix_vertex_type_;
      size_t calo_helix_id_;
      bool has_decay_tangent_vertex_;
      experimental_point decay_tangent_vertex_;
      std::string decay_tangent_vertex_type_;
      size_t calo_tangent_id_;

      //!Default constructor
      sequence();

      //!Default destructor
      virtual ~sequence();

      //! constructor from std::vector of nodes
      sequence(const std::vector<node>&  nodes, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      //! constructor from single node
      sequence(const node &node, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set nodes
      void set_nodes(const std::vector<node> &nodes);

      //! set free level
      void set_free(bool free);

      //! set name
      void set_name(const std::string & name);

      //! set names
      void set_names(const std::vector<std::string> &names);

      //! add name
      void add_name(const std::string &name);

      //! set helix_vertex
      void set_helix_vertex(const experimental_point & v, const std::string & type, size_t calo_helix_id = 0);

      //! set decay_helix_vertex
      void set_decay_helix_vertex(const experimental_point &v, const std::string &type, size_t calo_helix_id = 0);

      //! set tangent_vertex
      void set_tangent_vertex(const experimental_point & v, const std::string & type, size_t calo_tangent_id = 0);

      //! set decay_tangent_vertex
      void set_decay_tangent_vertex(const experimental_point & v, const std::string & type, size_t calo_tangent_id = 0);

      //! set chi2 list
      void set_chi2s(const std::vector<double> & chi2s);

      //! set helix_chi2 list
      void set_helix_chi2s(const std::vector<double> & helix_chi2s);

      //! set prob list
      void set_probs(const std::vector<double> & probs);

      //! set charge
      void set_charge(const experimental_double &charge);

      //! set helix_charge
      void set_helix_charge(const experimental_double &helix_charge);

      //! set detailed_charge
      void set_detailed_charge(const experimental_double &detailed_charge);

      //! set tangent length
      void set_tangent_length(const experimental_double &length);

      //! set helix length
      void set_helix_length(const experimental_double &helix_length);

      //! set momentum
      void set_momentum(const experimental_vector &mom);

      //! set primary
      void set_primary(bool primary);

      //! set helix
      void set_helix(const helix & h);

      //! get nodes
      const std::vector<node> & nodes()const;

      //! get free level
      bool Free()const;

      //! get name
      const std::string & name()const;

      //! get names
      const std::vector<std::string> & names()const;

      //! get primary
      bool primary()const;

      //! get list of all chi2
      const std::vector<double>& chi2s_all() const;

      //! get list of prob
      const std::vector<double> & probs_all() const;

      //! get list of chi2 used in the sequence
      std::vector<double> chi2s()const;

      //! get list of helix_chi2
      const std::vector<double> & helix_chi2s() const;

      //! get list of probs used in the sequence
      std::vector<double> probs()const;

      // get chi2 of sequence
      double chi2()const;

      // get helix_chi2 of sequence
      double helix_chi2()const;

      // get prob of sequence
      double Prob()const;

      // get ndof of sequence
      int32_t ndof()const;

      // get prob of sequence
      double helix_Prob()const;

      //! has momentum
      bool has_momentum()const;

      //! has helix
      bool has_helix()const;

      //! has charge
      bool has_charge()const;

      //! has helix_charge
      bool has_helix_charge()const;

      //! has detailed_charge
      bool has_detailed_charge()const;

      //! has tangent length
      bool has_tangent_length()const;

      //! has helix length
      bool has_helix_length()const;

      //! has helix_vertex
      bool has_helix_vertex()const;

      //! get helix_vertex
      const experimental_point & helix_vertex()const;

      //! get helix_vertex type
      const std::string & helix_vertex_type()const;

      //! get helix_vertex id
      size_t helix_vertex_id()const;

      //! has decay_helix_vertex
      bool has_decay_helix_vertex()const;

      //! get decay_helix_vertex
      const experimental_point & decay_helix_vertex()const;

      //! get calo id
      size_t calo_helix_id()const;

      //! get decay helix_vertex type
      const std::string & decay_helix_vertex_type()const;

      //! has tangent_vertex
      bool has_tangent_vertex()const;

      //! get tangent_vertex
      const experimental_point & tangent_vertex()const;

      //! get tangent_vertex type
      const std::string & tangent_vertex_type()const;

      //! get tangent_vertex id
      size_t tangent_vertex_id()const;

      //! has decay_tangent_vertex
      bool has_decay_tangent_vertex()const;

      //! get decay_tangent_vertex
      const experimental_point & decay_tangent_vertex()const;

      //! get calo id
      size_t calo_tangent_id()const;

      //! get decay tangent_vertex type
      const std::string & decay_tangent_vertex_type()const;

      //! get charge
      const experimental_double & charge() const;

      //! get helix_charge
      const experimental_double & helix_charge() const;

      //! get detailed_charge
      const experimental_double & detailed_charge() const;

      //! get tangent length
      const experimental_double & tangent_length() const;

      //! get helix length
      const experimental_double & helix_length() const;

      //! get momentum
      const experimental_vector & momentum() const;

      //! get helix
      const helix & get_helix() const;


      //! get fast
      bool fast()const;


      //! get last node
      node last_node()const;


      //! get second last node
      node second_last_node()const;


      //! get middle node
      node middle_node()const;

      bool has_kink(void) const;
      bool has_kink(std::vector<size_t> *index) const;

    public:

      bool has_cell(const cell & c)const;

      sequence invert();

      void set_free_level();

      //! get last free node
      void last_free_node(size_t *lfn, size_t *link);

      topology::node node_of_cell(const topology::cell & c);

      bool contained(const topology::sequence & big)const;

      bool contained_same_size_and_cells(const topology::sequence & big)const;

      bool contained_same_extreme_quadrants(const topology::sequence & big)const;

      bool is_bridge(const topology::sequence & first,const topology::sequence & second)const;

      void increase_iteration(size_t inode, size_t ilink);

      sequence copy_to_last_free_node(size_t *ilfn, size_t *ilink);

      void fill_links_of_node(size_t inode, cluster local_sequence);

      bool pick_new_cell(size_t *ilink, experimental_point *newp, cluster local_cluster);

      bool compatible(joint *j, cell cc);

      void get_chi2_change_for_changing_end_of_sequence(const topology::experimental_point &epa, const topology::experimental_point &epb, double *delta_chi_A, double *delta_chi_alpha);

      int get_link_index_of_cell(size_t inode, const cell &link) const;

      bool calculate_helix(void);

      const experimental_double & radius() const;

      const experimental_double & pitch()const;

      const experimental_point & center()const;

      void calculate_momentum(double bfield);

      void calculate_charge(void);

      void calculate_length(void);

      bool intersect_plane_with_tangent_from_end(const plane & pl,
                                                 experimental_point * ep)const;

      bool intersect_plane_from_end(const plane & pl, experimental_point * ep)const;

      bool intersect_plane_with_tangent_from_begin(const plane & pl, experimental_point * ep)const;

      bool intersect_plane_from_begin(const plane & pl, experimental_point * ep)const;

      bool intersect_circle_with_tangent_from_end(const circle & c, experimental_point * ep)const;

      bool intersect_circle_from_end(const circle &c, experimental_point * ep)const;

      bool intersect_circle_from_end_minus_one(const circle & c, experimental_point * ep)const;

      bool intersect_circle_with_tangent_from_begin(const circle & c, experimental_point * ep)const;

      bool intersect_circle_from_begin(const circle & c, experimental_point * ep)const;

      bool intersect_circle_from_begin_minus_one(const circle & c, experimental_point * ep)const;

      void remove_first_node();

      void remove_last_node();

      bool intersect_sequence(sequence & seq, bool invertA, bool invertB, bool acrossGAP, experimental_point * ep, double limit_distance, int* with_kink, int cells_to_delete);

      std::string family()const;

      std::vector<size_t> families()const;

      bool good_match(const sequence & seq,
                      bool &invertA, bool &invertB, size_t NOffLayers)const;


      sequence match(sequence & seq, bool invertA, bool invertB, bool *ok, int with_kink, int cells_to_delete);

      bool good_match_with_kink(const sequence & seq,
                                bool &invertA, bool &invertB, bool &acrossGAP,
                                double limit_distance, size_t NOffLayers,
				int &cells_to_delete)const;

      bool same_families(const topology::sequence & s)const;

      experimental_double delta_phi(const experimental_point & epa,
                       const experimental_point & epb)const;

      // is the (true) track all on one side of the foil?
      bool one_side()const;

      experimental_vector initial_dir()const;

      experimental_vector final_dir()const;

      //! get node of worst chi2
      bool worst_node(topology::node& n)const;

      // get node of largest horizontal kink
      bool largest_kink_node(topology::node& n, double& phi)const;

      bool helix_out_of_range(double lim);

      void point_of_max_min_radius(experimental_point epa, experimental_point epb, experimental_point *epmax, experimental_point *epmin);

      bool common_vertex_on_foil(const sequence *seqB, double *the_distance) const;

    };

  }


}

#endif
