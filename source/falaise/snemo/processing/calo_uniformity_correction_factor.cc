
// Ourselves:
#include "calo_uniformity_correction_factor.h"

// Standard library:
#include <cmath>
#include <iostream>

#include <falaise/snemo/processing/mock_calorimeter_s2c_module_utils.h>

namespace {

  // utility function 
  int fsgn(double val_)
  {
    if (val_ < 0.0) return -1;
    if (val_ > 0.0) return 1;
    return 0;
  }
}

namespace snemo {

  namespace processing {

    CaloUniformityCorrection::CaloUniformityCorrection()
    {
      return;
    }

    double CaloUniformityCorrection::correction_factor(const geomtools::geom_id & scin_gid_,
						       const geomtools::vector_3d & position_in_scin_block_) const
    {
      bool devel = false;
      // devel = true;
      static const double eps_mm = 1e-8;
      double uniformityCorrectionFactor = 1.0;
      bool cornerFix = false;
      cornerFix = true;
      
      // Default MWALL:
      double xyzPosition[3];
      xyzPosition[0] = +position_in_scin_block_.y() / CLHEP::mm;
      xyzPosition[1] = -position_in_scin_block_.x() / CLHEP::mm;
      xyzPosition[2] = -position_in_scin_block_.z() / CLHEP::mm;
      double maxX = std::fabs(mwallScinFrontBlockY_mm) / 2;
      double maxY = std::fabs(mwallScinFrontBlockX_mm) / 2;
      double maxZ = std::fabs(mwallScinFrontBlockZ_mm) / 2;
      
      if (scin_gid_.get_type() == xwallCategory) {
	// XWALL:
	maxX = std::fabs(xwallScinBlockY_mm) / 2;
	maxY = std::fabs(xwallScinBlockX_mm) / 2;
	maxZ = std::fabs(xwallScinBlockZ_mm) / 2;
      } else if (scin_gid_.get_type() == gvetoCategory) {
	// GVETO:
	maxX = std::fabs(gvetoScinBlockY_mm) / 2;
	maxY = std::fabs(gvetoScinBlockX_mm) / 2;
	maxZ = std::fabs(gvetoScinBlockZ_mm) / 2;
      }

      double & x = xyzPosition[0];
      double & y = xyzPosition[1];
      double & z = xyzPosition[2];
      if (devel) std::cerr << "[devel] x,y,z = " << x << " , " << y << " , " << z << '\n';
      //static const double defaultSafetyDist_mm = 5.0;
      // double safetyDist_mm = 0.0 * defaultSafetyDist_mm;

      // Retrieve uniformity correction
      if (scin_gid_.get_type() == mwallCategory) {
	double distToCornerX = maxX - std::fabs(x);
	double distToCornerY = maxY - std::fabs(y);
	double cornerGap = 0.0;
	if (cornerFix) {
	  cornerGap = 20.0;
	}
	double zSkip = maxZ + eps_mm;
	z += zSkip; // add half height of scintillator
	if (devel) {
	  std::cerr << "[devel] mwall zSkip = " << zSkip << '\n';
	  std::cerr << "[devel] mwall fix z = " << z << '\n';
	  std::cerr << "[devel] mwall eff x,y,z = " << x << " , " << y << " , " << z << '\n';
	}
	static const int mwallRowAddress = 3;
	const auto * fitParams = &parameters_mwall_8inch;
	// 8" PMTs only for rows 1 to 11:
	bool inch5 = false;
	if (scin_gid_.get(mwallRowAddress) == 0 or scin_gid_.get(mwallRowAddress) == 12) {
	  // 5" PMTs only for rows 0 and 12:
	  fitParams = &parameters_mwall_5inch;
	  if (cornerFix) {
	    cornerGap = 35.0;
	  }
	  inch5 = true;
	}
	{
	  double xEff = x;
	  double yEff = y;
	  if (distToCornerX+distToCornerY < cornerGap) {
	    double p1[3] = {
	      fsgn(x) * (maxX - cornerGap) ,
	      fsgn(y) * (maxY) ,
	      xyzPosition[2]
	    };
	    double p2[3] = {
	      fsgn(x) * (maxX) ,
	      fsgn(y) * (maxY - cornerGap) ,
	      xyzPosition[2]
	    };
	    double p3[3] = {
	      fsgn(x) * (maxX - cornerGap / 2) ,
	      fsgn(y) * (maxY - cornerGap / 2) ,
	      xyzPosition[2]
	    };
	    if (devel) {
	      std::cerr << "[devel]  p1 = " << p1[0] << " , " << p1[1] << " , " << p1[2] << '\n';
	      std::cerr << "[devel]  p2 = " << p2[0] << " , " << p2[1] << " , " << p2[2] << '\n';
	      std::cerr << "[devel]  p3 = " << p3[0] << " , " << p3[1] << " , " << p3[2] << '\n';
	    }
	    double d01 = std::hypot(x-p1[0], y-p1[1]);
	    double d02 = std::hypot(x-p2[0], y-p2[1]);
	    double d03 = std::hypot(x-p3[0], y-p3[1]);
	    if (devel) {
	      std::cerr << "[devel] mwall corner fix at x,y,z= " << x << " , " << y << " , " << z << '\n';
	      std::cerr << "[devel]  d01 = " << d01 << '\n';
	      std::cerr << "[devel]  d02 = " << d02 << '\n';
	      std::cerr << "[devel]  d03 = " << d03 << '\n';
	    }
	    double w1 = std::min(10.0, 1./d01);
	    double w2 = std::min(10.0, 1./d02);
	    double w3 = std::min(10.0, 1./d03);
	    if (cornerFix) {
	      if (inch5) w3 *= 6;
	    }
	    double wsum = w1+w2+w3;
	    w1 /= wsum;
	    w2 /= wsum;
	    w3 /= wsum;
	    if (devel) {
	      std::cerr << "[devel]  w1 = " << w1 << '\n';
	      std::cerr << "[devel]  w2 = " << w2 << '\n';
	      std::cerr << "[devel]  w3 = " << w3 << '\n';
	    }
	    double c1 = snemo::processing::pol3d(p1, fitParams->data());
	    double c2 = snemo::processing::pol3d(p2, fitParams->data());
	    double c3 = snemo::processing::pol3d(p3, fitParams->data());
	    if (devel) {
	      std::cerr << "[devel]  c1 = " << c1 << '\n';
	      std::cerr << "[devel]  c2 = " << c2 << '\n';
	      std::cerr << "[devel]  c3 = " << c3 << '\n';
	    }
	    uniformityCorrectionFactor = w1 * c1 + w2 * c2 + w3 * c3;
	  } else {
	    x = xEff;
	    y = yEff;
	    uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, fitParams->data());
	  }
	}
      }

