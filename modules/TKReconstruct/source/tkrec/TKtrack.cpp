// TK headers
#include "tkrec/TKtrack.h"

// ClassImp(tkrec::TKtrack);

namespace tkrec {
  
  using namespace std;

  TKtrack::TKtrack(int _side, double _phi, double _r)
    : TKtrack()
  {
    side  = _side;
    phi   = _phi;
    r     = _r;
    a = tan(phi);
    b = -r / cos(phi);
    return;
  }

  TKtrack::TKtrack(int _side, double _a, double _b, double _c, double _d)
    : TKtrack()
  {
    side = _side;
    a = _a;
    b = _b;
    c = _c;
    d = _d;
    phi = atan(a);
    r = -b/sqrt(a*a+1.0);
    theta = atan(c/sqrt(a*a+1.0));
    h = d - a*b*c/(a*a+1.0);
    return;
  }

  void TKtrack::set_side(double _side)
  {
    side = _side;
    return;	
  }

  void TKtrack::set_a(double _a)
  {
    a = _a;
    return;
  }

  void TKtrack::set_b(double _b)
  {
    b = _b;
    return;
  }

  void TKtrack::set_c(double _c)
  {
    c = _c;
    return;
  }

  void TKtrack::set_d(double _d)
  {
    d = _d;
    return;
  }

  void TKtrack::set_phi(double _phi)
  {
    phi = _phi;
    return;
  }

  void TKtrack::set_r(double _r)
  {
    r = _r;
    return;
  }

  void TKtrack::set_theta(double _theta)
  {
    theta = _theta;
    return;
  }

  void TKtrack::set_h(double _h)
  {
    h = _h;
    return;
  }

  void TKtrack::set_chi_squared(double _chi_squared)
  {
    chi_squared = _chi_squared;
    return;
  }

  void TKtrack::set_chi_squared_R(double _chi_squared_R)
  {
    chi_squared_R = _chi_squared_R;
    return;
  }

  void TKtrack::set_chi_squared_Z(double _chi_squared_Z)
  {
    chi_squared_Z = _chi_squared_Z;
    return;
  }

  void TKtrack::set_quality(double _quality)
  {
    quality = _quality;
    return;
  }

  void TKtrack::set_quality_R(double _quality_R)
  {
    quality_R = _quality_R;
    return;
  }

  void TKtrack::set_quality_Z(double _quality_Z)
  {
    quality_Z = _quality_Z;
    return;
  }

  void TKtrack::set_likelihood(double _likelihood)
  {
    likelihood = _likelihood;
    return;
  }

  void TKtrack::set_likelihood_R(double _likelihood_R)
  {
    likelihood_R = _likelihood_R;
    return;
  }

  void TKtrack::set_likelihood_Z(double _likelihood_Z)
  {
    likelihood_Z = _likelihood_Z;
    return;
  }

  void TKtrack::set_ambiguity_type(int _ambiguity_type)
  {
    ambiguity_type = _ambiguity_type;
    return;
  }

  bool TKtrack::has_mirror_image() const
  {
    return mirror_image.get() != nullptr;
  }

  void TKtrack::link_mirror_image(const TKtrackHdl & _mirror_image)
  {
    mirror_image = _mirror_image;
    mirror_image->set_ambiguity_type( ambiguity_type );
    mirror_image->set_chi_squared_R( chi_squared_R );
    mirror_image->set_quality_R( quality_R );
    mirror_image->set_likelihood_R( likelihood_R );
    for(auto i = 0u; i < associated_tr_hits.size(); i++)
      {
	mirror_image->add_associated_tr_hit( associated_tr_hits[i] );
      }
    return;
  }

  int TKtrack::get_side() const
  {
    return side;
  } 

  double TKtrack::get_a() const
  {
    return a;
  } 

  double TKtrack::get_b() const
  {
    return b;
  } 

  double TKtrack::get_c() const
  {
    return c;
  } 

  double TKtrack::get_d() const
  {
    return d;
  } 

