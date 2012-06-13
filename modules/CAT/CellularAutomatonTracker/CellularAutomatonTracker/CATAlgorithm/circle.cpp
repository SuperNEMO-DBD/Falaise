/* -*- mode: c++ -*- */

#include <CATAlgorithm/circle.h>

namespace CAT{
  namespace topology{

    using namespace std;
    using namespace mybhep;

    //!Default constructor 
    circle::circle(prlevel level, double nsigma)
    {
      appname_= "circle: ";
      center_ = experimental_point();
      radius_ = experimental_double(small_neg, small_neg);
      set_print_level(level);
      set_nsigma(nsigma);
    }

    //!Default destructor
    circle::~circle()
    {
      return;
    }

    //! constructor
    circle::circle(experimental_point center, experimental_double radius, prlevel level, double nsigma){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "circle: ";
      center_ = center;
      radius_ = radius;
    }

    /*** dump ***/
    void circle::dump (std::ostream & a_out,
                       const std::string & a_title,
                       const std::string & a_indent,
                       bool a_inherit) const{
      {
        string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << endl;
          }

        a_out << indent << appname_ << " -------------- " << endl;
        a_out << indent << " center " << endl;
        this->center().dump(a_out, "", indent + "    ");
        a_out << indent << " radius: "; radius().dump(); a_out << " " << endl;
        /*
          a_out << indent << " one round: " << endl;
          for(size_t i=0; i<100; i++){
          experimental_double theta(i*3.1417/100., 0.);
          a_out << indent << " .. theta " << theta.value()*180./M_PI << " x " << position(theta).x().value() << " , z " << position(theta).z().value() << endl;
          }
        */

        a_out << indent << " -------------- " << endl;

        return;
      }
    }



    //! set 
    void circle::set(experimental_point center, experimental_double radius)
    {
      center_ = center;
      radius_ = radius;
    }


    //! set center
    void circle::set_center(experimental_point center)
    {
      center_ = center;
    }

    //! set radius
    void circle::set_radius(experimental_double radius)
    {
      radius_ = radius;
    }

    //! get center
    const experimental_point& circle::center()const
    {
      return center_;
    }      

    //! get radius
    const experimental_double& circle::radius()const
    {
      return radius_;
    }      

    //! get curvature
    experimental_double circle::curvature()const
    {
      return 1./radius_;
    }      

    // get the phi of a point
    experimental_double circle::phi_of_point(experimental_point ep){

      experimental_double phi = experimental_vector(center_, ep).phi();

      if( phi.value() < 0. )
        phi.set_value(phi.value() + 2.*M_PI);

      return phi;

    }

    // get the position at parameter phi
    experimental_point circle::position(experimental_double phi){
      experimental_double deltax = experimental_cos(phi)*radius();
      experimental_double deltaz = experimental_sin(phi)*radius();

      return (experimental_vector(center()) + experimental_vector(deltax, center().y(), deltaz)).point_from_vector();
    }

    // get the position at the theta of point p
    experimental_point circle::position(experimental_point ep){
      return position(phi_of_point(ep));
    }

    // get the chi2 with point p
    double circle::chi2(experimental_point ep){
      experimental_vector residual(ep , position(phi_of_point(ep)));
      experimental_double r2 = residual.length2();

      return r2.value()/r2.error();
    }

    // get the chi2 with set of points
    double circle::chi2(std::vector<experimental_point> ps){

      double chi2 = 0.;
      for(std::vector<experimental_point>::iterator ip = ps.begin(); ip != ps.end(); ++ip){
        chi2 += experimental_vector(*ip , position(phi_of_point(*ip))).hor().length2().value();
      }

      return chi2;

    }

