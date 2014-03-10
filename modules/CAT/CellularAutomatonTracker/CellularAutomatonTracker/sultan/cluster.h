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


namespace SULTAN{
  namespace topology{


    class cluster : public tracking_object{

      // a cluster is composed of a list of nodes

    protected:
      std::string appname_;

    public:

      // list of nodes
      std::vector<node> nodes_;

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

    public:

      bool has_cell(const cell & c)const;

      cluster invert();

      topology::node node_of_cell(const topology::cell & c);

      bool is_good()const;

      bool is_contained_in(const cluster & s)const;

      bool contains(const cluster & s)const;

    };

  }
}

#endif
