/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__IEPOINT
#define __CATAlgorithm__IEPOINT
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>
#include <CATAlgorithm/experimental_double.h>

namespace CAT{

  namespace topology{


    //  class experimental_point : public point{
    class experimental_point {

      // an experimental point is composed of an ordinary point (x, y, z)
      // with corresponding error (ex, ey, ez)

    private:
      std::string appname_;

      // x coordinate
      experimental_double x_;

      // y coordinate
      experimental_double y_;

      // z coordinate
      experimental_double z_;

      // radius in horizontal view (= x^2 + z^2)
      experimental_double radius_;

    public:
      //!Default constructor
      experimental_point()
      {
        appname_ = "experimental_point: ";
        x_ = experimental_double();
        y_ = experimental_double();
        z_ = experimental_double();
        radius_ = experimental_double();
      }

      //!Default destructor
      virtual ~experimental_point(){};

      //! constructor
      experimental_point(experimental_double x, experimental_double y, experimental_double z){
        appname_ = "experimental_point: ";
        x_ = x;
        y_ = y;
        z_ = z;
        set_radius();
      }

      //! constructor from coordinates with error
      experimental_point(double x, double y, double z, double ex, double ey, double ez){
        appname_ = "experimental_point: ";
        x_.set_value(x);
        y_.set_value(y);
        z_.set_value(z);
        x_.set_error(ex);
        y_.set_error(ey);
        z_.set_error(ez);
        set_radius();
      }

      //! constructor
      experimental_point(mybhep::point p, double ex, double ey, double ez){
        appname_ = "experimental_point: ";
        x_.set_value(p.x());
        y_.set_value(p.y());
        z_.set_value(p.z());
        set_radius();
        x_.set_error(ex);
        y_.set_error(ey);
        z_.set_error(ez);

      }

      //! constructor from bhep hit
      experimental_point(mybhep::hit hit){
        appname_ = "experimental_point: ";
        std::vector<float> cellpos;
        mybhep::vector_from_string(hit.fetch_property("CELL_POS"), cellpos);
        x_.set_value(cellpos[0]);
        y_.set_value(cellpos[1]);
        z_.set_value(cellpos[2]);
        set_radius();

        x_.set_error(0.); // exact position of center of wire
        z_.set_error(0.);
        y_.set_error(8.*mybhep::mm); // transverse error (mm)

        if( hit.find_property("ERRY") ){
          double erry = mybhep::double_from_string(hit.fetch_property("ERRY"));
          if(! isnan(erry) && !isinf(erry) )
            y_.set_error(erry);
        }

      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit               = false) const{
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }

          a_out << indent << appname_ << std::endl;
          a_out << indent << " x : "; (x()/mybhep::mm).dump(); a_out << " [mm] " << std::endl;
          a_out << indent << " y : "; (y()/mybhep::mm).dump(); a_out << " [mm] " << std::endl;
          a_out << indent << " z : "; (z()/mybhep::mm).dump(); a_out << " [mm] " << std::endl;

          return;
        }
      }


      //! set point and errors
      void set(mybhep::point p, double ex,double ey, double ez)
      {
        x_.set_value(p.x());
        y_.set_value(p.y());
        z_.set_value(p.z());
        set_radius();
        x_.set_error(ex);
        y_.set_error(ey);
        z_.set_error(ez);
      }


      //! set x
      void set_x(experimental_double x)
      {
        x_.set_value(x.value());
        x_.set_error(x.error());
        set_radius();
      }

      //! set ex
      void set_ex(double ex)
      {
        x_.set_error(ex);
        set_radius();
      }

      //! set y
      void set_y(experimental_double y)
      {
        y_.set_value(y.value());
        y_.set_error(y.error());
      }

      //! set ey
      void set_ey(double ey)
      {
        y_.set_error(ey);
        set_radius();
      }

      //! set z
      void set_z(experimental_double z)
      {
        z_.set_value(z.value());
        z_.set_error(z.error());
        set_radius();
      }

      //! set ez
      void set_ez(double ez)
      {
        z_.set_error(ez);
        set_radius();
      }

      //!get experimental x
      const experimental_double& x() const
      {
        return x_;
      }

      //!get experimental y
      const experimental_double& y() const
      {
        return y_;
      }

      //!get experimental z
      const experimental_double& z() const
      {
        return z_;
      }

      //!get experimental radius
      const experimental_double& radius() const
      {
        return radius_;
      }

      //! distance
      experimental_double distance(const experimental_point& p2) const
      {
        experimental_double result;

        result.set_value( sqrt(mybhep::square(x_.value()-p2.x().value())
                               + mybhep::square(y_.value()-p2.y().value()) +
                               mybhep::square(z_.value()-p2.z().value())));
        result.set_error( sqrt( mybhep::square(x_.value()*x_.error()) + mybhep::square(p2.x().value()*p2.x().error()) +
                                mybhep::square(y_.value()*y_.error()) + mybhep::square(p2.y().value()*p2.y().error()) +
                                mybhep::square(z_.value()*z_.error()) + mybhep::square(p2.z().value()*p2.z().error()))/result.value());

        return result;
      }


    private:

      void set_radius(){
        // propagate radius error:
        //  r = sqrt(x^2 + z^2)
        //  dr/dx = x/r,
        //  dr/dz = z/r

        double rr = sqrt(mybhep::square(x_.value()) + mybhep::square(z_.value()));
        if( isnan(rr) ) rr = mybhep::small_neg;
        double err = sqrt(mybhep::square(x_.value()*x_.error())
                          + mybhep::square(z_.value()*z_.error()))/rr;
        if( isnan(err) ) err = mybhep::small_neg;

        radius_.set_value(rr);
        radius_.set_error(err);

        return;
      }



    };


    // average
    inline experimental_point average (const std::vector<experimental_point> vs)
    {
      std::vector<experimental_double> xs;
      std::vector<experimental_double> ys;
      std::vector<experimental_double> zs;

      for(std::vector<experimental_point>::const_iterator iv=vs.begin(); iv!=vs.end(); ++iv){
        xs.push_back(iv->x());
        ys.push_back(iv->y());
        zs.push_back(iv->z());
      }

      return experimental_point(average(xs), average(ys), average(zs));
    }



  }
}

#endif