    void circle::best_fit_pitch(std::vector<experimental_point> ps, experimental_double *_pitch, experimental_double *_center){

      if( ps.size() == 0 ){
        clog << " problem: asking for best fit pitch for p vector of size " << ps.size() << endl;
        return;
      }
      

      // fit parameters y0, pitch with formula
      // phi(y) = (y - y0)/pitch

      double Sy2 = 0.;
      double Sy = 0.;
      double Sphi = 0.;
      double Syphi = 0.;
      double Sw = 0.;
      vector<experimental_double> phis;
      vector<experimental_double> ys;
      
      for(vector<experimental_point>::iterator ip=ps.begin(); ip!=ps.end(); ++ip){
        ys.push_back(ip->y());
        experimental_double phi = phi_of_point(*ip);
        phis.push_back(phi);
        double weight = 1/square(ip->y().error());
        weight = 1.;
        Sy += ip->y().value()*weight;
        Sy2 += square(ip->y().value())*weight;
        Sphi += phi.value()*weight;
        Syphi += ip->y().value()*phi.value()*weight;
        Sw += weight;
      }
      
      double det = 1./(Sy2*Sw - square(Sy));
      
      double one_over_pi = (Syphi*Sw - Sy*Sphi)/det;
      double min_ce_over_pi = average(phis).value() - average(ys).value()*one_over_pi;
      double one_over_pi_err = square(Sw)/(Sy2 - square(Sy));
      double min_ce_over_pi_err = one_over_pi_err*Sy2/Sw;
      double pi = 1./one_over_pi;
      double ce = - min_ce_over_pi/one_over_pi;
      double errpi = sqrt(one_over_pi_err)/square(one_over_pi);
      double errce = ce*sqrt(one_over_pi_err/square(one_over_pi) + min_ce_over_pi_err/square(min_ce_over_pi));

      *_pitch = experimental_double(pi, errpi);
      *_center = experimental_double(ce, errce);

      if( print_level() >= mybhep::VVERBOSE ){
        clog << " average y " << average(ys).value() << " average phi " << average(phis).value() << " 1/p " << one_over_pi << " -y0/p " << min_ce_over_pi << " center y " << ce << " pitch " << pi << " " << endl;
      
        for(vector<experimental_point>::iterator ip=ps.begin(); ip!=ps.end(); ++ip){
          experimental_double phi = phi_of_point(*ip);
          experimental_double predicted = *_center + *_pitch*phi;
          experimental_double res = predicted - ip->y();
          
          if( print_level() >= mybhep::VVERBOSE ){
            clog << " input y: ( "; 
            ip->y().dump(); 
            clog << " ) predicted: ("; 
            predicted.dump(); 
            clog << " ) local res: " ; 
            res.dump(); 
            clog << " phi: " ; 
            phi.dump(); 
            clog << " " << endl;
          }
          
          
        }
      }
      
      return;
      
    }

    bool circle::intersect_plane(plane pl, experimental_point * ep, experimental_double _phi){
      
      // normal vector from face of plane to center of circle
      experimental_vector ntp = pl.norm_to_point(center());
      
      double diff = (ntp.length() - radius()).value();

      if( diff > 0. ){
        *ep = (center() - pl.norm()*radius().value()).point_from_vector();
        return false;
      }
      
      if( diff == 0. ){
        *ep = (center() - pl.norm()*radius().value()).point_from_vector();
      }
      else{
        if( print_level() >= mybhep::VVERBOSE ){
          clog << " intersecting circle with center " << center().x().value() << " " << center().z().value() <<
            " with plain with face " << pl.face().x().value() << " " << pl.face().z().value();
        }
        
        if( pl.view() == "x" ){
          
          double sign = 1.;
          if( sin(_phi.value()) < 0. )
            sign = -1.;
          
          ep->set_x(pl.face().x());
          ep->set_z(center().z() + experimental_sqrt(experimental_square(radius()) -
                                                     experimental_square(ntp.length()))*sign);
          ep->set_y(center().y());
        }
        
        else if( pl.view() == "z" ){
          
          double sign = 1.;
          if( cos(_phi.value()) < 0. )
            sign = -1.;
          
          ep->set_z(pl.face().z());
          ep->set_x(center().x() + experimental_sqrt(experimental_square(radius()) -
                                                     experimental_square(ntp.length()))*sign);
          ep->set_y(center().y());
        }


      }

      experimental_vector dist = experimental_vector(pl.face(), *ep);
      if( print_level() >= mybhep::VVERBOSE ){
        clog << " distance from extrapolation to calo face: " << dist.x().value() << " " << dist.y().value() << " " << dist.z().value() << " calo sizes: " << pl.sizes().x().value() << " " << pl.sizes().y().value() << " " << pl.sizes().z().value() << endl;
      }
      if( pl.view() == "x" ){
        if( fabs(dist.z().value()) > pl.sizes().z().value() )
          return false;
        return true;
      }
      if( pl.view() == "z" ){
        if( fabs(dist.x().value()) > pl.sizes().x().value() )
          return false;
        return true;
      }

      
      if( print_level() >= mybhep::NORMAL )
        clog << " problem: intersecting circle with plane of view " << pl.view() << endl;

      return false;
      
    }


