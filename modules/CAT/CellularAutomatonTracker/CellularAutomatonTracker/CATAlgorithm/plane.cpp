/* -*- mode: c++ -*- */

#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/plane.h>


namespace CAT {
  namespace topology{

    using namespace std;
    using namespace mybhep;

    //!Default constructor
    plane::plane(prlevel level, double nsigma)
    {
      appname_= "plane: ";
      sizes_ = experimental_vector(small_neg,small_neg,small_neg,
                                   small_neg, small_neg, small_neg);
      norm_ = experimental_vector(small_neg,small_neg,small_neg,
                                  small_neg, small_neg, small_neg);
      set_print_level(level);
      set_nsigma(nsigma);
      type_="undefined";
    }

    //!Default destructor
    plane::~plane()
    {
      return;
    }

    //! constructor
    plane::plane(experimental_point center,
                 experimental_vector sizes,
                 experimental_vector norm,
                 prlevel level, double nsigma)
    {
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "plane: ";
      center_ = center;
      sizes_ = sizes;
      norm_ = norm;
      type_="undefined";
    }

    /*** dump ***/
    void plane::dump (ostream & a_out ,
                      const std::string & a_title,
                      const std::string & a_indent,
                      bool a_inherit)const{
      {
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " -------------- " << std::endl;
        a_out << indent << " type: " << this->type() << " view: " << this->view() << std::endl;
        a_out << indent << " center " << std::endl;
        this->center().dump(a_out, "", indent + "    ");
        a_out << indent << " sizes " << std::endl;
        this->sizes().dump(a_out, "", indent + "    ");
        a_out << indent << " -------------- " << std::endl;

        return;
      }
    }



    //! set
    void plane::set(const experimental_point &center, 
                    const experimental_vector &sizes, 
                    const experimental_vector &norm)
    {
      center_ = center;
      sizes_ = sizes;
      norm_ = norm;
    }


    //! set center
    void plane::set_center(const experimental_point &center)
    {
      center_ = center;
    }

    //! set sizes
    void plane::set_sizes(const experimental_vector &sizes)
    {
      sizes_ = sizes;
    }


    //! set norm
    void plane::set_norm(const experimental_vector &norm)
    {
      norm_ = norm;
    }


    //! set tyoe
    void plane::set_type(const std::string &type)
    {
      type_ = type;
    }


    //! get center
    const experimental_point& plane::center()const
    {
      return center_;
    }

    //! get sizes
    const experimental_vector& plane::sizes()const
    {
      return sizes_;
    }

    // returns the normal looking towards the origin
    const experimental_vector& plane::norm() const{
      return norm_;
    }

    // get type
    const string& plane::type() const{
      return type_;
    }

    std::string plane::view()const{
      if( type() == "SuperNEMO" ){
        if( norm().x().value() != 0 )
          return "x";

        if( norm().y().value() != 0 )
          return "y";

        return "z";
      }
      else if( type() == "Nemo3" ){
	if( norm().y().value() == 0. ){ // inner and outer walls
	  const experimental_point origin(0.,0.,0.,0.,0.,0.);
	  experimental_vector v(origin, center_);  // vector from center of detector
	        	        	                // to center of calo block
	  if( (v.hor()*norm()).value() > 0. )
	    return "inner";
	  return "outer";
	}

	if( norm().y().value() == 1. )
	  return "bottom";

	if( norm().y().value() == -1. )
	  return "top";

        clog << " warning: undefined view for plane of type " << type() << std::endl;
        return "null";
      }


      std::clog << " warning: undefined plane type: " << type() << " cannot determine view " << std::endl;
      exit(0);

      return "null";

    }


    //! get point of the front face of the block
    experimental_point plane::face()const
    {
      if( view() == "x" )
        return (center_ + norm()*sizes().x()/2.).point_from_vector();

      else if( view() == "y" )
        return (center_ + norm()*sizes().y()/2.).point_from_vector();

      else if( view() == "z" )
	return (center_ + norm()*sizes().z()/2.).point_from_vector();

      else{ // Nemo3

	return (center_ + norm()*sizes().x()/2.).point_from_vector();
      }

    }

    bool plane::intersect(const experimental_point &ep)const{
      experimental_vector dist = ep - face();

      if( view() == "x" ){

        if( fabs(dist.x().value()) > dist.x().error() )
          return false;

        if( fabs(dist.y().value()) > sizes().y().value()/2. + dist.y().error() )
          return false;

        if( fabs(dist.z().value()) > sizes().z().value()/2. + dist.z().error() )
          return false;

        return true;
      }
      else if( view() == "y" ){

        if( fabs(dist.y().value()) > dist.y().error() )
          return false;

        if( fabs(dist.x().value()) > sizes().x().value()/2. + dist.x().error() )
          return false;

        if( fabs(dist.z().value()) > sizes().z().value()/2. + dist.z().error() )
          return false;

        return true;
      }

      else if( view() == "z" ){
	if( fabs(dist.z().value()) > dist.z().error() )
	  return false;
	
	if( fabs(dist.x().value()) > sizes().x().value()/2. + dist.x().error() )
	  return false;
	
	if( fabs(dist.y().value()) > sizes().y().value()/2. + dist.y().error() )
	  return false;

	return true;
	
      }
      else{
	experimental_double dist_norm = dist*norm(); // distance along the normal
	experimental_vector pv=dist^norm();
	experimental_double dist_transv = pv.hor().length(); // transverse distance
	experimental_double dist_vert = pv.y(); // vertical distance

        if( fabs(dist_norm.value()) > fabs( dist_norm.error()))
          return false;

	if( fabs(dist_transv.value()) > sizes().z().value()/2. + dist_transv.error())
          return false;

	if( fabs(dist_vert.value()) > sizes().y().value()/2. + dist_vert.error())
          return false;

	return true;
      }
      
    }

    bool plane::intersect(const experimental_point &start, const experimental_vector &direction, experimental_point* ep)const{

      if( view() == "x" ){
        if( direction.x().value() == 0 ){
          return false;
        }

        experimental_double time = (face().x() - start.x())/direction.x();

        *ep = (experimental_vector(start) + time*direction).point_from_vector();

        return intersect(*ep);
      }
      else if( view() == "y" ){
        if( direction.y().value() == 0 ){
          return false;
        }

        experimental_double time = (face().y() - start.y())/direction.y();

        *ep = (experimental_vector(start) + time*direction).point_from_vector();

        return intersect(*ep);
      }

      else if( view() == "z" ){
	if( direction.z().value() == 0 ){
	  return false;
	}
	
	experimental_double time = (face().z() - start.z())/direction.z();
	
	*ep = (experimental_vector(start) + time*direction).point_from_vector();
	
	return intersect(*ep);
      }
      else{ // Nemo3
	if( (direction*norm()).value() == 0 ) return false;

	experimental_double time = (face() - start)*norm()/(direction*norm());
	
	*ep = (experimental_vector(start) + time*direction).point_from_vector();

	return intersect(*ep);
      }



    }


    // normal vector from the face of the plane to the parallel plane through the point
    experimental_vector plane::norm_to_point(const experimental_point &ep)const{

      experimental_vector result = norm();
      return ((ep - face())*result)*result;

    }

  }
}
