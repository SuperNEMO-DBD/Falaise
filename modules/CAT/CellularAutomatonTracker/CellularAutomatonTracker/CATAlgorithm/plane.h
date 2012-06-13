/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__plane_h
#define __CATAlgorithm__plane_h 1

#include <iostream>
#include <string>
#include <mybhep/utilities.h>
#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/experimental_point.h>


namespace CAT {
  namespace topology{

    using namespace mybhep;

    class plane : public tracking_object {

      // a plane is identified by one point (center)
      // the plane sizes
      // and a normal to the plane

    private:
      std::string appname_;

      // experimental point
      experimental_point center_; 

      // sizes
      experimental_vector sizes_;

      // normal looking towards the origin
      experimental_vector norm_;

      // Nemo3 or SuperNEMO
      std::string type_;

    public:   
      //!Default constructor 
      plane(prlevel level=mybhep::NORMAL, double nsigma=10.);

      //!Default destructor
      virtual ~plane();

      //! constructor
      plane(experimental_point center, experimental_vector sizes, experimental_vector norm, prlevel level=mybhep::NORMAL, double nsigma=10.);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false)const;


      //! set 
      void set(experimental_point center, experimental_vector sizes, experimental_vector norm);


      //! set center
      void set_center(experimental_point center);
 
      //! set sizes
      void set_sizes(experimental_vector sizes);


      //! set norm
      void set_norm(experimental_vector norm);

      //! set type
      void set_type(string type);

      //! get center
      const experimental_point& center()const;

      //! get sizes
      const experimental_vector& sizes()const;

      // returns the normal looking towards the origin
      const experimental_vector& norm() const;

      // get type
      const string& type() const;

      std::string view()const;
    

      //! get point of the face of the plane
      experimental_point face()const;

      bool intersect(experimental_point ep)const;

      bool intersect(experimental_point start, experimental_vector direction, experimental_point* ep)const;

      // vector from the face of the plane to the point
      experimental_vector norm_to_point(experimental_point ep)const;

    };
  }
}

#endif // __CATAlgorithm__plane_h
