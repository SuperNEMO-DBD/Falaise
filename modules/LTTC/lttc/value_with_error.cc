 // \file value_with_error.cc

// Ourselves:
#include <lttc/value_with_error.hh>

// Standard library:
#include <cmath>

// Third party:
#include <boost/math/special_functions/pow.hpp>

namespace lttc {

  value_with_error::value_with_error(double value_, double sigma_)
  {
    set_value(value_);
    set_sigma(sigma_);
    return;
  }

  double value_with_error::value() const
  {
    return _value_;
  }
   
  void value_with_error::set_value(const double value_)
  {
    _value_ = value_;
    return;
  }
    
  double value_with_error::sigma() const
  {
    return _sigma_;
  }
 
  void value_with_error::set_sigma(const double sigma_)
  {
    DT_THROW_IF(sigma_ < 0.0, std::domain_error, "Invalid sigma!");
    _sigma_ = sigma_;
    return;
  }
 
  void value_with_error::set(const double value_, const double sigma_)
  {
    set_value(value_);
    set_sigma(sigma_);
    return;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const value_with_error & v_)
  {
    out_ << v_._value_ << " +/- " << v_._sigma_;
    return out_;
  }
  
  value_with_error operator+(const value_with_error & v1_,
                             const value_with_error & v2_)
  {
    value_with_error v;
    v._value_ = v1_._value_ + v2_._value_;
    v._sigma_ = std::sqrt(v1_._sigma_ * v1_._sigma_ + v2_._sigma_ * v2_._sigma_);
    return v;
  }

  value_with_error operator+(const value_with_error & v_,
                             const double val_)
  {
    value_with_error v;
    v._value_ = v_._value_ + val_;
    v._sigma_ = v_._sigma_;
    return v;
  }

  value_with_error operator+(const double val_,
                             const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = v_._value_ + val_;
    v._sigma_ = v_._sigma_;
    return v;
  }

  value_with_error operator-(const value_with_error & v1_,
                             const value_with_error & v2_)
  {
    value_with_error v;
    v._value_ = v1_._value_ - v2_._value_;
    v._sigma_ = std::sqrt(v1_._sigma_ * v1_._sigma_ + v2_._sigma_ * v2_._sigma_);
    return v;
  }

  value_with_error operator-(const value_with_error & v_,
                             const double val_)
  {
    value_with_error v;
    v._value_ = v_._value_ - val_;
    v._sigma_ = v_._sigma_;
    return v;
  }

  value_with_error operator-(const double val_,
                             const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = val_ - v_._value_;
    v._sigma_ = v_._sigma_;
    return v;
  }

  value_with_error operator-(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = - v_._value_;
    v._sigma_ = v_._sigma_;
    return v;
  }

  value_with_error operator*(const value_with_error & v1_,
                             const value_with_error & v2_)
  {
    value_with_error v;
    v._value_ = v1_._value_ * v2_._value_;
    v._sigma_ = std::hypot(v1_._value_ * v2_._sigma_, v2_._value_ * v2_._sigma_);
    return v;
  }

  value_with_error operator*(const value_with_error & v_,
                             const double val_)
  {
    value_with_error v;
    v._value_ = val_ * v_._value_;
    v._sigma_ = std::abs(val_) * v_._sigma_;
    return v;
  }

  value_with_error operator*(const double val_,
                             const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = val_ * v_._value_;
    v._sigma_ = std::abs(val_) * v_._sigma_;
    return v;
 }

  // friend
  value_with_error sqrt(const value_with_error & v_)
  {
    value_with_error v;
    double t = std::sqrt(v_._value_);
    v._value_ = t;
    v._sigma_ = 0.5 * v_._sigma_ / t;
    return v;
  }

  // friend
  value_with_error pow(const value_with_error & v_, const int n_)
  {
    value_with_error v;
    double t = std::pow(v_._value_, n_);
    v._value_ = t;
    v._sigma_ = n_ * t * v_._sigma_ / v_._value_;
    return v;
  }

  // friend
  value_with_error pow(const value_with_error & v_, const double alpha_)
  {
    value_with_error v;
    double t = std::pow(v_._value_, alpha_);
    v._value_ = t;
    v._sigma_ = alpha_ * t * v_._sigma_ / v_._value_;
    return v;
  }

  // friend
  value_with_error pow2(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = v_._value_ * v_._value_;
    v._sigma_ = 2 * v_._value_ * v_._sigma_;
    return v;
  }

  // friend
  value_with_error pow3(const value_with_error & v_)
  {
    value_with_error v;
    double t = v_._value_ * v_._value_;
    v._value_ = t * v_._value_;
    v._sigma_ = 3 * t * v_._sigma_;
    return v;
  }

  // friend
  value_with_error pow4(const value_with_error & v_)
  {
    value_with_error v;
    double t = v_._value_ * v_._value_;
    v._value_ = t * t;
    v._sigma_ = 4 * t * v_._value_ * v_._sigma_;
    return v;
  }

  // friend
  value_with_error cos(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::cos(v_._value_);
    v._sigma_ = std::abs(std::sin(v_._value_)) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error sin(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::sin(v_._value_);
    v._sigma_ = std::abs(std::cos(v_._value_)) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error tan(const value_with_error & v_)
  {
    value_with_error v;
    double t = std::tan(v_._value_);
    v._value_ = t;
    v._sigma_ = (1.0 + t * t) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error ctan(const value_with_error & v_)
  {
    value_with_error v;
    double t = std::tan(v_._value_);
    v._value_ = t;
    v._sigma_ = (1.0 + t * t) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error acos(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::acos(v_._value_);
    v._sigma_ = v_._sigma_ / std::sqrt(1 - v_._value_ * v_._value_);
    return v;
  }

  // friend
  value_with_error asin(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::asin(v_._value_);
    v._sigma_ = v_._sigma_ / std::sqrt(1 - v_._value_ * v_._value_);
    return v;
  }

  // friend
  value_with_error atan(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::atan(v_._value_);
    v._sigma_ = v_._sigma_ / (1 + v_._value_ * v_._value_);
    return v;
  }

  // friend
  value_with_error exp(const value_with_error & v_)
  {
    value_with_error v;
    double t = std::exp(v_._value_);
    v._value_ = t;
    v._sigma_ = t * v_._sigma_;
    return v;
  }

  // friend
  value_with_error cosh(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::cosh(v_._value_);
    v._sigma_ = std::abs(std::sinh(v_._value_)) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error sinh(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::sinh(v_._value_);
    v._sigma_ = std::cosh(v_._value_) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error tanh(const value_with_error & v_)
  {
    value_with_error v;
    double t = std::tanh(v_._value_);
    v._value_ = t;
    v._sigma_ = std::abs(1 - t * t) * v_._sigma_;
    return v;
  }

  // friend
  value_with_error log(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::log(v_._value_);
    v._sigma_ = v_._sigma_ / v_._value_;
    return v;
  }

  // friend
  value_with_error log10(const value_with_error & v_)
  {
    value_with_error v;
    v._value_ = std::log10(v_._value_);
    v._sigma_ = v_._sigma_ / v_._value_ / 2.30258509299405;
    return v;
  }

  // friend
  value_with_error hypot(const value_with_error & v1_,
                         const value_with_error & v2_)
  {
    value_with_error v;
    double h = std::hypot(v1_._value_, v2_._value_);;
    v._value_ = h;
    v._sigma_ = boost::math::pow_2(v1_._value_ * v1_._sigma_,  v2_._value_ * v2_._sigma_) / h;
    return v;
  }

} // namespace lttc 
