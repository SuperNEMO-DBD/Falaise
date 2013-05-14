/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICELLCOUP
#define __CATAlgorithm__ICELLCOUP
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>

namespace CAT{
  namespace topology{


    class cell_couplet : public tracking_object{

      // a cell_couplet is composed of two cells
      // and the tangents between them

    protected:
      std::string appname_;

      // first cell
      cell ca_;

      // second cell
      cell cb_;

      // unit axis from first to second cell
      experimental_vector forward_axis_;
      bool forward_axis_calculated_;

      // unit transverse axis
      experimental_vector transverse_axis_;
      bool transverse_axis_calculated_;

      // distance from first to second cell
      experimental_double distance_;

      // horizontal distance from first to second cell
      experimental_double distance_hor_;

    public:

      // list of tangents
      std::vector<line>  tangents_;

      // status of cell couplet
      bool free_;

      // begun cell couplet
      bool begun_;

      //!Default constructor
      cell_couplet();

      //!Default destructor
      virtual ~cell_couplet();

      //! constructor
      cell_couplet(const cell &ca,const cell &cb,const std::vector<line> &tangents);

      //! constructor
      cell_couplet(const cell &ca, const cell &cb, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);
   
      //! constructor
      cell_couplet(const cell &ca, const cell &cb, const std::string &just, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      //! constructor from bhep hit
      cell_couplet(const mybhep::hit &hita, const mybhep::hit &hitb);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set cells and tangents
      void set(const cell &ca, const cell &cb, const std::vector<line> &tangents);

      //! set cells
      void set(const cell &ca, const cell &cb);


      //! set free level
      void set_free(bool free);

      //! set begun level
      void set_begun(bool begun);

      //! set tangents
      void set_tangents( const std::vector<line> &tangents);

      //! set fwd axis
      void set_a_forward_axis(const experimental_vector &v);

      //! set trv axis
      void set_a_transverse_axis(const experimental_vector &v);

      //! set distance
      void set_a_distance(const experimental_double &d);

      //! set hor distance
      void set_a_hor_distance(const experimental_double &d);

      //! get first cell
      const cell& ca()const;

      //! get second cell
      const cell& cb()const;

      //! get tangents
      const std::vector<line>& tangents()const;

      //! get forward axis
      const experimental_vector& forward_axis()const;

      //! get transverse axis
      const experimental_vector& transverse_axis()const;

      //! get distance
      const experimental_double& distance()const;

      //! get horizontal distance
      const experimental_double& distance_hor()const;

      //! get free level
      const bool free()const;

      //! get begun level
      const bool begun()const;

    protected:

      void obtain_tangents();
      void obtain_tangents_between_circle_and_circle();
      void obtain_tangents_between_point_and_point(experimental_point &epa, experimental_point &epb);
      void obtain_tangents_between_circle_and_point(const cell &c, experimental_point &ep);
      void obtain_tangents_between_point_and_circle(experimental_point &ep, const cell &c);
      void set_first_error_in_build_from_cell(double sin, int sign_parallel_crossed, int sign_up_down, experimental_point *epa)const;
      void set_second_error_in_build_from_cell(double sin, int sign_parallel_crossed, int sign_up_down, experimental_point *epb)const;

    public:

      void set_forward_axis(void);
      void set_transverse_axis(void);
      size_t iteration()const;
      cell_couplet invert();
      void set_all_used();

      friend bool operator==(const cell_couplet& left,
                             const cell_couplet& right);

      //! are the two circles tangent or intersecting?
        bool intersecting()const;
    


    };


  }

}

#endif
