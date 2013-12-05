#include <sultan/cell.h>
#include <cmath>

namespace SULTAN{
  namespace topology{
    
    using namespace mybhep;
    using namespace std;
    
    experimental_double cell::distance(cell c) const{
      
      experimental_vector v(ep(),c.ep());
      
      return v.length();
      
    }


    void cell::dump_point(experimental_point epp)const{
      
      experimental_vector v(ep(), epp);
      
      std::clog << " x: "; (v.x()).dump();
      std::clog << " y: "; (v.y()).dump();
      std::clog << " z: "; (v.z()).dump();
      std::clog << " phi: "; (v.phi()*180./M_PI).dump();
      
    }


    void cell::dump_point_phi(experimental_point epp)const{
      
      experimental_vector v(ep(), epp);
      std::clog << " phi: "; (v.phi()*180./M_PI).dump();
      
    }


    bool cell::same_cell(topology::cell c)const{
      
      if( block() == c.block() &&
	  layer() == c.layer() &&
	  iid() == c.iid() &&
	  n3id() == c.n3id() )
	return true;
      
      return false;
      
    }


    size_t cell::near_level( const topology::cell & cell, double nofflayers, double cell_distance )const{

      // returns 0 for far-away cell
      // 1 for cells separated by nofflayers
      // 2 for diagonal cells
      // 3 for side-by-side cells

      // side-by-side connection: distance = 1
      // diagonal connection: distance = sqrt(2) = 1.41
      // skip 1 connection, side: distance = 2
      // skip 1 connection, tilt: distance = sqrt(5) = 2.24
      // skip 1 connection, diag: distance = 2 sqrt(2) = 2.83

      topology::experimental_double distance = topology::experimental_vector(this->ep(),cell.ep()).hor().length();

      double limit_side = cell_distance;
      double limit_diagonal = sqrt(2.)*cell_distance;
      double precision = 0.15*limit_side;


      if( print_level() >= mybhep::VVERBOSE )
	std::clog << " (c " << this->id() << " d " << distance.value() << " )";

      if( fabs(distance.value() - limit_side) < precision )
	return 3;

      if( fabs(distance.value() - limit_diagonal) < precision )
	return 2;

      if( distance.value() < limit_diagonal*(1. + nofflayers) )
	return 1;

      return 0;


    }


  }
}

