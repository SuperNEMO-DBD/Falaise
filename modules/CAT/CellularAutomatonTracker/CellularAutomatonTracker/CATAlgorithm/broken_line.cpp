/* -*- mode: c++ -*- */

#include <CATAlgorithm/broken_line.h>


namespace CAT {

  namespace topology{

    using namespace std;
    using namespace mybhep;

    //!Default constructor
    broken_line::broken_line()
    {
      appname_= "broken_line: ";
      chi2_ = 0.;
      ndof_ = 0;
      p_ = 0.;
      ifirst_ = 0;
      ilast_ = 0;
      return;
    }

    //!Default destructor
    broken_line::~broken_line()
    {
      return;
    }

    //! constructor
    broken_line::broken_line(const std::vector<experimental_point> &eps, 
			     prlevel level, double probmin)
    {
      set_print_level(level);
      set_probmin(probmin);
      appname_= "broken_line: ";
      eps_ = eps;
      chi2_ = 0.;
      ndof_ = 0;
      p_ = 0.;
      ifirst_ = 0;
      ilast_ = 0;
    }

    /*** dump ***/
    void broken_line::dump (std::ostream & a_out ,
                      const std::string & a_title,
                      const std::string & a_indent,
                      bool /*a_inherit*/ ) const
    {
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " -------------- " << std::endl;
      a_out << indent << " chi2 " << chi2() << " ndof " << ndof() << " prob " << p() << " ifirst " << ifirst() << " ilast " << ilast() << " points: " << std::endl;
      for(std::vector<experimental_point>::const_iterator p = eps_.begin(); p!=eps_.end(); ++p)
	p->dump(a_out, "", indent + "    ");
      a_out << indent << " -------------- " << std::endl;
      
      return;
    }



    //! set experimental_points
    void broken_line::set(const std::vector<experimental_point> &eps)
    {
      eps_ = eps;
    }


    //! set chi2
    void broken_line::set_chi2(double chi2)
    {
      chi2_ = chi2;
    }
    void broken_line::set_ndof(int32_t ndof)
    {
      ndof_ = ndof;
    }
    void broken_line::set_p(double p)
    {
      p_ = p;
    }
    void broken_line::set_ifirst(size_t i){
      ifirst_ = i;
    }
    void broken_line::set_ilast(size_t i){
      ilast_ = i;
    }

    //! get experimental_points
    const std::vector<experimental_point>& broken_line::eps()const
    {
      return eps_;
    }

    //! get chi2
    double broken_line::chi2() const
    {
      return chi2_;
    }
    int32_t broken_line::ndof() const
    {
      return ndof_;
    }
    double broken_line::p() const
    {
      return p_;
    }
    size_t broken_line::ifirst() const{
      return ifirst_;
    }
    size_t broken_line::ilast() const{
      return ilast_;
    }

    void broken_line::calculate_chi2(){

      if( eps_.size() < 3 ){
	chi2_ = 0.;
	ndof_ = 1;
	p_ = 1.;
	return;
      }

      chi2_ = 0.;
      ndof_ = 0;
      for(std::vector<experimental_point>::const_iterator p = eps_.begin(); p!=eps_.end()-2; ++p){
	experimental_point B = eps_[p - eps_.begin()+1];
	experimental_point C = eps_[p - eps_.begin()+2];
	experimental_vector AB(*p, B);
	experimental_vector BC(B,C);
	experimental_double phi = AB.kink_phi(BC);
	chi2_ += pow(phi.value(),2)/pow(phi.error(),2);
	ndof_ ++;
      }

      p_ = probof(chi2_,ndof_);

      return ;

    }

  }
}
