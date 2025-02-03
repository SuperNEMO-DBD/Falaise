#ifndef FALAISE_TKRECONSTRUCT_TKTRACK_H
#define FALAISE_TKRECONSTRUCT_TKTRACK_H

// Standard headers
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

// ROOT headers
// #include "TObject.h"

#include "tkrec/TKOMhit.h"
#include "tkrec/TKtrhit.h"
#include "tkrec/TKpoint.h"

#include <datatools/utils.h>

namespace tkrec {

  // line in the form:
  //	y = ax + b
  //	z = cx + d

  // parametrically:
  //	x(s) = s
  //	y(s) = a*s + b
  //	z(s) = c*s + d

  // or in the form:
  // 	x(t) = cos(phi)*cos(theta)*t + r*sin(phi)
  // 	y(t) = sin(phi)*cos(theta)*t - r*cos(phi)
  // 	z(t) = sin(theta)*t + h

  // set of transformations:
  //	a = tan(phi)
  //	b = -r/cos(phi)
  //	c = tan(theta)/cos(phi)
  //	d = h - r*tan(phi)*tan(theta)

  //	phi = atan(a) 
  //	r = -b/sqrt(a*a+1.0)
  //	theta = atan(c/sqrt(a*a+1.0))
  //	h = d - a*b*c/(a*a+1.0)

  // associated tracker hit point: (tracker hit - anode wire (xi,yi), vertical position zi)
  // 	x(t) = (xi*cos(phi)*+yi*sin(phi))*cos(phi) + r*sin(phi)
  // 	y(t) = (xi*cos(phi)*+yi*sin(phi))*sin(phi) - r*cos(phi)
  // 	z(t) = (xi*cos(phi)*+yi*sin(phi))*tan(theta) + h - zi

  class TKtrack;
  typedef std::shared_ptr<TKtrack> TKtrackHdl;
  typedef std::shared_ptr<const TKtrack> ConstTKtrackHdl;
  
  class TKtrack
  //     : public TObject
  {
  private:
		
    int    side = -1;
    double a = datatools::invalid_real();
    double b = datatools::invalid_real();
    double c = datatools::invalid_real();
    double d = datatools::invalid_real();
		
    double phi = datatools::invalid_real();
    double r = datatools::invalid_real();
    double theta = datatools::invalid_real();
    double h = datatools::invalid_real();
		
    // metrics for determining quality of track 
		
    double chi_squared = datatools::invalid_real();
    double chi_squared_R = datatools::invalid_real();
    double chi_squared_Z = datatools::invalid_real();
		
    // it is the n-th root of likelihood, scaled so that the theoretical maximum value is 1,
    //	where n is number of hits in the cluster 
    // value from [0,1] where 1 is absolutely perfect track
    // quality = quality_R * quality_Z 
    double quality = datatools::invalid_real();
    double quality_R = datatools::invalid_real();
    double quality_Z = datatools::invalid_real();
		
    // most reasonable statistical evaluation
    // L = L_R * L_Z
    double likelihood = datatools::invalid_real(); 
    // horizontal part of likelihood
    double likelihood_R = datatools::invalid_real(); 
    // vertical part of likelihood
    double likelihood_Z = datatools::invalid_real();
		
    // in case if ambiguities the mirror images are linked
    // ambiguity_type == 0 means no ambiguity
    int ambiguity_type = 0;
    TKtrackHdl mirror_image;
		
    // association_distance can be changed in reconstruction functions (3 sigma by default = 6mm)
    std::vector<TKtrhitHdl>      associated_tr_hits; 
    std::vector<ConstTKpointHdl> associated_tr_hit_points; 

  public:
		
    TKtrack() = default;
    TKtrack(int _side, double _phi, double _r);
    TKtrack(int _side, double _a, double _b, double _c, double _d);
    virtual ~TKtrack() = default;
		
    void add_associated_tr_hit(const TKtrhitHdl & tracker_hit);
    const std::vector<TKtrhitHdl> & get_associated_tr_hits() const;
		
    void add_associated_tr_hit_point(const ConstTKpointHdl & tracker_hit_point);
    const std::vector<ConstTKpointHdl> & get_associated_tr_hit_points() const;
		
    void calculate_tr_hit_points();
			
    void set_side(double _side);
		
    void set_a   (double _a);
    void set_b   (double _b);
    void set_c   (double _c);
    void set_d   (double _d);
		
    void set_phi   (double _phi);
    void set_r     (double _r);
    void set_theta (double _theta);
    void set_h     (double _h);
		
    void set_chi_squared(double _chi_squared);
    void set_chi_squared_R(double _chi_squared_R);
    void set_chi_squared_Z(double _chi_squared_Z);
		
    void set_quality(double _quality);
    void set_quality_R(double _quality_R);
    void set_quality_Z(double _quality_Z);
		
    void set_likelihood(double _likelihood);
    void set_likelihood_R(double _likelihood_R);
    void set_likelihood_Z(double _likelihood_Z);
		
    void set_ambiguity_type(int _ambiguity_type);
		
    // links a mirror solution to the original track and copies common information: 
    // 	associated hits, quality_R, likelihood_R, chi_squared_R
    //	note: link is not mutual - original track is a "master track"
    void link_mirror_image(const TKtrackHdl & _mirror_image);
		
    int get_side() const;
		
    double get_a() const;
    double get_b() const;
    double get_c() const;
    double get_d() const;
		
    double get_phi  () const;
    double get_r    () const;
    double get_theta() const;
    double get_h    () const;
		
    double get_chi_squared() const;
    double get_chi_squared_R() const;
    double get_chi_squared_Z() const;
		
    double get_quality() const;
    double get_quality_R() const;
    double get_quality_Z() const;
		
    double get_likelihood() const;
    double get_likelihood_R() const;
    double get_likelihood_Z() const;

    int get_ambiguity_type() const;
    bool has_mirror_image() const;
    const TKtrackHdl & get_mirror_image() const;

    // calculates likelihood_Z and likelihood based on associated hits
    void update_likelihood();
    void reconstruct_vertical_least_square();
    void reconstruct_vertical_MLM();

    void print(std::ostream & out_ = std::cout) const;
		
    // ClassDef(TKtrack,1);
    
  };

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKTRACK_H