      if (scin_gid_.get_type() == xwallCategory) {
	double distToCornerX = maxX - std::fabs(x);
	double distToCornerY = maxY - std::fabs(y);
	double cornerGap = 0.0;
	if (cornerFix) {
	  cornerGap = 50.0;
	}
	double zSkip = maxZ + eps_mm;
	z += zSkip; // add half height of scintillator
	if (devel) {
	  std::cerr << "[devel] xwall zSkip = " << zSkip << '\n';
	  std::cerr << "[devel] xwall fix z = " << z << '\n';
	  std::cerr << "[devel] xwall eff x,y,z = " << x << " , " << y << " , " << z << '\n';
	}
	const auto * fitParams = &parameters_xwall;
	{
	  double xEff = x;
	  double yEff = y;
	  if (distToCornerX+distToCornerY < cornerGap) {
	    double p1[3] = {
	      fsgn(x) * (maxX - cornerGap) ,
	      fsgn(y) * (maxY),
	      xyzPosition[2]
	    };
	    double p2[3] = {
	      fsgn(x) * (maxX) ,
	      fsgn(y) * (maxY - cornerGap) ,
	      xyzPosition[2]
	    };
	    double p3[3] = {
	      fsgn(x) * (maxX - cornerGap / 2) ,
	      fsgn(y) * (maxY - cornerGap / 2) ,
	      xyzPosition[2]
	    };
	    if (devel) {
	      std::cerr << "[devel]  p1 = " << p1[0] << " , " << p1[1] << " , " << p1[2] << '\n';
	      std::cerr << "[devel]  p2 = " << p2[0] << " , " << p2[1] << " , " << p2[2] << '\n';
	      std::cerr << "[devel]  p3 = " << p3[0] << " , " << p3[1] << " , " << p3[2] << '\n';
	    }
	    double d01 = std::hypot(x-p1[0], y-p1[1]);
	    double d02 = std::hypot(x-p2[0], y-p2[1]);
	    double d03 = std::hypot(x-p3[0], y-p3[1]);
	    if (devel) {
	      std::cerr << "[devel] xwall corner fix at x,y,z= " << x << " , " << y << " , " << z << '\n';
	      std::cerr << "[devel]  d01 = " << d01 << '\n';
	      std::cerr << "[devel]  d02 = " << d02 << '\n';
	      std::cerr << "[devel]  d03 = " << d03 << '\n';
	    }
	    double w1 = std::min(10.0, 1./d01);
	    double w2 = std::min(10.0, 1./d02);
	    double w3 = std::min(10.0, 1./d03);
	    if (cornerFix) {
	      w1 *= 4;
	      w2 *= 4;
	    }
	    double wsum = w1+w2+w3;
	    w1 /= wsum;
	    w2 /= wsum;
	    w3 /= wsum;
	    if (devel) {
	      std::cerr << "[devel]  w1 = " << w1 << '\n';
	      std::cerr << "[devel]  w2 = " << w2 << '\n';
	      std::cerr << "[devel]  w3 = " << w3 << '\n';
	    }
	    double c1 = snemo::processing::pol3d(p1, fitParams->data());
	    double c2 = snemo::processing::pol3d(p2, fitParams->data());
	    double c3 = snemo::processing::pol3d(p3, fitParams->data());
	    if (devel) {
	      std::cerr << "[devel]  c1 = " << c1 << '\n';
	      std::cerr << "[devel]  c2 = " << c2 << '\n';
	      std::cerr << "[devel]  c3 = " << c3 << '\n';
	    }
	    uniformityCorrectionFactor = w1 * c1 + w2 * c2 + w3 * c3;
	  } else {
	    x = xEff;
	    y = yEff;
	    uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, fitParams->data());
	  }
	}
      }
      