  double TKtrack::get_phi() const
  {
    return phi;
  } 

  double TKtrack::get_r() const
  {
    return r;
  } 

  double TKtrack::get_theta() const
  {
    return theta;
  } 

  double TKtrack::get_h() const
  {
    return h;
  } 

  double TKtrack::get_chi_squared() const
  {
    return chi_squared;
  }

  double TKtrack::get_chi_squared_R() const
  {
    return chi_squared_R;
  }

  double TKtrack::get_chi_squared_Z() const
  {
    return chi_squared_Z;
  }

  double TKtrack::get_quality() const
  {
    return quality;
  } 

  double TKtrack::get_quality_R() const
  {
    return quality_R;
  } 

  double TKtrack::get_quality_Z() const
  {
    return quality_Z;
  } 

  double TKtrack::get_likelihood() const
  {
    return likelihood;
  } 

  double TKtrack::get_likelihood_R() const
  {
    return likelihood_R;
  } 

  double TKtrack::get_likelihood_Z() const
  {
    return likelihood_Z;
  } 

  int TKtrack::get_ambiguity_type() const
  {
    return ambiguity_type;
  } 

  const TKtrackHdl & TKtrack::get_mirror_image() const
  {
    return mirror_image;
  } 

  void TKtrack::reconstruct_vertical_least_square()
  {
    std::vector<double> hits_p;
    double projection_distance;
    double distance_from_wire;
    double denominator = sqrt((a*a) + 1);	
    for(auto i = 0u; i < associated_tr_hits.size(); i++)
      {
	double x = associated_tr_hits[i]->get_xy('x');
	double y = associated_tr_hits[i]->get_xy('y');
	distance_from_wire = abs(y - a*x - b) / denominator;
	projection_distance = pow(x, 2.0) + pow(y - b, 2.0) - pow(distance_from_wire, 2.0);
	projection_distance = pow(projection_distance, 0.5);
	hits_p.push_back(projection_distance);
      } 
	
    int sum_n = 0;
    double sum_Z = 0.0;
    double sum_P = 0.0;
    double sum_ZP = 0.0;
    double sum_PP = 0.0;
    for(auto i = 0u; i < associated_tr_hits.size(); i++)
      {
	if(associated_tr_hits[i]->get_h() != 0.0)
	  {
	    sum_Z += associated_tr_hits[i]->get_h();
	    sum_P += hits_p[i];
	    sum_ZP += associated_tr_hits[i]->get_h()*hits_p[i];
	    sum_PP += hits_p[i]*hits_p[i];
	    sum_n++;		
	  }
      }
	
    double c_temp;
    denominator = sum_n*sum_PP - sum_P*sum_P; 
    if(sum_n > 0 && denominator != 0.0)
      {
	c_temp = (sum_n*sum_ZP - sum_P*sum_Z) / denominator;
		
	c = c_temp*(2.0*side-1.0)*pow(1+a*a, 0.5);
	d = (sum_PP*sum_Z - sum_P*sum_ZP) / denominator;
      }	
    else
      {
	c = 0.0;
	d = 0.0;
      }
	
    theta = atan(c/sqrt(a*a+1.0));
    h = d - a*b*c/(a*a+1.0);
	
    this->update_likelihood();
    return;
  }

