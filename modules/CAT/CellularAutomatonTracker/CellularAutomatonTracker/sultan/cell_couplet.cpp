#include <sultan/cell_couplet.h>

namespace SULTAN{
  namespace topology{

    using namespace std;
     
    cell_couplet::~cell_couplet()
    {
    }

    cell_couplet::cell_couplet()
      {
        appname_= "cell_couplet: ";
        ca_ = cell();
        cb_ = cell();
        forward_axis_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                            mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        transverse_axis_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                               mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        forward_axis_calculated_ = false;
        transverse_axis_calculated_ = false;
        distance_ = experimental_double();
        distance_hor_ = experimental_double();
      }
    
    cell_couplet::cell_couplet(const cell &ca, const cell &cb, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
    }

    void cell_couplet::set_forward_axis(){

      if( forward_axis_calculated_ )
        return;

      forward_axis_.set(ca().ep(), cb().ep());

      distance_ = forward_axis().length();

      distance_hor_ = forward_axis_.hor().length();

      forward_axis_ /= distance_.value();

      forward_axis_calculated_ = true;

      return;
    }

    void cell_couplet::dump (std::ostream & a_out,
			     const std::string & a_title,
			     const std::string & a_indent,
			     bool a_inherit) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " ------------------- " << std::endl;
      a_out << indent  << " first cell " << std::endl;
      this->ca().dump(a_out,"",indent + "   ");
      a_out << indent << " second cell " << std::endl;
      this->cb().dump(a_out, "",indent + "   ");
      a_out << indent << " forward axis " << std::endl;
      this->forward_axis().dump(a_out,"",indent + "   ");
      a_out << indent << " transverse axis " << std::endl;
      this->transverse_axis().dump(a_out,"",indent + "   ");

      return;
    }

    void cell_couplet::set_transverse_axis(){

      if( transverse_axis_calculated_ )
        return;

      experimental_vector waxis(0.,1.,0.,0.,0.,0.);

      transverse_axis_ = (forward_axis() ^ waxis).unit();

      transverse_axis_calculated_ = true;

      return;

    }


    //! set cells
    void cell_couplet::set(const cell &ca, const cell &cb){
        ca_ = ca;
        cb_ = cb;
      }


    //! set fwd axis
    void cell_couplet::set_a_forward_axis(const experimental_vector &v)
    {
      forward_axis_ = v;
    }

    //! set trv axis
    void cell_couplet::set_a_transverse_axis(const experimental_vector &v)
    {
      transverse_axis_ = v;
    }

    //! set distance
    void cell_couplet::set_a_distance(const experimental_double &d)
    {
      distance_ = d;
    }

    //! set hor distance
    void cell_couplet::set_a_hor_distance(const experimental_double &d)
    {
      distance_hor_ = d;
    }

    //! get first cell
    const cell& cell_couplet::ca()const
    {
      return ca_;
    }

    //! get second cell
    const cell& cell_couplet::cb()const
    {
      return cb_;
    }

    //! get forward axis
    const experimental_vector& cell_couplet::forward_axis()const
    {
      return forward_axis_;
    }

    //! get transverse axis
    const experimental_vector& cell_couplet::transverse_axis()const
    {
      return transverse_axis_;
    }

    //! get distance
    const experimental_double& cell_couplet::distance()const
    {
      return distance_;
    }

    //! get horizontal distance
    const experimental_double& cell_couplet::distance_hor()const
    {
      return distance_hor_;
    }

    bool operator==(const cell_couplet& left,

                           const cell_couplet& right)
    {

      return left.cb().id() == right.cb().id();

    };


  }
}

