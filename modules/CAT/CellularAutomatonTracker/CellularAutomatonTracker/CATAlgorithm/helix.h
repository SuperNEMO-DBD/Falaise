/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IHELIX
#define __CATAlgorithm__IHELIX
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/circle_base.h>

namespace CAT {
  namespace topology{


    class helix : public tracking_object {

      // a helix is identified by origin,
      // radius
      // and pitch

    private:
      std::string appname_;

      // experimental point
      experimental_point center_;

      // radius
      experimental_double radius_;

      // pitch
      experimental_double pitch_;


      // points in the helix are given by:
      // x(phi) = center_.x() + radius*cos(phi)
      // y(phi) = center_.y() + pitch*phi
      // z(phi) = center_.z() + radius*sin(phi)


    public:

      //!Default constructor
      helix(mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200)
      {
        appname_= "helix: ";
        center_ = experimental_point();
        radius_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        pitch_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        set_print_level(level);
        set_probmin(probmin);
      }

      //!Default destructor
      virtual ~helix(){};

      //! constructor
      helix(const experimental_point &center, const experimental_double &radius, const experimental_double &pitch, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200){
        set_print_level(level);
        set_probmin(probmin);
        appname_= "helix: ";
        center_ = center;
        radius_ = radius;
        pitch_ = pitch;
      }

      //! constructor from a circle and a pitch
      helix(const circle &c, const experimental_double  &pitch, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200){
        set_print_level(level);
        set_probmin(probmin);
        appname_= "helix: ";
        center_ = c.center();
        radius_ = c.radius();
        pitch_ = pitch;
      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool /*a_inherit */         = false)const{
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }

          a_out << indent << appname_ << " -------------- " << std::endl;
          a_out << indent << " center " << std::endl;
          this->center().dump(a_out, "", indent + "    ");
          a_out << indent << " radius: "; radius().dump(); a_out << " " << std::endl;
          a_out << indent << " pitch: "; pitch().dump(); a_out << " " << std::endl;
          a_out << indent << " one round: " << std::endl;
          experimental_double theta(0.,0.);
          for(size_t i=0; i<100; i++){
            theta.set_value(i*3.1417/100.);
            a_out << indent << " .. theta " << theta.value()*180./M_PI << " x " << position(theta).x().value() << " , z " << position(theta).z().value() << std::endl;
          }
          a_out << indent << " -------------- " << std::endl;

          return;
        }
      }



      //! set
      void set(const experimental_point &center, const experimental_double &radius, const experimental_double &pitch)
      {
        center_ = center;
        radius_ = radius;
        pitch_ = pitch;
      }


      //! set center
      void set_center(const experimental_point &center)
      {
        center_ = center;
      }

      //! set radius
      void set_radius(const experimental_double &radius)
      {
        radius_ = radius;
      }

      //! set pitch
      void set_pitch(const experimental_double &pitch)
      {
        pitch_ = pitch;
      }


      //! get center
      const experimental_point& center()const
      {
        return center_;
      }

      //! get radius
      const experimental_double& radius()const
      {
        return radius_;
      }

      //! get pitch
      const experimental_double& pitch()const
      {
        return pitch_;
      }

      //! get curvature
      experimental_double curvature()const
      {
        return 1./radius_;
      }

      //! get circle
      circle get_circle()const
      {
        return circle(center(), radius(), print_level(), probmin());
      }

      // get the phi of a point
      experimental_double phi_of_point(const experimental_point &ep)const{

        return phi_of_point(ep,0.);
      }

      experimental_double phi_of_point(const experimental_point &ep, double phi_ref )const{
        // if no ref is given, phi is in [-pi, pi]
        // if ref is given is in [ref - \pi, ref + \pi]

        experimental_double phi = experimental_vector(center_, ep).phi();

        while( phi.value() - phi_ref > M_PI ){
          phi.set_value(phi.value() - 2.*M_PI);
        }

        while( phi.value() - phi_ref < - M_PI ){
          phi.set_value(phi.value() + 2.*M_PI);
        }

        return phi;

      }

      // get delta-phi of two points
      experimental_double delta_phi(const experimental_point &epa, const experimental_point &epb)const{

        experimental_double phi1 = experimental_vector(center_, epa).phi();
        experimental_double phi2 = experimental_vector(center_, epb).phi();

        experimental_double dp = phi2 - phi1;

        double vphi1 = phi1.value();
        double vphi2 = phi2.value();

        mybhep::fix_angles(&vphi1, &vphi2);

        dp.set_value(vphi2 - vphi1);
        return dp;

      }

      // get the position at parameter phi
      experimental_point position(const experimental_double &phi)const{

        experimental_double deltax = experimental_cos(phi)*radius();
        experimental_double deltay = phi*pitch();
        experimental_double deltaz = experimental_sin(phi)*radius();

        return (experimental_vector(center()) + experimental_vector(deltax, deltay, deltaz)).point_from_vector();
      }

      // get the position at the theta of point p
      experimental_point position(const experimental_point &ep)const{
        return position(ep,0.);

      }

