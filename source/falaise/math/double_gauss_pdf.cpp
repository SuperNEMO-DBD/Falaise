#include <falaise/math/double_gauss_pdf.hpp>

// Standard library:
#include <stdexcept>

// GSL:
#include <gsl/gsl_math.h>

// Bayeux:
#include <bayeux/datatools/exception.h>



namespace falaise {

  namespace math {

    double_gauss_pdf::double_gauss_pdf(const double xmax_,
				       const double sigma_left_,
				       const double sigma_right_)
    {
      DT_THROW_IF(sigma_left_ <= 0.0, std::domain_error,
		  "Invalid sigma left value");
      DT_THROW_IF(sigma_right_ <= 0.0, std::domain_error,
		  "Invalid sigma right value");
      _xmax_ = xmax_;
      _sigma_left_ = sigma_left_;
      _sigma_right_ = sigma_right_;
      _a_ = 2 * _sigma_left_ / (_sigma_left_ + _sigma_right_);
      _b_ = 2.0 - _a_;
      return;
    }
    
    double double_gauss_pdf::xmax() const
    {
      return _xmax_;
    }
    
    double double_gauss_pdf::sigma_left() const
    {
      return _sigma_left_;
    }
   
    double double_gauss_pdf::sigma_right() const
    {
      return _sigma_right_;
    }
  
    double double_gauss_pdf::a() const
    {
      return _a_;
    }
 
    double double_gauss_pdf::b() const
    {
      return _b_;
    }

    double double_gauss_pdf::operator()(const double x_) const
    {
      if (x_ < _xmax_) {
	return _a_ * std::exp(-0.5 * gsl_pow_2((x_ - _xmax_) / _sigma_left_)) / _sigma_left_ / std::sqrt(2 * M_PI);
      }
      return _b_ * std::exp(-0.5 * gsl_pow_2((x_ - _xmax_) / _sigma_right_)) / _sigma_right_ / std::sqrt(2 * M_PI);
    }
    
    double_gauss_generator::double_gauss_generator(const double_gauss_pdf & pdf_)
      : _pdf_(pdf_)
    {
      _rmed_ = _pdf_.a() / (_pdf_.a()+ _pdf_.b());
      return;
    }

    double double_gauss_generator::shoot(mygsl::rng & random_) const
    {
      double r = random_.uniform();
      if (r < _rmed_) {
	return _pdf_.xmax() - std::fabs(random_.gaussian(_pdf_.sigma_left()));
      }
      return _pdf_.xmax() + std::fabs(random_.gaussian(_pdf_.sigma_right()));
    }
    
  } // end of namespace simulation

} // end of namespace snemo
