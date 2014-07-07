/* -*- mode: c++ -*- */
#ifndef __sultan__ICLUSTER
#define __sultan__ICLUSTER
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/utilities.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>
#include <sultan/node.h>
#include <sultan/experimental_helix.h>
#include <sultan/LinearRegression.h>


namespace SULTAN{
  namespace topology{

    class cluster : public tracking_object{

      // a cluster is composed of a list of nodes

    protected:
      std::string appname_;

    public:

      // list of nodes
      std::vector<node> nodes_;

      // cluster type: "neighbouring_cells", "broken_line", "straight_line", "helix"
      std::string cluster_type_;

      // fitted helix
      experimental_helix helix_;

      // vector of pieces: length, first cell
      std::vector<size_t> first_cell_of_piece_;
      std::vector<size_t> length_of_piece_;

      //!Default constructor
      cluster();

      //!Default destructor
      virtual ~cluster();

      //! constructor from std::vector of nodes
      cluster(const std::vector<node> &nodes, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const ;
      //! set nodes
      void set_nodes(const std::vector<node> &nodes);

      //! get nodes
      const std::vector<node> & nodes()const;

      //! add nodes
      void add_nodes(const std::vector<node> &nodes);

      //! remove node
      void remove_node_with_id(size_t id);

      //! remove nodes
      void remove_nodes(const std::vector<node> &nodes);

      //! set cluster type
      void set_cluster_type(std::string a){
	cluster_type_ = a;
      }

      //! get cluster type
      std::string get_cluster_type()const{
	return cluster_type_;
      }

      //! set helix
      void set_helix(const experimental_helix & helix);

     //! get helix
      const experimental_helix & get_helix()const;

      std::vector<size_t> first_cell_of_piece(size_t nofflayers, double cell_distance);

      std::vector<size_t> length_of_piece(){ return length_of_piece_;}

      std::vector<size_t> first_cell_of_piece(){ return first_cell_of_piece_;}

    public:

      bool has_cell(const cell & c)const;

      cluster invert();

      void circle_order();

      topology::node node_of_cell(const topology::cell & c);

      bool is_good()const;

      bool is_contained_in(const cluster & s)const;

      bool contains(const cluster & s)const;

      topology::cluster get_cluster_with_first_last(size_t first, size_t last);

      topology::cluster remove_cluster_with_first_last(size_t first, size_t last);

      size_t get_next_index(size_t index);

      topology::cluster longest_piece();

      void break_into_continous_pieces(size_t nofflayers, double cell_distance);

      void self_order(topology::node * n);

      bool is_continous();

      size_t max_length_of_piece();

      void recalculate(size_t n_iterations);

      void recalculate_x0();
      void recalculate_y0();
      void recalculate_z0();
      void recalculate_R();
      void recalculate_H();

    };

  }
}

#endif
