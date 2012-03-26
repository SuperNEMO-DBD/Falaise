/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IPLANE
#define __CATAlgorithm__IPLANE
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>


namespace CAT {
namespace topology{

using namespace std;
using namespace mybhep;

  class plane : public tracking_object {

    // a plane is identified by one point (center)
    // the plane sizes
    // and a normal to the plane

  private:
    string appname_;

    // experimental point
    experimental_point center_; 

    // sizes
    experimental_vector sizes_;

    // normal looking towards the origin
    experimental_vector norm_;

  public:   
    //!Default constructor 
    plane(prlevel level=mybhep::NORMAL, double nsigma=10.)
    {
      appname_= "plane: ";
      center_ = experimental_point();
      sizes_ = experimental_vector(small_neg,small_neg,small_neg,
                                   small_neg, small_neg, small_neg);
      norm_ = experimental_vector(small_neg,small_neg,small_neg,
                                   small_neg, small_neg, small_neg);
      set_print_level(level);
      set_nsigma(nsigma);
    }

    //!Default destructor
    virtual ~plane(){};

    //! constructor
    plane(experimental_point center, experimental_vector sizes, experimental_vector norm, prlevel level=mybhep::NORMAL, double nsigma=10.){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "plane: ";
      center_ = center;
      sizes_ = sizes;
      norm_ = norm;
    }

    /*** dump ***/
    virtual void dump (ostream & a_out         = clog,
                            const string & a_title  = "",
                            const string & a_indent = "",
                            bool a_inherit          = false)const{
      {
        string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << endl;
          }

        a_out << indent << appname_ << " -------------- " << endl;
        a_out << indent << " view: " << this->view() << endl;
        a_out << indent << " center " << endl;
        this->center().dump(a_out, "", indent + "    ");
        a_out << indent << " sizes " << endl;
        this->sizes().dump(a_out, "", indent + "    ");
        a_out << indent << " -------------- " << endl;

        return;
      }
    }



    //! set 
    void set(experimental_point center, experimental_vector sizes, experimental_vector norm)
      {
        center_ = center;
        sizes_ = sizes;
        norm_ = norm;
      }


    //! set center
    void set_center(experimental_point center)
      {
        center_ = center;
      }

    //! set sizes
    void set_sizes(experimental_vector sizes)
      {
        sizes_ = sizes;
      }


    //! set norm
    void set_norm(experimental_vector norm)
      {
        norm_ = norm;
      }


    //! get center
    const experimental_point& center()const
    {
      return center_;
    }      

    //! get sizes
    const experimental_vector& sizes()const
    {
      return sizes_;
    }      

    // returns the normal looking towards the origin
    const experimental_vector& norm() const{
      return norm_;
    }

    string view()const{
      if( norm().x().value() != 0 )
        return "x";

      if( norm().y().value() != 0 )
        return "y";

      return "z";

    }
    

    //! get point of the face of the plane
    experimental_point face()const
    {
      if( view() == "x" )
        return (center_ + norm()*sizes().x()/2.).point_from_vector();

      else if( view() == "y" )
        return (center_ + norm()*sizes().y()/2.).point_from_vector();

      return (center_ + norm()*sizes().z()/2.).point_from_vector();

    }      

    bool intersect(experimental_point ep)const{
      experimental_vector dist = ep - center();

      if( view() == "x" ){

        if( fabs(dist.x().value()) > dist.x().error() )
          return false;

        if( fabs(dist.y().value()) > sizes().y().value() + dist.y().error() )
          return false;

        if( fabs(dist.z().value()) > sizes().z().value() + dist.z().error() )
          return false;

        return true;
      }
      else if( view() == "y" ){

        if( fabs(dist.y().value()) > dist.y().error() )
          return false;

        if( fabs(dist.x().value()) > sizes().x().value() + dist.x().error() )
          return false;

        if( fabs(dist.z().value()) > sizes().z().value() + dist.z().error() )
          return false;

        return true;
      }

      if( fabs(dist.z().value()) > dist.z().error() )
        return false;
      
      if( fabs(dist.x().value()) > sizes().x().value() + dist.x().error() )
        return false;
      
      if( fabs(dist.y().value()) > sizes().y().value() + dist.y().error() )
        return false;
      
      return true;

    }

    bool intersect(experimental_point start, experimental_vector direction, experimental_point* ep)const{

      if( view() == "x" ){
        if( direction.x().value() == 0 ){
          return false;
        }

        experimental_double time = (center().x() - start.x())/direction.x();

        *ep = (experimental_vector(start) + time*direction).point_from_vector();

        return intersect(*ep);
      }
      else if( view() == "y" ){
        if( direction.y().value() == 0 ){
          return false;
        }

        experimental_double time = (center().y() - start.y())/direction.y();

        *ep = (experimental_vector(start) + time*direction).point_from_vector();

        return intersect(*ep);
      }

      if( direction.z().value() == 0 ){
        return false;
      }
      
      experimental_double time = (center().z() - start.z())/direction.z();
      
      *ep = (experimental_vector(start) + time*direction).point_from_vector();
      
      return intersect(*ep);


    }


    // vector from the face of the plane to the point
    experimental_vector norm_to_point(experimental_point ep)const{

      experimental_vector result = norm();
      return ((ep - face())*result)*result;

    }

  };
}
}

#endif