  void TKtrack::reconstruct_vertical_MLM()
  {
    double S = 0.0;
    double Sx = 0.0;
    double Sy = 0.0;
    double Sz = 0.0;
    double Sxy = 0.0;
    double Sxz = 0.0;
    double Syz = 0.0;
    double Sxx = 0.0;
    double Syy = 0.0;
    double x,y,z;
    double const_Z;
	
    for(auto i = 0u; i < associated_tr_hits.size(); i++)
      {
	z = associated_tr_hits[i]->get_h();
	if(z != 0.0)
	  {	
	    x = associated_tr_hits[i]->get_xy('x');
	    y = associated_tr_hits[i]->get_xy('y');
	    const_Z = pow(associated_tr_hits[i]->get_sigma_Z(), -2.0);
			
	    S = S + const_Z;
	    Sx = Sx + x*const_Z;
	    Sy = Sy + y*const_Z;
	    Sz = Sz + z*const_Z;
	    Sxy = Sxy + x*y*const_Z;
	    Sxz = Sxz + x*z*const_Z;
	    Syz = Syz + y*z*const_Z;
	    Sxx = Sxx + x*x*const_Z;
	    Syy = Syy + y*y*const_Z;
	  }
      }
	
    double denominator = (Sy*Sy-S*Syy)*sin(phi)*sin(phi) + 2.0*(Sx*Sy-S*Sxy)*sin(phi)*cos(phi) + (Sx*Sx-S*Sxx)*cos(phi)*cos(phi);
    if( denominator != 0.0)
      {
	double tan_theta = ((Sy*Sz-S*Syz)*sin(phi) + (Sx*Sz-S*Sxz)*cos(phi)) / denominator;
	double h_temp = (Sz / S) - (Sx*cos(phi) + Sy*sin(phi)) * tan_theta / S;
		
	h = h_temp;
	theta = atan(tan_theta);

	c = tan_theta/cos(phi);
	d = h - r*tan(phi)*tan_theta;
      }
    else 
      {
	h = 0.0;
	theta = 0.0;

	c = 0.0;
	d = 0.0;
      }
    this->update_likelihood();
    return;
  }

  void TKtrack::update_likelihood()
  {
    double sin_phi = sin(phi);
    double cos_phi = cos(phi);
    double tan_theta = tan(theta); 
	
    auto no_hits_R = associated_tr_hits.size();
    auto no_hits_Z = 0u;

    double sum = 0.0;
    double temp;

    double x,y,z;
    double sigma_Z; 
    for(auto i = 0u; i < no_hits_R; i++)
      {
	z = associated_tr_hits[i]->get_h();
	if(z != 0.0)
	  {	
	    no_hits_Z++;
	    x = associated_tr_hits[i]->get_xy('x');
	    y = associated_tr_hits[i]->get_xy('y');
	    sigma_Z = associated_tr_hits[i]->get_sigma_Z();
			
	    temp = h + tan_theta*(y*sin_phi + x*cos_phi) - z; 
	    /*
	    // apparently I dont understand my own parametrization
	    if(r > 0)
	    {
	    temp = h + tan_theta*(y*sin_phi + x*cos_phi) - z; 
	    }			
	    else
	    {
	    temp = h - tan_theta*(y*sin_phi + x*cos_phi) - z; 
	    }
	    */
	    sum = sum + pow( temp/sigma_Z, 2.0 );
	  }		
      }
	
    double weight_R = double(no_hits_R) / double(no_hits_R + no_hits_Z);
    double weight_Z = double(no_hits_Z) / double(no_hits_R + no_hits_Z);
	
    if(chi_squared_R != -1.0 && chi_squared == -1.0)	
      {
	if(no_hits_Z > 1)
	  {
	    chi_squared_Z = sum / double(no_hits_Z);
	    chi_squared = weight_R*chi_squared_R + weight_Z*chi_squared_Z; 
			
	    quality_Z = exp( -0.5*chi_squared_Z );
	    quality = pow(quality_R, weight_R) * pow(quality_Z, weight_Z);
			
	    double norm_const = pow( 2.0*M_PI, -0.5*double(no_hits_Z)); 
	    for(auto i = 0u; i < no_hits_R; i++)
	      {
		if(associated_tr_hits[i]->get_h() != 0.0)
		  {
		    norm_const = norm_const / associated_tr_hits[i]->get_sigma_Z();
		  }
	      }
	    likelihood_Z = norm_const * pow(quality_Z, no_hits_Z);
	    likelihood = likelihood_R * likelihood_Z; 
	  }
	else
	  {
	    chi_squared = chi_squared_R;
	    quality = quality_R;
	    likelihood = likelihood_R;
	  }
      }
    else if(chi_squared_R == -1.0 && chi_squared != -1.0)	
      {
	if(no_hits_Z > 1)
	  {
	    chi_squared_Z = sum / double(no_hits_Z);
	    chi_squared_R = (1.0/weight_R)*chi_squared - (weight_Z/weight_R)*chi_squared_Z;
			
	    quality_Z = exp( -0.5*chi_squared_Z );
	    quality_R = pow(quality, 1.0/weight_R) * pow(quality_Z, -weight_Z/weight_R);
			
	    double norm_const = pow( 2.0*M_PI, -0.5*double(no_hits_Z));
	    for(auto i = 0u; i < no_hits_R; i++)
	      {
		if(associated_tr_hits[i]->get_h() != 0.0)
		  {
		    norm_const = norm_const / associated_tr_hits[i]->get_sigma_Z();
		  }
	      }
	    likelihood_Z = norm_const * pow(quality_Z, no_hits_Z);
	    likelihood_R = likelihood / likelihood_Z; 
	  }
	else
	  {
	    chi_squared_R = chi_squared;
	    quality_R = quality;
	    likelihood_R = likelihood;
	  }
      }
    return;
  }

