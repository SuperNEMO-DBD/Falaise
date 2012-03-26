/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICALOHIT
#define __CATAlgorithm__ICALOHIT
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/plane.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>


namespace CAT{
namespace topology{

using namespace std;
using namespace mybhep;

  class calorimeter_hit : public tracking_object {

    // a calorimeter_hit is identified by one plane
    // energy and time

  private:
    string appname_;

    // energy
    experimental_double e_;

    // time
    experimental_double t_;

    size_t id_;

  public:   

    // plane
    plane pl_;

    double layer_;

    //!Default constructor 
    calorimeter_hit(prlevel level=mybhep::NORMAL, double nsigma=10.)
    {
      appname_= "calorimeter_hit: ";
      pl_ = plane();
      e_ = experimental_double(small_neg, small_neg);
      t_ = experimental_double(small_neg, small_neg);
      set_print_level(level);
      set_nsigma(nsigma);
      id_ = default_integer;
      layer_ = small_neg;
    }

    //!Default destructor
    virtual ~calorimeter_hit(){};

    //! constructor
    calorimeter_hit(plane pl, experimental_double e, experimental_double t, size_t id, double layer, prlevel level=mybhep::NORMAL, double nsigma=10.){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "calorimeter_hit: ";
      pl_ = pl;
      e_ = e;
      t_ = t;
      id_ = id;
      layer_ = layer;
    }

    /*** dump ***/
    void dump (ostream & a_out         = clog,
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
        a_out << indent << " id " << id() << " plane " << endl;
        pl_.dump(a_out, "", indent + "    ");
        a_out << indent << " energy "; this->e().dump(a_out, "", indent + "    ");
        a_out << indent << " time "; this->t().dump(a_out, "", indent + "    ");
        a_out << indent << " -------------- " << endl;

        return;
      }
    }



    //! set 
    void set(plane pl, experimental_double e, experimental_double t, size_t id)
      {
        pl_ = pl;
        e_ = e;
        t_ = t;
        id_ = id;
      }


    //! set plane
    void set_pl(plane pl)
      {
        pl_ = pl;
      }

    //! set energy
    void set_e(experimental_double e)
      {
        e_ = e;
      }

    //! set time
    void set_t(experimental_double t)
      {
        t_ = t;
      }

    //! set layer
    void set_layer(double layer)
    {
      layer_ = layer;
    }

    //! get plane
    const plane& pl()const
    {
      return pl_;
    }      

    //! get energy
    const experimental_double& e()const
    {
      return e_;
    }      

    //! get time
    const experimental_double& t()const
    {
      return t_;
    }      

    //! get id
    const size_t& id()const
    {
      return id_;
    }      

    //!get layer
    const double& layer() const {return layer_;}

    bool same_calo(calorimeter_hit c)const{

      double dist = (experimental_vector(pl().center(), c.pl().center())).length().value();

      if( dist < 0.1 ) 
        return true;

      return false;
    }


  };
}
}

#endif