    bool circle::intersect_circle(circle c, experimental_point * ep, experimental_double _phi){
      
      // check that circles are intersecting
      experimental_double dist = experimental_vector(center(), c.center()).hor().length();
      experimental_double rsum = radius() + c.radius();

      if( rsum.value() < dist.value() ){
	if( print_level() >= mybhep::VVERBOSE ){
	  clog << " can't extrapolate circle to circle: the circles don't intesect " << endl;
	}
	return false;
      }


      // find middle point between circles
      // a^2 + h^2 = R0^2,   b^2 + h^2 = R1^2
      // so   
      // a^2 - R0^2 = b^2 - R1^2 =
      //            = a^2 + d^2 - 2ad - R1^2
      // hence
      // 2ad = R0^2 - R1^2 + d^2
      experimental_double a=(experimental_square(radius()) - experimental_square(c.radius()) + experimental_square(dist))/(dist*2.);
      experimental_point middle = (center() + (c.center() - center())*a/dist).point_from_vector();
      experimental_double h=experimental_sqrt(experimental_fabs(experimental_square(radius()) - square(a)));



      // find transverse axis
      experimental_vector forward_axis(center(), c.center());
      experimental_double fdistance = forward_axis.length();
      experimental_double fdistance_hor_ = forward_axis.hor().length();
      forward_axis /= fdistance.value();
      experimental_vector waxis(0.,1.,0.,0.,0.,0.);
      experimental_vector transverse_axis = (forward_axis ^ waxis).unit();


      // find two intersection point
      experimental_point p1=(middle + transverse_axis*h).point_from_vector();
      double phi1 = phi_of_point(p1).value();
      double dphi1 = fabs(phi1 - _phi.value());
      experimental_point p2=(middle - transverse_axis*h).point_from_vector();
      double phi2 = phi_of_point(p2).value();
      double dphi2 = fabs(phi2 - _phi.value());

      // pick closest to initial point of extrapolation
      if( dphi1 < dphi2 )
	*ep = p1;
      else
	*ep = p2;

      if( print_level() >= mybhep::VVERBOSE ){
	clog << " track: "; dump(); 
	clog << " foil : "; c.dump();
	clog << " intersection 1: "; p1.dump();		      
	clog << " intersection 2: "; p2.dump();		      
	clog << " dist "; dist.dump(); clog << " rsum "; rsum.dump(); clog << " a "; a.dump(); clog << " h "; h.dump(); clog << " middle "; middle.dump();
      }

      return true;
      
    }


    // average
    circle average (const std::vector<circle> vs)
    {
      circle mean;
    
      std::vector<experimental_double> radii;
      std::vector<experimental_point> centers;
      for(std::vector<circle>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        radii.push_back(iv->radius());
        centers.push_back(iv->center());
      }

      return circle(average(centers), average(radii));
    }


