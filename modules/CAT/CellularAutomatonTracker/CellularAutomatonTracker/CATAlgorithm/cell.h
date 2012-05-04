/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICELL
#define __CATAlgorithm__ICELL
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>
#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>

namespace CAT {

  namespace topology {

    using namespace mybhep;

    class cell : public tracking_object {

      // a cell is composed of an experimental point
      // and an experimental radius

    private:
      std::string appname_;

      // experimental point
      experimental_point ep_; 

      // radius (original value)
      experimental_double r0_;

      // radius (modified value if the cell is "small")
      experimental_double r_;

      // id
      size_t id_;

      // characterize fast and delayed cells
      bool fast_;

      // layer number
      int layer_;

      // block number
      int block_;

      // iid number
      int iid_;

      // n3id number
      int n3id_;

      // N3 or SN
      string type_;

    public:   


      // status of cell couplet
      bool free_;

      // begun cell couplet
      bool begun_;

      //!Default constructor 
      cell();

      //!Default destructor
      virtual ~cell(){};

      //! constructor
      cell(experimental_point p, 
           experimental_double r, 
           size_t id, 
           bool fast=true, 
           double nsigma=10., 
           prlevel level=mybhep::NORMAL);

      //! constructor
      cell(experimental_point p, 
           double r, 
           double er, 
           size_t id, 
           bool fast=true, 
           prlevel level=mybhep::NORMAL, 
           double nsigma=10.);

      //! constructor
      cell(experimental_point p, 
           double r, 
           size_t id, 
           bool fast=true);

      //! constructor from bhep hit
      cell(mybhep::hit hit, size_t id, bool SuperNemo, prlevel level=mybhep::NORMAL, double nsigma=10.);

      //! constructor from bhep hit
      cell(mybhep::hit hit);


      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set experimental_point, radius, error and id;
      void set(const experimental_point & p, double r,double er, size_t id, bool fast);

      //! set experimental_point
      void set_p(const experimental_point & p);

      //! set radius
      void set_r(double r);

      //! set radius error
      void set_er(double er);

      //! set id
      void set_id(size_t id);

      //! set layer
      void set_layer(size_t layer);

      //! set block
      void set_block(size_t block);

      //! set iid
      void set_iid(size_t iid);

      //! set n3id
      void set_n3id(size_t n3id);

      //! set fast flag
      void set_fast(bool fast);

      //! set free level
      void set_free(bool free);

      //! set begun level
      void set_begun(bool begun);

      //! set type
      void set_type(string type);

      bool small() const;

      bool unknown_vertical() const;

      //! get experimental_point
      const experimental_point& ep()const;

      //!get experimental r
      const experimental_double& r() const;

      //!get original experimental r
      const experimental_double& r0() const; 

      //!get id
      const size_t& id() const;

      //!get layer
      const int& layer() const;

      //!get block
      const int& block() const;

      //!get iid
      const int& iid() const;

      //!get n3id
      const int& n3id() const;

      //!get fast flag
      const bool& fast() const;

      //! get free level
      const bool free()const;

      //! get begun level
      const bool begun()const;

      //! get type
      string type();

      //! get cell number
      int cell_number();

    public:

      experimental_double distance(const cell & c) const;

      experimental_point angular_average(const experimental_point & epa, 
                                         const experimental_point & epb, 
                                         experimental_double* angle);

      experimental_point build_from_cell(const experimental_vector & forward, 
                                         const experimental_vector & transverse, 
                                         const experimental_double & cos, int sign) const;

      void dump_point(const experimental_point & epp) const;

      double error_x_in_build_from_cell(const experimental_vector & forward, 
                                        const experimental_vector & transverse, 
                                        const experimental_double & cos, 
                                        double sin) const;

      double error_z_in_build_from_cell(const experimental_vector & forward, 
                                        const experimental_vector & transverse, 
                                        const experimental_double & cos, 
                                        double sin) const;

      bool same_quadrant(const experimental_point & epa, const experimental_point & epb) const;

      bool same_cell(const topology::cell & c) const;
      bool intersect(const topology::cell & c) const;



      bool operator<(const topology::cell& c) const;

      static bool compare(const topology::cell& c1, const topology::cell& c);


    private:
      void set_radius();

    };

  }
}

#endif
