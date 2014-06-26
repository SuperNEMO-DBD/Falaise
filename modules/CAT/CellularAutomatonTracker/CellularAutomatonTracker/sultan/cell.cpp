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


   bool cell::intersect(cell c) const{


     // fraction of radius after which cells intersect
     double fraction_limit = 1.;

     // horizontal distance between cell centers
     double dist = experimental_vector(ep(), c.ep()).hor().length().value();

     // sum if radii
     double rsum = (r() + c.r()).value();

     return (rsum >= dist*fraction_limit );

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


    size_t cell::near_level( const topology::cell & cell, double nofflayers, double /*cell_distance*/ )const{

      // returns 0 for far-away cell
      // 1 for cells separated by nofflayers
      // 2 for diagonal cells
      // 3 for side-by-side cells

      // side-by-side connection: distance = 1
      // diagonal connection: distance = sqrt(2) = 1.41
      // skip 1 connection, side: distance = 2
      // skip 1 connection, tilt: distance = sqrt(5) = 2.24
      // skip 1 connection, diag: distance = 2 sqrt(2) = 2.83

#if 1  // use side, layer and row


      // Use geiger locator for such research Warning: use integer
      // because uint32_t has strange behavior with absolute value
      // cmath::abs
      const int hit1_side  = this->block();  // -1, 1
      const int hit1_layer = abs(this->layer()); // 0, 1, ..., 8
      const int hit1_row   = this->iid();  // -56, -55, ..., 55, 56

      const int hit2_side  = cell.block();
      const int hit2_layer = abs(cell.layer());
      const int hit2_row   = cell.iid();

      // Do not cross the foil
      if (hit1_side != hit2_side) return 0;

      // Check neighboring
      const unsigned int layer_distance = abs (hit1_layer - hit2_layer); // 1 --> side-by-side
      const unsigned int row_distance = abs (hit1_row - hit2_row);

      if (layer_distance == 0 && row_distance == 0){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: sultan asking near level of cells with identical position (" << hit1_side << ", " << hit1_layer << ", " << hit1_row << ") (" << hit2_side << ", " << hit2_layer << ", " << hit2_row << ")" << std::endl;
        }
        return 3;
      }
      else if (layer_distance == 1 && row_distance == 0) return 3;
      else if (layer_distance == 0 && row_distance == 1) return 3;
      else if (layer_distance == 1 && row_distance == 1) return 2;
      else if (layer_distance > nofflayers + 1 || row_distance > nofflayers + 1) return 0;
      return 1;

#else // use physical distance
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
#endif


    }

    bool cell::is_near_foil()const{
      // blocks = -1, 1
      // layers = 0, +-1, ..., +-8
      // rows = -56, -55, ..., 55, 56

      return (this->layer()==0) ? true : false;

    }

    bool cell::is_near_calo()const{
      // blocks = -1, 1
      // layers = 0, +-1, ..., +-8
      // rows = -56, -55, ..., 55, 56

      return (abs(this->layer())==8) ? true : false;
    }


    bool cell::is_near_xcalo()const{
      // blocks = -1, 1
      // layers = 0, +-1, ..., +-8
      // rows = -56, -55, ..., 55, 56

      return (abs(this->iid())==56) ? true : false;
    }


    bool cell::is_near_gveto()const{

      return true;
    }


  }
}