    // get circle through three points
    circle three_points_circle(experimental_point epa, experimental_point epb, experimental_point epc) {
    
      ////////////////////////////////////////////////////////////////////////
      //                                                                    //
      //  see http://local.wasp.uwa.edu.au/~pbourke/geometry/circlefrom3/   //
      //                                                                    //
      ////////////////////////////////////////////////////////////////////////
    
      experimental_double ma = experimental_vector(epa, epb).tan_phi();
      experimental_double mb = experimental_vector(epb, epc).tan_phi();
    
      experimental_double Xc = (ma*mb*(epa.z() - epc.z()) + mb*(epa.x() + epb.x()) - ma*(epb.x() + epc.x()))/((mb - ma)*2);
      experimental_double Zc;
      if( ma.value() != 0. )
        Zc = (epa.z() + epb.z())/2. - (Xc - (epa.x() + epb.x())/2.)/ma;
      else
        Zc = (epb.z() + epc.z())/2. - (Xc - (epb.x() + epc.x())/2.)/mb;
    
    
      experimental_double _radius = experimental_sqrt(experimental_square(Xc - epa.x()) + experimental_square(Zc - epa.z()));
    
      if( isnan(_radius.value()) )
        _radius.set_value(small_neg);
    
      experimental_double dist = epc.distance(epa);
    
      experimental_double deviation;
      if( dist.value() < 2.*_radius.value() )
        deviation = experimental_asin(dist/(_radius*2.))*2.;
      else
        deviation = experimental_asin(experimental_double(1.,0.))*2.;
    
      if( experimental_vector(epa, epb).phi().value() > experimental_vector(epb, epc).phi().value() )
        deviation.set_value(-deviation.value());

      experimental_point _center(Xc, experimental_double(0.,0.), Zc);
      circle h(_center,_radius);

      return h;
    
    }


    // get circle that best fits coordinates
    circle best_fit_circle(std::vector<experimental_double> xs, std::vector<experimental_double> zs){

      circle h;

      if( xs.size() == 0 ){
        clog << " problem: asking for best fit radius for x vector of size " << xs.size() << endl;
        return h;
      }

      if( zs.size() != xs.size() ){
        clog << " problem: asking for best fit radius for z vector of size " << zs.size() << " x vector of size " << xs.size() << endl;
        return h;
      }
    
      experimental_double xave = average(xs);
      experimental_double zave = average(zs);

      clog << " calculating best fit circle " << endl;
      clog << " xave "; xave.dump();
      clog << " zave "; zave.dump();


      std::vector<experimental_double> us;
      std::vector<experimental_double> vs;

      experimental_double Suu(0.,0.);
      experimental_double Suv(0.,0.);
      experimental_double Svv(0.,0.);
      experimental_double Suuu(0.,0.);
      experimental_double Suuv(0.,0.);
      experimental_double Suvv(0.,0.);
      experimental_double Svvv(0.,0.);
    
      for(vector<experimental_double>::iterator ix=xs.begin(); ix!=xs.end(); ++ix){
        experimental_double u = *ix - xave;
        experimental_double v = zs[ix - xs.begin()] - zave;
        clog << " .. x " ; ix->dump(); clog << " u "; u.dump(); clog << " z "; zs[ix - xs.begin()].dump(); clog << " v "; v.dump(); clog << " " << endl;
        us.push_back(u);
        vs.push_back(v);
        Suu += experimental_square(u);
        Svv += experimental_square(v);
        Suv += u*v;
        Suuu += experimental_cube(u);
        Suuv += experimental_square(u)*v;
        Suvv += u*experimental_square(v);
        Svvv += experimental_cube(v);
      }

      experimental_double det = Suu*Svv - square(Suv);
      experimental_double suma = (Suuu + Suvv)/2.;
      experimental_double sumb = (Svvv + Suuv)/2.;
      experimental_double sum = (Suu + Suv)/xs.size();

      experimental_double uc = (Svv*suma - Suv*sumb)/det;
      experimental_double vc = (-Suv*suma + Suu*sumb)/det;
      experimental_double xc = uc + xave;
      experimental_double zc = vc + zave;
      experimental_point center(xc, experimental_double(0.,0.), zc);
      experimental_double radius = experimental_sqrt( square(uc) + square(vc) + sum  );

      h = circle(center,radius);
      return h; 

    }

  }
}
