/* -*- mode: c++ -*- */

#include <CATAlgorithm/joint.h>


namespace CAT {

  namespace topology{

    using namespace std;
    using namespace mybhep;

    //!Default constructor
    joint::joint()
    {
      appname_= "joint: ";
      used_ = false;
      chi2_ = 0.;
      return;
    }

    //!Default destructor
    joint::~joint()
    {
      return;
    }

    //! constructor
    joint::joint(experimental_point epa,
                 experimental_point epb,
                 experimental_point epc, prlevel level, double nsigma)
    {
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "joint: ";
      epa_ = epa;
      epb_ = epb;
      epc_ = epc;
      calculate_kinks();
      used_ = false;
      chi2_ = 0.;
    }

    /*** dump ***/
    void joint::dump (std::ostream & a_out ,
                      const std::string & a_title,
                      const std::string & a_indent,
                      bool a_inherit ) const
    {
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << endl;
        }

      a_out << indent << appname_ << " -------------- " << endl;
      a_out << indent << " used: " << used() << " chi2 " << chi2() << endl;
      a_out << indent << " first point " << endl;
      this->epa().dump(a_out, "", indent + "    ");
      a_out << indent << " second point " << endl;
      this->epb().dump(a_out, "", indent + "    ");
      a_out << indent << " third point " << endl;
      this->epc().dump(a_out, "", indent + "    ");
      a_out << indent << "phi : "; kink_phi().dump(); a_out << " " << endl;
      a_out << indent << "theta : "; kink_theta().dump(); a_out << " " << endl;
      a_out << indent << " -------------- " << endl;

      return;
    }



    //! set experimental_points
    void joint::set(experimental_point epa, experimental_point epb, experimental_point epc)
    {
      epa_ = epa;
      epb_ = epb;
      epc_ = epc;
      calculate_kinks();
    }


    //! set first experimental_points
    void joint::set_epa(experimental_point epa)
    {
      epa_ = epa;
    }

    //! set second experimental_points
    void joint::set_epb(experimental_point epb)
    {
      epb_ = epb;
    }

    //! set third experimental_points
    void joint::set_epc(experimental_point epc)
    {
      epc_ = epc;
    }

    //! set kink phi
    void joint::set_kink_phi(experimental_double phi)
    {
      kink_phi_ = phi;
    }

    //! set kink theta
    void joint::set_kink_theta(experimental_double theta)
    {
      kink_theta_ = theta;
    }

    //! set used
    void joint::set_used(bool used)
    {
      used_ = used;
    }

    //! set chi2
    void joint::set_chi2(double chi2)
    {
      chi2_ = chi2;
    }

    //! get experimental_point a
    const experimental_point& joint::epa()const
    {
      return epa_;
    }

    //! get experimental_point b
    const experimental_point& joint::epb()const
    {
      return epb_;
    }

    //! get experimental_point c
    const experimental_point& joint::epc()const
    {
      return epc_;
    }

    //! get kink phi
    const experimental_double& joint::kink_phi()const
    {
      return kink_phi_;
    }

    //! get kink theta
    const experimental_double& joint::kink_theta()const
    {
      return kink_theta_;
    }

    //! get used
    bool joint::used() const
    {
      return used_;
    }

    //! get chi2
    double joint::chi2() const
    {
      return chi2_;
    }

    joint joint::invert()
    {
      joint inverted;
      inverted.set_print_level(print_level());
      inverted.set_nsigma(nsigma());
      inverted.set_epa(epc());
      inverted.set_epb(epb());
      inverted.set_epc(epa());
      inverted.set_used(used());
      inverted.set_chi2(chi2());
      inverted.set_kink_phi(-kink_phi());
      inverted.set_kink_theta(-kink_theta());
      return inverted;
    }

    bool joint::operator<(const joint &j) const
    {
      return (chi2() < j.chi2());
    }

    void joint::calculate_kinks()
    {
      experimental_vector v1(epa_, epb_);
      experimental_vector v2(epb_, epc_);
      kink_phi_ = v1.kink_phi(v2);
      kink_theta_ = v1.kink_theta(v2);
      return;
    }

  }
}
