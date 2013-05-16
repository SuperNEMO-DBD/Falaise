#include <CATAlgorithm/cell.h>
#include <cmath>

namespace CAT{
namespace topology{

  using namespace mybhep;
  using namespace std;

  experimental_double cell::distance(cell c) const{

    experimental_vector v(ep(),c.ep());

    return v.length();

  }



  experimental_point cell::build_from_cell(experimental_vector forward, experimental_vector transverse, experimental_double cos, int sign, bool replace_r, double maxr ) const{

    experimental_double sin = experimental_sin(experimental_acos(cos))*sign;

    experimental_double radius = r();
    if( replace_r ){
      radius.set_value(maxr);
    }

    experimental_point p = (experimental_vector(ep()) + radius*(forward*cos + transverse*sin)).point_from_vector();

    return p;


  }


  experimental_point cell::angular_average(experimental_point epa, experimental_point epb, experimental_double* angle){

    if( print_level() >= mybhep::VVERBOSE )
      std::clog << " calculating angular average for cell " << id() << std::endl;

    if( small() ){
      angle->set_value(0.);
      angle->set_error(0.1); // fictitious value to avoid divergences
      /*
      experimental_double avx = (epa.x() + epb.x())/2.;
      experimental_double avy = (epa.y() + epb.y())/2.;
      experimental_double avz = (epa.z() + epb.z())/2.;
      experimental_point average(avx, avy, avz);
      return average;
      */
      return ep();
    }

    experimental_vector v1 = experimental_vector(ep(), epa);
    experimental_vector v2 = experimental_vector(ep(), epb);

    experimental_double phi1 = v1.phi();
    experimental_double phi2 = v2.phi();

    double rephi1 = phi1.value();
    double rephi2 = phi2.value();

    fix_angles(&rephi1, &rephi2);

    phi1.set_value(rephi1);
    phi2.set_value(rephi2);

    std::vector<experimental_double> phis;
    phis.push_back(phi1);
    phis.push_back(phi2);

    experimental_double ave_phi = weighted_average(phis);
    //    experimental_double ave_phi = (phi1 + phi2)/2.;
    *angle = phi1 - phi2;

    if( print_level() >= mybhep::VVERBOSE ){
      std::clog << " avereging phi1: "; (phi1*180./M_PI).dump();
      std::clog << " and phi2: "; (phi2*180./M_PI).dump();
      std::clog << " to phi_ave: "; (ave_phi*180./M_PI).dump();
      std::clog << " " << std::endl;
    }
    
    experimental_double cos_ave_phi = experimental_cos(ave_phi);
    
    experimental_vector x(1.,0.,0.,0.,0.,0.);
    experimental_vector z(0.,0.,1.,0.,0.,0.);
    int sign = 1; 
    if( ave_phi.value() < 0. ) sign = -1;  // ave_phi in [180,360], i.e. p to the left of cell center


    // distance of each point from center of cell
    // not necessarily the same if one of the 2 points results from intersecting cells
    // in such case, keep largest of 2 values to locate average
    // otherwise it will not make sense with the other point from intersecting cells
    double r1 = v1.hor().length().value(); 
    double r2 = v2.hor().length().value();
    bool replace_r = false;
    double maxr = 0.;
    if( r1 != r2 ){
      replace_r = true;
      maxr = max(r1,r2);
    }

    double errx = max(fabs( epb.x().value() - epa.x().value() ), ((epb+epa)/2.).x().error());
    double errz = max(fabs( epb.z().value() - epa.z().value() ), ((epb+epa)/2.).z().error());

    experimental_point p = build_from_cell(x, z, cos_ave_phi, sign, replace_r, maxr);
    p.set_ex(errx);
    p.set_ez(errz);
  
    return p;

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


  bool cell::same_quadrant(experimental_point epa, experimental_point epb)const{
    // check if the angular position of points epa and epb
    // is less than 90 degrees apart around the cell

    experimental_double initial_phi1 = experimental_vector(ep(), epa).phi();
    experimental_double initial_phi2 = experimental_vector(ep(), epb).phi();

    double re_initial_phi1 = initial_phi1.value();
    double re_initial_phi2 = initial_phi2.value();
    fix_angles(&re_initial_phi1, &re_initial_phi2);

    if( fabs(re_initial_phi1 - re_initial_phi2) > M_PI/2. ) return false;

    return true;

  }

  bool cell::same_cell(topology::cell c)const{

    if( block() == c.block() &&
        layer() == c.layer() &&
        iid() == c.iid() &&
        n3id() == c.n3id() )
      return true;

    return false;

  }

  bool cell::intersect(topology::cell c) const{

    double fraction_limit = 0.9; /// fraction of radius after which cells intersect

    double dist = experimental_vector(ep(), c.ep()).hor().length().value();
    experimental_double rsum = r() + c.r();

    if( rsum.value() > dist*fraction_limit ){
      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " cells " << id() << " and " << c.id() << " intersect: dist " << dist << " radii " << r().value() << " and " << c.r().value() << " rsum " << rsum.value() << std::endl;
      }
      return true;
    }

    bool test = false;

    /*
    double chi2 = square((rsum.value() - dist)/rsum.error());

    bool test = probof(chi2, 1) > probmin();
    if( test )
      if( print_level() >= mybhep::VVERBOSE ){
        std::clog <<" cells " << id() << " and " <<c.id() << " intersect: dist " << dist << " radii " << r().value() << " and " <<c.r().value()<< " rsum " << rsum.value() << " +- " << rsum.error() << " prob " << probof(chi2, 1) << " chi2 " << chi2 << " problimit " << probmin() << std::endl;
      }
    */

    return test;

  }


}
}

