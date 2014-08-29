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
      ndof_ = 0;
      p_ = 0.;
      return;
    }

    //!Default destructor
    joint::~joint()
    {
      return;
    }

    //! constructor
    joint::joint(const experimental_point &epa,
                 const experimental_point &epb,
                 const experimental_point &epc, prlevel level, double probmin)
    {
      set_print_level(level);
      set_probmin(probmin);
      appname_= "joint: ";
      epa_ = epa;
      epb_ = epb;
      epc_ = epc;
      calculate_kinks();
      used_ = false;
      chi2_ = 0.;
      ndof_ = 0;
      p_ = 0.;
    }

    /*** dump ***/
    void joint::dump (std::ostream & a_out ,
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
      a_out << indent << " used: " << used() << " chi2 " << chi2() << " ndof " << ndof() << " prob " << p() << std::endl;
      a_out << indent << " first point " << std::endl;
      this->epa().dump(a_out, "", indent + "    ");
      a_out << indent << " second point " << std::endl;
      this->epb().dump(a_out, "", indent + "    ");
      a_out << indent << " third point " << std::endl;
      this->epc().dump(a_out, "", indent + "    ");
      a_out << indent << "phi : "; kink_phi().dump(); a_out << " " << std::endl;
      a_out << indent << "theta : "; kink_theta().dump(); a_out << " " << std::endl;
      a_out << indent << " -------------- " << std::endl;

      return;
    }



    //! set experimental_points
    void joint::set(const experimental_point &epa, const experimental_point &epb, const experimental_point &epc)
    {
      epa_ = epa;
      epb_ = epb;
      epc_ = epc;
      calculate_kinks();
    }


    //! set first experimental_points
    void joint::set_epa(const experimental_point &epa)
    {
      epa_ = epa;
    }

    //! set second experimental_points
    void joint::set_epb(const experimental_point &epb)
    {
      epb_ = epb;
    }

    //! set third experimental_points
    void joint::set_epc(const experimental_point &epc)
    {
      epc_ = epc;
    }

    //! set kink phi
    void joint::set_kink_phi(const experimental_double &phi)
    {
      kink_phi_ = phi;
    }

    //! set kink theta
    void joint::set_kink_theta(const experimental_double &theta)
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
    void joint::set_ndof(int32_t ndof)
    {
      ndof_ = ndof;
    }
    void joint::set_p(double p)
    {
      p_ = p;
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
    int32_t joint::ndof() const
    {
      return ndof_;
    }
    double joint::p() const
    {
      return p_;
    }

    joint joint::invert()
    {
      joint inverted;
      inverted.set_print_level(print_level());
      inverted.set_probmin(probmin());
      inverted.set_epa(epc());
      inverted.set_epb(epb());
      inverted.set_epc(epa());
      inverted.set_used(used());
      inverted.set_chi2(chi2());
      inverted.set_ndof(ndof());
      inverted.set_p(p());
      inverted.set_kink_phi(-kink_phi());
      inverted.set_kink_theta(-kink_theta());
      return inverted;
    }

    bool joint::operator<(const joint &j) const
    {
      return (p() > j.p());
    }

    void joint::calculate_kinks()
    {
      experimental_vector v1(epa_, epb_);
      experimental_vector v2(epb_, epc_);
      kink_phi_ = v1.kink_phi(v2);
      kink_theta_ = v1.kink_theta(v2);
      return;
    }

    double joint::calculate_chi2(joint j, topology::cell A, topology::cell B, topology::cell C, joint * modified, bool A_is_on_gap, bool B_is_on_gap)const{
      // this: A B C
      // j:    0 A B

      topology::experimental_double angle_AA, angle_BB, angle_phi, angle_theta;
      topology::experimental_point pa = j.epb();
      topology::experimental_point pb= this->epb();

      if( !A_is_on_gap && !B_is_on_gap){ // connection A->B is not through a gap
	pa = A.angular_average(pa, this->epa(), &angle_AA);
	pb = B.angular_average(j.epc(), pb, &angle_BB);
      }
      topology::joint lj(pa, pb, this->epc());
      angle_phi = lj.kink_phi();
      angle_theta = lj.kink_theta();
      *modified = lj;
      //double local_chi2 = pow(angle_AA.value()/angle_AA.error(),2) + pow(angle_BB.value()/angle_BB.error(),2) + pow(angle_phi.value()/angle_phi.error(),2) + pow(angle_theta.value()/angle_theta.error(),2);
      double local_chi2 = pow(angle_phi.value(),2);
      if (print_level() >= mybhep::VVERBOSE){
	std::clog << " CAT::joint::calculate_chi2: phiA " << experimental_vector(A.ep(), lj.epa()).phi().value()*180./acos(-1.) <<  " phiB " << experimental_vector(B.ep(), lj.epb()).phi().value()*180./acos(-1.) << " phiC " << experimental_vector(C.ep(), lj.epc()).phi().value()*180./acos(-1.) << " cell A_is_on_gap " << A_is_on_gap << " A " << A.id() << " angle_AA " << angle_AA.value() << " +- " << angle_AA.error() <<  " B_is_on_gap " << B_is_on_gap << " B " << B.id() << " angle_BB " << angle_BB.value() << " +- " << angle_BB.error() << " angle_phi " << angle_phi.value() << " +- " << angle_phi.error() << " angle_theta " << angle_theta.value() << " +- " << angle_theta.error() << " local chi2 " << local_chi2 << std::endl;
      }
      return local_chi2;

    }

  }
}
