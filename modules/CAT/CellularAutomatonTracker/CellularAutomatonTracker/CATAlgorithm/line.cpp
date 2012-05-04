/* -*- mode: c++ -*- */

#include <CATAlgorithm/line.h>

namespace CAT {

  namespace topology{

    using namespace std;
    using namespace mybhep;

    //!Default constructor 
    line::line(prlevel level, double nsigma)
    {
      appname_= "line: ";
      forward_axis_ = experimental_vector(small_neg,small_neg,small_neg,
                                          small_neg, small_neg, small_neg);
      used_ = false;
      set_print_level(level);
      set_nsigma(nsigma);
    }

    //!Default destructor
    line::~line()
    {
      return;
    }

    //! constructor
    line::line(const experimental_point & epa, const experimental_point & epb, prlevel level, double nsigma){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "line: ";
      epa_ = epa;
      epb_ = epb;
      set_forward_axis();
      used_ = false;
    }

    /*** dump ***/
    void line::dump (std::ostream & a_out       ,
                     const std::string & a_title,
                     const std::string & a_indent,
                     bool a_inherit) const
    {
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << endl;
        }

      a_out << indent << appname_ << " -------------- " << endl;
      a_out << indent << " used: " << used() << endl;
      a_out << indent << " first point " << endl;
      this->epa().dump(a_out, "", indent + "    ");
      a_out << indent << " second point " << endl;
      this->epb().dump(a_out, "", indent + "    ");
      a_out << indent << " forward axis " << endl;
      this->forward_axis().dump(a_out, "", indent + "    ");
      a_out << indent << " -------------- " << endl;

      return;
    }



  //! set experimental_points
  void line::set(const experimental_point & epa, const experimental_point & epb)
  {
    epa_ = epa;
    epb_ = epb;
    set_forward_axis();
  }


  //! set first experimental_point
  void line::set_epa(const experimental_point & epa)
  {
    epa_ = epa;
  }

  //! set second experimental_point
  void line::set_epb(const experimental_point & epb)
  {
    epb_ = epb;
  }

  //! set used
  void line::set_used(bool used)
  {
    used_ = used;
  }

  //! get experimental_point a
  const experimental_point& line::epa()const
  {
    return epa_;
  }      

  //! get experimental_point b
  const experimental_point& line::epb()const
  {
    return epb_;
  }      

  //! get forward axis
  const experimental_vector& line::forward_axis()const
  {
    return forward_axis_;
  }

  //! get used
  bool line::used() const
  {
    return used_;
  }

  experimental_double line::phi(){
    return forward_axis_.phi();
  }
    
  experimental_double line::theta(){
    return forward_axis_.theta();
  }

  experimental_double line::kink_phi(const line & l){
    return forward_axis_.kink_phi(l.forward_axis());
  }

  experimental_double line::kink_theta(const line & l)
  {
    return forward_axis_.kink_theta(l.forward_axis());
  }

  double line::chi2(const line & l, bool use_theta_kink)
  {

    experimental_double phi_kink = kink_phi(l);
    experimental_double theta_kink = kink_theta(l);

    double result = square(phi_kink.value()/phi_kink.error()) ;

    if( use_theta_kink ){
      result += square(theta_kink.value()/theta_kink.error());
    }

    if( print_level() > mybhep::VERBOSE ){
      clog << appname_ << " calculate chi2: phi kink : "; (phi_kink*180./M_PI).dump();
      if( use_theta_kink ){
        clog << " theta kink : "; (theta_kink*180./M_PI).dump();
      }
      clog << " chi2 " << result << endl;
    }

    return result;
  }


  void line::set_a_forward_axis(const experimental_vector &  v)
  {
      
    forward_axis_ = v;

    return;
  }

  line line::invert()
  {
    line inverted(print_level(), nsigma());
    inverted.set_epa(epb());
    inverted.set_epb(epa());
    inverted.set_used(used());
    experimental_vector O(0.,0.,0.,0.,0.,0.);
    inverted.set_a_forward_axis(O-forward_axis());
    return inverted;

  }


  void line::set_forward_axis()
  {
      
    forward_axis_.set(epa(), epb());

    return;
  }

}
}