      experimental_point position(const experimental_point &ep, double phi_ref )const{
        return position(phi_of_point(ep, phi_ref));
      }

      experimental_point position(const experimental_point &ep, double phi_ref , double y_ref)const{
        experimental_point pos = position(ep, phi_ref);

        double one_turn=pitch().value()*2.*M_PI;
        int n_extra_turns = (int)((pos.y().value() - y_ref)/one_turn);

	double newy = pos.y().value() - n_extra_turns*one_turn;
	double dist = std::abs(newy - y_ref);
	double newyup = newy + one_turn;
	double distup = std::abs(newyup - y_ref);
	double newydown = newy - one_turn;
	double distdown = std::abs(newydown - y_ref);
	if( distup < dist && distup < distdown )
	  newy = newyup;
	if( distdown < dist && distdown < distup )
	  newy = newydown;

        pos.set_y(experimental_double(newy, pos.y().error()));
        return pos;
      }

      // get the chi2 with point p
      double chi2(const experimental_point &ep)const{
        return chi2(ep,0.);
      }

      double chi2(const experimental_point &ep, double phi_ref )const{
        experimental_point predicted = position(phi_of_point(ep, phi_ref));
        experimental_vector residual(ep , predicted);
	/*
        double res2 = mybhep::square(residual.x().value()/residual.x().error()) +
          mybhep::square(residual.y().value()/residual.y().error()) +
          mybhep::square(residual.z().value()/residual.z().error());
	*/
	// the helix errors are based on fluctuation of points from helix curve
	// so they cannot be used for chi2 (they are large for a bad helix giving it a good chi2)
	// use just error on point
	if( ep.x().error() ) residual.set_x(residual.x()/ep.x().error());
	if( ep.y().error() ) residual.set_y(residual.y()/ep.y().error());
	if( ep.z().error() ) residual.set_z(residual.z()/ep.z().error());

        double res2 = mybhep::square(residual.x().value()) +
          mybhep::square(residual.y().value()) +
          mybhep::square(residual.z().value());

        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " input point: ( "; ep.x().dump(); std::clog << " , "; ep.y().dump(); std::clog << " , "; ep.z().dump(); std::clog << " ) helix: ("; predicted.x().dump(); std::clog << " , "; predicted.y().dump(); std::clog << " , ";predicted.z().dump(); std::clog << " ) local chi2: " << res2  << " " << std::endl;
        }

