#include <CATAlgorithm/experimental_point.h>

namespace CAT{

  namespace topology{

      //!Default constructor
    experimental_point::experimental_point()
      {
        appname_ = "experimental_point: ";
        x_ = experimental_double();
        y_ = experimental_double();
        z_ = experimental_double();
        radius_ = experimental_double();
      }

      //!Default destructor
     experimental_point::~experimental_point()
     {
     }

      //! constructor
      experimental_point::experimental_point(const experimental_double &x, const experimental_double &y, const experimental_double &z){
        appname_ = "experimental_point: ";
        x_ = x;
        y_ = y;
        z_ = z;
        set_radius();
      }

      //! constructor from coordinates with error
      experimental_point::experimental_point(double x, double y, double z, double ex, double ey, double ez){
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
      experimental_point::experimental_point(const mybhep::point &p, double ex, double ey, double ez){
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
      experimental_point::experimental_point(const mybhep::hit &hit){
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
      void experimental_point::dump (std::ostream & a_out,
                         const std::string & a_title  ,
                         const std::string & a_indent ,
                                     bool /* a_inherit */  ) const{
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
      void experimental_point::set(const mybhep::point &p, double ex,double ey, double ez)
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
      void experimental_point::set_x(const experimental_double &x)
      {
        x_.set_value(x.value());
        x_.set_error(x.error());
        set_radius();
      }

      //! set ex
      void experimental_point::set_ex(double ex)
      {
        x_.set_error(ex);
        set_radius();
      }

      //! set y
      void experimental_point::set_y(const experimental_double &y)
      {
        y_.set_value(y.value());
        y_.set_error(y.error());
      }

      //! set ey
      void experimental_point::set_ey(double ey)
      {
        y_.set_error(ey);
        set_radius();
      }

      //! set z
      void experimental_point::set_z(const experimental_double &z)
      {
        z_.set_value(z.value());
        z_.set_error(z.error());
        set_radius();
      }

      //! set ez
      void experimental_point::set_ez(double ez)
      {
        z_.set_error(ez);
        set_radius();
      }

      //!get experimental x
      const experimental_double& experimental_point::x() const
      {
        return x_;
      }

      //!get experimental y
      const experimental_double& experimental_point::y() const
      {
        return y_;
      }

      //!get experimental z
      const experimental_double& experimental_point::z() const
      {
        return z_;
      }

      //!get experimental radius
      const experimental_double& experimental_point::radius() const
      {
        return radius_;
      }

      //! distance
      experimental_double experimental_point::distance(const experimental_point& p2) const
      {
        experimental_double result;

        result.set_value( std::sqrt(mybhep::square(x_.value()-p2.x().value())
                               + mybhep::square(y_.value()-p2.y().value()) +
                               mybhep::square(z_.value()-p2.z().value())));
        result.set_error( std::sqrt( mybhep::square(x_.value()*x_.error()) + mybhep::square(p2.x().value()*p2.x().error()) +
                                mybhep::square(y_.value()*y_.error()) + mybhep::square(p2.y().value()*p2.y().error()) +
                                mybhep::square(z_.value()*z_.error()) + mybhep::square(p2.z().value()*p2.z().error()))/result.value());

        return result;
      }


      //! distance
      experimental_double experimental_point::hor_distance(const experimental_point& p2) const
      {
        experimental_double result;

        result.set_value( std::sqrt(mybhep::square(x_.value()-p2.x().value())+
                               mybhep::square(z_.value()-p2.z().value())));
        result.set_error( std::sqrt( mybhep::square(x_.value()*x_.error()) + mybhep::square(p2.x().value()*p2.x().error()) +
                                mybhep::square(z_.value()*z_.error()) + mybhep::square(p2.z().value()*p2.z().error()))/result.value());

        return result;
      }


      void experimental_point::set_radius(){
        // propagate radius error:
        //  r = sqrt(x^2 + z^2)
        //  dr/dx = x/r,
        //  dr/dz = z/r

        double rr = std::sqrt(mybhep::square(x_.value()) + mybhep::square(z_.value()));
        if( isnan(rr) ) rr = mybhep::small_neg;
        double err = std::sqrt(mybhep::square(x_.value()*x_.error())
                          + mybhep::square(z_.value()*z_.error()))/rr;
        if( isnan(err) ) err = mybhep::small_neg;

        radius_.set_value(rr);
        radius_.set_error(err);

        return;
      }

    experimental_point average (const std::vector<experimental_point> &vs)
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
