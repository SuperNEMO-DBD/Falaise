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
#include <lttc/point.hh>
#include <lttc/i_curve2.hh>
#include <lttc/triplet2.hh>

namespace lttc {

  /// \brief Circle in XY-plane
  struct circle
    : public i_curve2
  {
  public:
    
    circle() = default;
    
    virtual ~circle() = default;
 
    circle(const point2 & center_, double r_);
     
    circle(double x0_, double y0_, double r_);

    const point2 & center() const;
    
    double r() const;

    void set_center(const point2 & c_);

    void set_r(double r_);
    
    void compute(double t_, point2 & vtx_) const override;

    void generate_samples(double t1_, double t2_, polyline2 & samples_, size_t nsamples_ = 100) const override;

    std::tuple<double,double> eval(double x_) const;

    double dist(const point2 &) const;
    
    static circle make_circle(const point2 & p1_, const point2 & p2_, const point2 & p3_);

    static circle make_circle(const triplet2 & t_);

    static bool build_circle(const point2 & p1_, const point2 & p2_, const point2 & p3_, circle & c_);

    bool is_valid() const;

    void invalidate();

  public:
    
    // Attributes:
    point2 _center_; ///< Center of the circle
    double _r_ = datatools::invalid_real(); ///< Radius of the circle
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_CIRCLE_HH