        return res2;
      }

      // compare with set of points and get total chi2
      double chi2(std::vector<experimental_point> &ps)const{

        double _chi2 = 0.;
        double phi_ref = 0.;
        double phi_ref2 = 0.;
        // size_t index=0;
        for(std::vector<experimental_point>::const_iterator ip = ps.begin(); ip != ps.end(); ++ip){

          _chi2 += chi2(*ip, phi_ref);

          phi_ref2 = phi_ref;
          phi_ref = phi_of_point(*ip, phi_ref2).value();

        }

        return _chi2;

      }

      // compare with set of points and get set of chi2
      std::vector<double> chi2s(std::vector<experimental_point> & ps)const{

        std::vector<double> _chi2s;
        double phi_ref = 0.;
        double phi_ref2 = 0.;
        //size_t index=0;
        for(std::vector<experimental_point>::const_iterator ip = ps.begin(); ip != ps.end(); ++ip){

          _chi2s.push_back(chi2(*ip, phi_ref));

          phi_ref2 = phi_ref;
          phi_ref = phi_of_point(*ip, phi_ref2).value();

        }

        return _chi2s;

      }


      bool intersect_plane(const plane &pl, experimental_point * ep, const experimental_double &_phi, const double y_ref)const{

        if( pl.view() == "x" || pl.view() == "z" || pl.view() == "inner" || pl.view() == "outer" ){
          bool result = get_circle().intersect_plane(pl, ep, _phi);
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " helix intersected point y " << ep->y().value() << " _phi " << _phi.value() << " y_ref " << y_ref << " -> y " << position(*ep,_phi.value(), y_ref).y().value() << std::endl;
	  }
          ep->set_y(position(*ep,_phi.value(), y_ref).y());

          if( std::isnan(ep->x().value())  || std::isnan(ep->y().value()) || std::isnan(ep->z().value()) ) return false;

          return result;

        }

        if( pl.view() == "y" || pl.view() == "top" || pl.view() == "bottom" ){

          if( pitch().value() == 0.)
            return false;

          *ep = position((pl.face().y() - center().y())/pitch());


          // vector from center of plane face to extrapolated point
          experimental_vector dist = experimental_vector(pl.face(), *ep).hor();
          if( print_level() >= mybhep::VVERBOSE ){
            std::clog << " helix distance from extrapolation to plane face: " << dist.x().value() << ", " << dist.y().value() << ", " << dist.z().value() << " plane sizes: " << pl.sizes().x().value() << " " << pl.sizes().y().value() << " " << pl.sizes().z().value() << std::endl;
          }

          if( std::abs(dist.x().value()) > pl.sizes().x().value()/2. )
            return false;
          if( std::abs(dist.z().value()) > pl.sizes().z().value()/2. )
            return false;

          ep->set_y(pl.face().y());

          if( std::isnan(ep->x().value())  || std::isnan(ep->y().value()) || std::isnan(ep->z().value()) ) return false;

          return true;
        }


        std::clog << " problem: cannot intersect helix with plane of view " << pl.view() << std::endl;
        return false;

      }

      bool intersect_circle(const circle &c, experimental_point * ep, const experimental_double &_phi)const{

        bool result = get_circle().intersect_circle(c, ep, _phi);
        ep->set_y(position(*ep, _phi.value()).y());

        return result;

      }

      bool intersect_circle_with_tangent(const circle &c, const experimental_point &start, const experimental_vector &direction, experimental_point* ep)const{


        experimental_vector center_to_start(c.center(),start);
        center_to_start = center_to_start.hor();

        experimental_vector rotation=(direction.unit())^(center_to_start.unit());

        experimental_double beta=experimental_asin(rotation.y());

        //      experimental_double angle_at_center=center_to_start.kink_phi(direction)/2.;
        experimental_double phi0=c.phi_of_point(start);
	experimental_double angle = center_to_start.length()/c.radius()*rotation.y();
	if( angle.value() > 1. ) angle.set_value(1.);
	if( angle.value() < -1. ) angle.set_value(-1.);
        experimental_double alpha=experimental_double(asin(1.),0.)-beta-experimental_acos(angle);

        experimental_double sign(1.,0.);  // if the track is in the outer part of the foil
        if( (center_to_start*direction).value() > 0 ) // the track in in the inner part of the foil
          sign.set_value(-1.);

        //      *ep = c.position(phi0 + angle_at_center);
        *ep = c.position(phi0 + sign*alpha);
        ep->set_y(position(*ep, phi0.value(),start.y().value()).y());

        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " center (" << c.center().x().value() << ", " << c.center().y().value() << ", " << c.center().z().value() << ") r " << c.radius().value() << " +- " << c.radius().error() << " start (" << start.x().value() << ", " << start.y().value() << ", " << start.z().value() << ") cts (" << center_to_start.x().value() << ", " << center_to_start.y().value() << ", " << center_to_start.z().value() << ") length " << center_to_start.length().value() << " +- " << center_to_start.length().error() <<  " beta " << beta.value()*180./acos(-1.) << " +- " << beta.error()*180./acos(-1.) << " phi0 " << phi0.value()*180./acos(-1.) << " angle " << angle.value() << " +- " << angle.error() << " alpha " << alpha.value()*180./acos(-1.) << " sign " << sign.value() << " newangle " << (phi0+sign*alpha).value()*180./acos(-1.) << " pos: (" << ep->x().value() << ", " << ep->y().value() << ", " << ep->z().value() << ") " << std::endl;
        }

        if( std::isnan(ep->x().value())  || std::isnan(ep->y().value()) || std::isnan(ep->z().value()) ) return false;

        return true;
      }


      helix invert(){
        helix inverted;
        inverted.set_print_level(print_level());
        inverted.set_probmin(probmin());
        inverted.set_center(center());
        inverted.set_radius(radius());
        inverted.set_pitch(pitch());
        return inverted;
      }

      void point_of_max_min_radius(experimental_point epa, experimental_point epb, experimental_point *epmax, experimental_point *epmin){

        get_circle().point_of_max_min_radius(epa, epb, epmax, epmin);

      return;

      }

      experimental_vector direction_at(const experimental_point & ep)const{

	const experimental_double phi = phi_of_point(ep);

	experimental_vector dir(-radius() * experimental_sin(phi),
				pitch(),
				radius() * experimental_cos(phi));

	return dir.unit();

      }


    };

    // average
    inline helix average (const std::vector<helix> &vs)
    {
      helix mean;

      std::vector<experimental_double> radii;
      std::vector<experimental_double> pitches;
      std::vector<experimental_point> centers;
      for(std::vector<helix>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        radii.push_back(iv->radius());
        pitches.push_back(iv->pitch());
        centers.push_back(iv->center());
      }

      return helix(average(centers), average(radii), average(pitches));
    }


    // get helix through three points
    inline helix three_points_helix(const experimental_point &epa, const experimental_point &epb, const experimental_point &epc){

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

      if( std::isnan(_radius.value()) || std::isinf(_radius.value()) )
        _radius.set_value(mybhep::small_neg);

      experimental_double dist = epc.distance(epa);

      experimental_double deviation;
      if( dist.value() < 2.*_radius.value() )
        deviation = experimental_asin(dist/(_radius*2.))*2.;
      else
        deviation = experimental_asin(experimental_double(1.,0.))*2.;

      if( experimental_vector(epa, epb).phi().value() > experimental_vector(epb, epc).phi().value() )
        deviation.set_value(-deviation.value());

      experimental_double _pitch = (epc.y() - epa.y())/deviation;

      experimental_point _center(Xc, experimental_double(0.,0.), Zc);
      experimental_double Yc = epb.y() - _pitch*experimental_vector(_center, epb).phi();
      _center.set_y(Yc);

      helix h(_center,_radius,_pitch);

      return h;

    }

  }

}

#endif
