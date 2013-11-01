/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__SSEQUENCE
#define __CATAlgorithm__SSEQUENCE
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
#include <CATAlgorithm/sequence.h>
#include <CATAlgorithm/Cell.h>


namespace CAT {
  namespace topology{

    class Sequence : public sequence {

      // a sequence is composed of a list of nodes

    protected:
      std::string appname_;

      size_t track_id_;

    public:

      // list of nodes
      std::vector<Cell> cells_;

      //!Default constructor
      Sequence(){};

      //!Default destructor
      virtual ~Sequence(){};

      //! set Cells
      void set_cells(const std::vector<Cell> &a){cells_=a;};

      //! get Cells
      const std::vector<Cell> & cells()const{return cells_;};

      void set_track_id(size_t a){track_id_ = a;};

      const size_t track_id()const{return track_id_;};

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const{

	std::string indent;
	if (! a_indent.empty ()) indent = a_indent;
	if (! a_title.empty ())
	  {
	    a_out << indent << a_title << std::endl;
	  }
	
	a_out << indent << " Sequence ------------------- " << this->track_id() << " cells " << cells_.size() << std::endl;
	for(std::vector<Cell>::const_iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
	  ic->dump();
	
      }

    };

  }


}

#endif
