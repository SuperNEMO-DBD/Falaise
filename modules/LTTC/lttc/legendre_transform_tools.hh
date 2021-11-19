#ifndef FALAISE_LTTC_PLUGIN_LEGENDRE_TRANSFORM_TOOLS_HH
#define FALAISE_LTTC_PLUGIN_LEGENDRE_TRANSFORM_TOOLS_HH

namespace lttc {

  /// \brief Result of the Legendre transform evaluation functor
  struct ltr_type
  {
    double concave; ///< Evaluation result for the concave part
    double convex;  ///< Evaluation result for the convex part
  };

  /// \brief Legendre transform functor in theta-r space
  struct legendre_transform
  {
    legendre_transform(double x_, double y_, double r_);
   
    ltr_type eval(double theta_) const;

    ltr_type operator()(double theta_) const;

    double x;
    double y;
    double r;
    
  };

  /// \brief Gauss kernel 2D-functor in Legendre transform theta-r space:
  struct lt_gauss_func
  {
    lt_gauss_func(double t0_, double t0_err_, double r0_, double r0_err_);

    double operator()(double t_, double r_) const;
    
    double theta0;
    double theta0_err;
    double r0;
    double r0_err;
    
  };

} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_LEGENDRE_TRANSFORM_TOOLS_HH
  