  void TKtrack::print(std::ostream & out_) const
  {
    out_ << "Track | side: " << side 
	 << ", a = " << a 
	 << ", b = " << b 
	 << ", c = " << c 
	 << ", d = " << d << std::endl; 
    out_ << "		 phi = " << phi 
	 << ", r = " << r 
	 << ", theta = " << theta 
	 << ", h = " << h << std::endl; 
    out_ << "	chi squared: " << chi_squared << std::endl;
    out_ << "	chi squared R: " << chi_squared_R << std::endl;
    out_ << "	chi squared Z: " << chi_squared_Z << std::endl;
    out_ << "	number of associated tracker hits: " << associated_tr_hits.size() << std::endl;
    out_ << "	ambiguity: " << ambiguity_type << std::endl << std::endl;
    return;
  }

  void TKtrack::add_associated_tr_hit(const TKtrhitHdl & tracker_hit)
  {
    associated_tr_hits.push_back(tracker_hit);
    return;
  }

  const std::vector<TKtrhitHdl> & TKtrack::get_associated_tr_hits() const
  {
    return associated_tr_hits;
  }

  // std::vector<const TKtrhit*> TKtrack::get_associated_tr_hits() const
  // {
  //   std::vector<const TKtrhit*> trkhits;
  //   for (const TKtrhit* th : associated_tr_hits) {
  //     trkhits.push_back(th);
  //   }
  //   return trkhits;
  // }

  void TKtrack::add_associated_tr_hit_point(const ConstTKpointHdl & tracker_hit_point)
  {
    associated_tr_hit_points.push_back(tracker_hit_point);
    return;
  }

  const std::vector<ConstTKpointHdl> & TKtrack::get_associated_tr_hit_points() const
  {
    return associated_tr_hit_points;
  }

  // std::vector<const TKpoint*> TKtrack::get_associated_tr_hit_points() const
  // {
  //   std::vector<const TKpoint*> trkpts;
  //   for (const TKpoint* tp : associated_tr_hit_points) {
  //     trkpts.push_back(tp);
  //   }
  //   return trkpts;
  // }

  void TKtrack::calculate_tr_hit_points()
  {
    for(auto i = 0u; i < associated_tr_hits.size(); i++)
      {
	const auto & hitPtr = associated_tr_hits[i];
	double t0 = hitPtr->get_xy('X') * cos(phi) + hitPtr->get_xy('Y') * sin(phi);
	double x0 = t0 * cos(phi) + r * sin(phi);
	double y0 = t0 * sin(phi) - r * cos(phi);
	double z0 = t0 * tan(theta) + h;
	associated_tr_hit_points.push_back(std::make_shared<TKpoint>(x0, y0, z0));
      }
    return;
  }

} //  end of namespace tkrec

