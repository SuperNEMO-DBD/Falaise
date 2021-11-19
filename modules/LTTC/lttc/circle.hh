#ifndef FALAISE_LTTC_PLUGIN_CIRCLE_HH
#define FALAISE_LTTC_PLUGIN_CIRCLE_HH

// Standard library:
#include <iostream>
#include <tuple>

// Third party:
// - Bayeux :
#include <datatools/utils.h>
#include <geomtools/utils.h>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>
#include <lttc/triplet.hh>

namespace lttc {

  /// \brief Circle in 2D-space
  struct circle
    : public i_curve
  {
  public:
    
    circle() = default;
    
    virtual ~circle() = default;
 
    circle(const point & center_, double r_);
     
    circle(double x0_, double y0_, double r_);

    const point & center() const;
    
    double r() const;

    void set_center(const point & c_);

    void set_r(double r_);

    void compute(double t_, point & vtx_) const override;

    void generate_samples(double t1_, double t2_, polyline & samples_) const override;

    std::tuple<double,double> eval(double x_) const;

    double dist(const point &) const;
    
    static circle make_circle(const point & p1_, const point & p2_, const point & p3_);

    static circle make_circle(const triplet & t_);

    bool is_valid() const;

  public:
    
    // Attributes:
    point  _center_; ///< Center of the circle
    double _r_ = datatools::invalid_real(); ///< Radius of the circle
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_CIRCLE_HH
