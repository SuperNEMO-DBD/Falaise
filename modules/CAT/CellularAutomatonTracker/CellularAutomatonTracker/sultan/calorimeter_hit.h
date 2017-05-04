/* -*- mode: c++ -*- */
#ifndef __SULTAN__calorimeter_hit_h
#define __SULTAN__calorimeter_hit_h 1

#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/plane.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>


namespace SULTAN {
  namespace topology {


    class calorimeter_hit : public tracking_object {

      // a calorimeter_hit is identified by one plane
      // energy and time

    private:

      std::string appname_;

      // energy
      experimental_double e_;

      // time
      experimental_double t_;

      size_t id_;

      int user_id_; // use this value to store user id, never inside tracking

    public:

      // plane
      plane pl_;

      double layer_;

      //!Default constructor
      calorimeter_hit(mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      //!Default destructor
      virtual ~calorimeter_hit();

      //! constructor
      calorimeter_hit(const plane & pl,
                      const experimental_double & e,
                      const experimental_double & t,
                      size_t id,
                      double layer,
                      mybhep::prlevel level=mybhep::NORMAL,
                      double probmin=1.e-200);

      /*** dump ***/
      void dump (std::ostream & a_out         = std::clog,
                 const std::string & a_title  = "",
                 const std::string & a_indent = "",
                 bool a_inherit               = false) const;
      //! set
      void set(const plane & pl, const experimental_double & e, const experimental_double & t, size_t id);

      //! set id
      void set_id(size_t id)
      {
        id_ = id;
      }
      void set_user_id(int id)
      {
        user_id_ = id;
      }

      //! set plane
      void set_pl(const plane & pl);

      //! set energy
      void set_e(const experimental_double & e);

      //! set time
      void set_t(const experimental_double & t);

      //! set layer
      void set_layer(double layer);

      //! get plane
      const plane & pl() const;

      //! get energy
      const experimental_double& e()const;

      //! get time
      const experimental_double& t()const;

      //! get id
      size_t id()const;
      int user_id()const;

      //!get layer
      double layer() const;

      bool same_calo(const calorimeter_hit & c) const;

    };

  }
}

#endif // __SULTAN__calorimeter_hit_h
