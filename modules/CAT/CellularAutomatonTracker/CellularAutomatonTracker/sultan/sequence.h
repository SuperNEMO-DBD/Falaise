/* -*- mode: c++ -*- */
#ifndef __sultan__ISEQUENCE
#define __sultan__ISEQUENCE
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/utilities.h>
#include <boost/cstdint.hpp>

#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>
#include <sultan/node.h>
#include <sultan/cluster.h>


namespace SULTAN {
  namespace topology{

    class sequence : public tracking_object{

      // a sequence is composed of a list of nodes

    protected:
      std::string appname_;

      // fitted helix
      experimental_helix helix_;

      experimental_vector helix_momentum_;

    public:

      // list of nodes
      std::vector<node> nodes_;

      // sequence's name
      std::string name_;

      // calo id
      size_t calo_id_;

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

      //! set name
      void set_name(const std::string & name);

      //! set helix
      void set_helix(const experimental_helix & helix);

      //! set helix_momentum
      void set_helix_momentum(const experimental_vector &mom);

      //! set calo_id
      void set_calo_id(const size_t id){
	calo_id_ = id;
      }

      //! get nodes
      const std::vector<node> & nodes()const;

      //! get name
      const std::string & name()const;

      //! get helix
      const experimental_helix & get_helix()const;

      //! get helix_momentum
      const experimental_vector & helix_momentum() const;

      //! get calo id
      size_t calo_id()const;

    public:

      bool has_cell(const cell & c)const;

      bool is_contained_in(const sequence & s)const;

      bool contains(const sequence & s)const;

      experimental_vector initial_helix_dir(bool SuperNEMO, double ref_value)const;

      void calculate_momentum(double bfield, bool SuperNEMO, double ref_value);

    };

  }


}

#endif