      if (scin_gid_.get_type() == gvetoCategory) {
	double distToCornerX = maxX - std::fabs(x);
	double distToCornerY = maxY - std::fabs(y);
	double cornerGap = 0.0;
	if (z < 10) {
	  // correction trick for surface hits
	  if (cornerFix) {
	    cornerGap = 120.0;
	  }
	}
	double zSkip = maxZ + eps_mm;
	z += zSkip; // add half height of scintillator
	if (devel) {
	  std::cerr << "[devel] gveto zSkip = " << zSkip << '\n';
	  std::cerr << "[devel] gveto fix z = " << z << '\n';
	  std::cerr << "[devel] gveto eff x,y,z = " << x << " , " << y << " , " << z << '\n';
	}
	const auto * fitParams = &parameters_gveto;
	{
	  double xEff = x;
	  double yEff = y;
	  if (distToCornerX+distToCornerY < cornerGap) {
	    double p1[3] = {
	      fsgn(x) * (maxX - cornerGap) ,
	      fsgn(y) * (maxY),
	      xyzPosition[2]
	    };
	    double p2[3] = {
	      fsgn(x) * (maxX) ,
	      fsgn(y) * (maxY - cornerGap) ,
	      xyzPosition[2]
	    };
	    double p3[3] = {
	      fsgn(x) * (maxX - cornerGap / 2) ,
	      fsgn(y) * (maxY - cornerGap / 2) ,
	      xyzPosition[2]
	    };
	    if (devel) {
	      std::cerr << "[devel]  p1 = " << p1[0] << " , " << p1[1] << " , " << p1[2] << '\n';
	      std::cerr << "[devel]  p2 = " << p2[0] << " , " << p2[1] << " , " << p2[2] << '\n';
	      std::cerr << "[devel]  p3 = " << p3[0] << " , " << p3[1] << " , " << p3[2] << '\n';
	    }
	    double d01 = std::hypot(x-p1[0], y-p1[1]);
	    double d02 = std::hypot(x-p2[0], y-p2[1]);
	    double d03 = std::hypot(x-p3[0], y-p3[1]);
	    if (devel) {
	      std::cerr << "[devel] gveto corner fix at x,y,z= " << x << " , " << y << " , " << z << '\n';
	      std::cerr << "[devel]  d01 = " << d01 << '\n';
	      std::cerr << "[devel]  d02 = " << d02 << '\n';
	      std::cerr << "[devel]  d03 = " << d03 << '\n';
	    }
	    double w1 = std::min(10.0, 1./d01);
	    double w2 = std::min(10.0, 1./d02);
	    if (cornerFix) {
	      w1 *= 2;
	      w2 *= 2;
	    }
	    double w3 = std::min(10.0, 1./d03);
	    double wsum = w1+w2+w3;
	    w1 /= wsum;
	    w2 /= wsum;
	    w3 /= wsum;
	    if (devel) {
	      std::cerr << "[devel]  w1 = " << w1 << '\n';
	      std::cerr << "[devel]  w2 = " << w2 << '\n';
	      std::cerr << "[devel]  w3 = " << w3 << '\n';
	    }
	    double c1 = snemo::processing::pol3d(p1, fitParams->data());
	    double c2 = snemo::processing::pol3d(p2, fitParams->data());
	    double c3 = snemo::processing::pol3d(p3, fitParams->data());
	    if (devel) {
	      std::cerr << "[devel]  c1 = " << c1 << '\n';
	      std::cerr << "[devel]  c2 = " << c2 << '\n';
	      std::cerr << "[devel]  c3 = " << c3 << '\n';
	    }
	    uniformityCorrectionFactor = w1 * c1 + w2 * c2 + w3 * c3;
	  } else {
	    x = xEff;
	    y = yEff;
	    uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, fitParams->data());
	  }
	}
      }
     
      if (devel) {
	std::cerr << "[devel] pos x,y,z = " << xyzPosition[0] << " , " << xyzPosition[1] << " , " << xyzPosition[2] << '\n';
	std::cerr << "[devel] c = " << uniformityCorrectionFactor << '\n';
      }
      return uniformityCorrectionFactor;
    }
    
  } // end of namespace processing

} // end of namespace snemo

/*

// ORIGINAL CODE:
// Retrieve uniformity correction
switch (a_scin_gid.get_type()) {

case 1302: // M-wall
xyzPosition[2] += 15.50000001; // add half height of scintillator
if (std::fabs(xyzPosition[2]) < 1500.) { // only for rows 1 to 11
uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, &_uniformity_correction_parameters_mwall_8inch_[0]);
} else {
uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, &_uniformity_correction_parameters_mwall_5inch_[0]);
}
break;

case 1232: // X-wall
xyzPosition[2] += 75.10000001; // add half height of scintillator
uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, &_uniformity_correction_parameters_xwall_[0]);
break;

case 1252: // V-wall
xyzPosition[2] += 75.10000001;
uniformityCorrectionFactor = snemo::processing::pol3d(xyzPosition, &_uniformity_correction_parameters_gveto_[0]);
break;

default:
DT_THROW(std::logic_error, "unexpected geom ID type for calorimeter [" << a_scin_gid.get_type() << "]");
}
*/
