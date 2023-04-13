#ifndef FALAISE_LTTC_PLUGIN_VALUE_WITH_ERROR_HH
#define FALAISE_LTTC_PLUGIN_VALUE_WITH_ERROR_HH

// Standard library:
#include <limits>

// Third party:
// - Bayeux/datatools :
#include <datatools/utils.h>

namespace lttc {

  class value_with_error
  {
  public:

    value_with_error() = default;

    value_with_error(double value_, double sigma_);

    double value() const;
   
    void set_value(const double value_);
    
    double sigma() const;
   
    void set_sigma(const double sigma_);
  
    void set(const double value_, const double sigma_);
 
    friend value_with_error operator+(const value_with_error & v1_,
                                      const value_with_error & v2_);

    friend value_with_error operator+(const value_with_error & v_,
                                      const double val_);

    friend value_with_error operator+(const double val_,
                                      const value_with_error & v_);

    friend value_with_error operator-(const value_with_error & v1_,
                                      const value_with_error & v2_);

    friend value_with_error operator-(const value_with_error & v_,
                                      const double val_);

    friend value_with_error operator-(const double val_,
                                      const value_with_error & v_);

    friend value_with_error operator-(const value_with_error & v_);

    friend value_with_error operator*(const value_with_error & v1_,
                                      const value_with_error & v2_);

    friend value_with_error operator*(const value_with_error & v_,
                                      const double val_);

    friend value_with_error operator*(const double val_,
                                      const value_with_error & v_);

    friend value_with_error operator/(const value_with_error & v1_,
                                      const value_with_error & v2_);

    friend value_with_error operator/(const value_with_error & v_,
                                      const double val_);

    friend value_with_error operator/(const double val_,
                                      const value_with_error & v_);

    friend value_with_error sqrt(const value_with_error & v_);

    friend value_with_error pow(const value_with_error & v_, const int n_);

    friend value_with_error pow(const value_with_error & v_, const double alpha_);

    friend value_with_error pow2(const value_with_error & v_);

    friend value_with_error pow3(const value_with_error & v_);

    friend value_with_error pow4(const value_with_error & v_);

    friend value_with_error cos(const value_with_error & v_);

    friend value_with_error sin(const value_with_error & v_);

    friend value_with_error tan(const value_with_error & v_);

    friend value_with_error ctan(const value_with_error & v_);

    friend value_with_error acos(const value_with_error & v_);

    friend value_with_error asin(const value_with_error & v_);

    friend value_with_error atan(const value_with_error & v_);

    friend value_with_error exp(const value_with_error & v_);

    friend value_with_error cosh(const value_with_error & v_);

    friend value_with_error sinh(const value_with_error & v_);

    friend value_with_error tanh(const value_with_error & v_);

    friend value_with_error hypot(const value_with_error & v1_,
                                  const value_with_error & v2_);

    friend value_with_error log(const value_with_error & v_);

    friend value_with_error log10(const value_with_error & v_);

    friend std::ostream & operator<<(std::ostream & out_, const value_with_error & v_);
    
  private:
    double _value_ = std::numeric_limits<double>::quiet_NaN();
    double _sigma_ = std::numeric_limits<double>::quiet_NaN();
  };

  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_VALUE_WITH_ERROR_HH
