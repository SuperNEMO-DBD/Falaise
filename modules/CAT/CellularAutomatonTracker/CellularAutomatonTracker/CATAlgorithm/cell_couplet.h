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
    cell_couplet()
    {
      appname_= "cell_couplet: ";
      ca_ = cell();
      cb_ = cell();
      tangents_.clear();
      forward_axis_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                          mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
      transverse_axis_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                             mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      distance_ = experimental_double();
      distance_hor_ = experimental_double();
      free_ = false;
      begun_ = false;
    }

    //!Default destructor
    virtual ~cell_couplet(){};

    //! constructor
    cell_couplet(const cell &ca,const cell &cb,const std::vector<line> &tangents){
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      for(std::vector<line>::const_iterator itang=tangents.begin(); itang!=tangents.end(); ++itang)
        tangents_.push_back(*itang);
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      free_ = false;
      begun_ = false;
    }

    //! constructor
    cell_couplet(const cell &ca, const cell &cb, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      obtain_tangents();
      free_ = false;
      begun_ = false;
    }

    //! constructor
    cell_couplet(const cell &ca, const cell &cb, const std::string &just, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      free_ = false;
      begun_ = false;
    }

    //! constructor from bhep hit
    cell_couplet(const mybhep::hit &hita, const mybhep::hit &hitb){
      appname_= "cell_couplet: ";
      ca_ = cell(hita);
      cb_ = cell(hitb);
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      obtain_tangents();
      free_ = false;
      begun_ = false;
    }

    /*** dump ***/
    virtual void dump (std::ostream & a_out         = std::clog,
                       const std::string & a_title  = "",
                       const std::string & a_indent = "",
                       bool a_inherit          = false) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " ------------------- " << std::endl;
      a_out << indent  << " free: " << free() << " begun: " << begun() << std::endl;
      a_out << indent  << " first cell " << std::endl;
      this->ca().dump(a_out,"",indent + "   ");
      a_out << indent << " second cell " << std::endl;
      this->cb().dump(a_out, "",indent + "   ");
      a_out << indent << " tangents: " << tangents().size() << std::endl;
      for(std::vector<line>::const_iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang)
        itang->dump(a_out,"",indent + "   ");
      a_out << indent << " forward axis " << std::endl;
      this->forward_axis().dump(a_out,"",indent + "   ");
      a_out << indent << " transverse axis " << std::endl;
      this->transverse_axis().dump(a_out,"",indent + "   ");
      a_out << indent  << " ------------------- " << std::endl;

      return;
    }

    //! set cells and tangents
    void set(const cell &ca, const cell &cb, const std::vector<line> &tangents){
        ca_ = ca;
        cb_ = cb;
        for(std::vector<line>::const_iterator itang=tangents.begin(); itang!=tangents.end(); ++itang)
          tangents_.push_back(*itang);
        set_forward_axis();
        set_transverse_axis();
      }


    //! set cells
    void set(const cell &ca, const cell &cb){
        ca_ = ca;
        cb_ = cb;
        obtain_tangents();
      }


    //! set free level
    void set_free(bool free){
      free_ = free;
    }

    //! set begun level
    void set_begun(bool begun){
      begun_ = begun;
    }

    //! set tangents
    void set_tangents( const std::vector<line> &tangents)
    {
      tangents_ = tangents;
    }

    //! set fwd axis
    void set_a_forward_axis(const experimental_vector &v)
    {
      forward_axis_ = v;
    }

    //! set trv axis
    void set_a_transverse_axis(const experimental_vector &v)
    {
      transverse_axis_ = v;
    }

    //! set distance
    void set_a_distance(const experimental_double &d)
    {
      distance_ = d;
    }

    //! set hor distance
    void set_a_hor_distance(const experimental_double &d)
    {
      distance_hor_ = d;
    }

    //! get first cell
    const cell& ca()const
    {
      return ca_;
    }

    //! get second cell
    const cell& cb()const
    {
      return cb_;
    }

    //! get tangents
    const std::vector<line>& tangents()const
    {
      return tangents_;
    }

    //! get forward axis
    const experimental_vector& forward_axis()const
    {
      return forward_axis_;
    }

    //! get transverse axis
    const experimental_vector& transverse_axis()const
    {
      return transverse_axis_;
    }

    //! get distance
    const experimental_double& distance()const
    {
      return distance_;
    }

    //! get horizontal distance
    const experimental_double& distance_hor()const
    {
      return distance_hor_;
    }

    //! get free level
    const bool free()const{
      return free_;
    }

    //! get begun level
    const bool begun()const{
      return begun_;
    }

  protected:

    void obtain_tangents();
    void obtain_tangents_between_circle_and_circle();
    void obtain_tangents_between_point_and_point(experimental_point epa, experimental_point epb);
    void obtain_tangents_between_circle_and_point(cell c, experimental_point ep);
    void obtain_tangents_between_point_and_circle(experimental_point ep, cell c);

  public:

    void set_forward_axis(void);
    void set_transverse_axis(void);

    size_t iteration()const{
      for(std::vector<line>::const_iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang)
        if( ! itang->used() )
          return itang - tangents_.begin();

      return tangents().size();
    }


    cell_couplet invert(){

      //      clock.start(" cell couplet: invert ","cumulative");


      cell_couplet inverted;
      inverted.set(cb(),ca());
      inverted.set_free(free());
      inverted.set_begun(begun());

      std::vector<line> inverted_lines;
      for(std::vector<line>::iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang){
        inverted_lines.push_back(itang->invert());
      }
      inverted.set_tangents( inverted_lines );

      experimental_vector O(0.,0.,0.,0.,0.,0.);
      inverted.set_a_forward_axis(O-forward_axis());
      inverted.set_a_transverse_axis(O-transverse_axis());
      inverted.set_a_distance(-distance());
      inverted.set_a_hor_distance(-distance_hor());

      //      clock.stop(" cell couplet: invert ");

      return inverted;
    }


    void set_all_used(){
      //      clock.start(" cell couplet: set all used ","cumulative");
      for(std::vector<line>::iterator itang=tangents_.begin(); itang != tangents_.end(); ++itang)
        itang->set_used(true);

      set_begun(true);
      //      clock.stop(" cell couplet: set all used ");
      return;
    }

    friend bool operator==(const cell_couplet& left,

                           const cell_couplet& right)
    {

      return left.cb().id() == right.cb().id();

    };

    //! are the two circles tangent or intersecting?
    bool intersecting()const{

      return ca_.intersect(cb_);
    }


  };


}

}

#endif
