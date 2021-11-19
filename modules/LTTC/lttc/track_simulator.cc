#include <lttc/track_simulator.hh>

// Third party:
// - Bayeux/datatools :
#include <datatools/clhep_units.h>

// This project:
#include <lttc/point.hh>
#include <lttc/circle.hh>
#include <lttc/line.hh>
#include <lttc/translated_curve.hh>
#include <lttc/rotated_curve.hh>
#include <lttc/kinked_curve.hh>

namespace lttc {

  track_simulator::track_simulator(const tracker & sntracker_, const config & cfg_)
    : _sntracker_(&sntracker_)
    , _cfg_(cfg_)
  {
    return;
  }

  void track_simulator::shoot(std::default_random_engine & generator_, track & trk_)
  {
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    polyline trajectory;
    if (_cfg_.track_shape == TRACK_SHAPE_LINE) {
      double x0min =    0.0;
      double x0max =   50.0 * CLHEP::mm;
      double y0min = -200.0 * CLHEP::mm;
      double y0max =  200.0 * CLHEP::mm;
      double tmin  =  300.0 * CLHEP::mm;
      double tmax  =  600.0 * CLHEP::mm;
      
      double x0    = x0min + (x0max-x0min) * ud01(generator_);
      double y0    = y0min + (y0max-y0min) * ud01(generator_);
      double theta = M_PI / 2.2 * (-1 + 2 * ud01(generator_));
      double deltaT = (tmax-tmin);
      deltaT /= std::cos(theta);
      double t     = tmin + deltaT * ud01(generator_);

      double ux = std::cos(theta);
      double uy = std::sin(theta);

      point lstart(x0, y0);
      vector2 ldir(ux, uy);
      line l(lstart, ldir);
      double t1 = 0.0;
      double t2 = t;
      polyline lsamples;
      if (_cfg_.kinked_trajectory && ud01(generator_) < _cfg_.kinked_prob) {
        double kinkAngle = (5.0 + 30.0 * ud01(generator_)) * CLHEP::degree;
        if (ud01(generator_) < 0.5) kinkAngle *= -1;
        double tKink = t1 + (0.2 + 0.6 * ud01(generator_)) * (t2 - t1);
        kinked_curve kc(l, tKink, kinkAngle);
        kc.generate_samples(t1, t2, lsamples);
        trk_.kink = kc.get_pivot();
      } else {
        l.generate_samples(t1, t2, lsamples);
      }
      for (const auto & p : lsamples) {
        if (_sntracker_->contains(p)) {
          trajectory.push_back(p);
          trk_.flags |= track::LINE;
        }
      }

    } else {
      double x0min =    0.0;
      double x0max =  100.0 * CLHEP::mm;
      double y0min = -100.0 * CLHEP::mm;
      double y0max =  100.0 * CLHEP::mm;
      double rmin  =  500.0 * CLHEP::mm;
      double rmax  = 2000.0 * CLHEP::mm;
      double t1min =   0.10;
      double t1max =   0.15;
      double dtmin =   0.10;
      double dtmax =   0.25;

      double x0    = x0min + (x0max-x0min) * ud01(generator_);
      double y0    = y0min + (y0max-y0min) * ud01(generator_);
      double r     = rmin  + (rmax-rmin)   * ud01(generator_);
      y0 -= r;
      double t1    = t1min  + (t1max-t1min)   * ud01(generator_);
      double dt    = dtmin  + (dtmax-dtmin)   * ud01(generator_);
      double t2    = t1 + dt;
      if (t2 > 0.25) {
        t2 = 0.25;
      }

      point  ccenter(x0, y0);
      circle c(ccenter, r);
      point  cfirst;
      c.compute(t1, cfirst); 
      point  clast;
      c.compute(t2, clast);
      
      // double ytmp = clast.y();
      // ytmp += y0min + (y0max-y0min) * ud01(generator_);
      // clast.setY(ytmp);

      // vector2 trans(-clast.x(), -clast.y());
      // translated_curve tc(c, trans);

      point origin(0.0, 0.0);
      if (std::abs(cfirst.x()) < std::abs(clast.x())) {
        origin = cfirst;
      } else {
        origin = clast;
      }
      double angle = (M_PI / 3) * (-1 + 2 * ud01(generator_));
      //  rotated_curve rc(tc, origin, angle);
      rotated_curve rc(c, origin, angle);

      i_curve * crv = &rc;
      polyline rcsamples;
      if (_cfg_.kinked_trajectory && ud01(generator_) < _cfg_.kinked_prob) {
        double kinkAngle = (5.0 + 30.0 * ud01(generator_)) * CLHEP::degree;
        if (ud01(generator_) < 0.5) kinkAngle *= -1;
        double tKink = t1 + (0.2 + 0.6 * ud01(generator_)) * (t2 - t1);
        kinked_curve kc(*crv, tKink, kinkAngle);
        kc.generate_samples(t1, t2, rcsamples);
        trk_.kink = kc.get_pivot();
      } else {
        crv->generate_samples(t1, t2, rcsamples);
      }
      for (const auto & p : rcsamples) {
        if (_sntracker_->contains(p)) {
          trajectory.push_back(p);
          trk_.flags |= track::CIRCLE;
        }
      }
    }
    
    trk_.pl = trajectory;
    if (_cfg_.kinked_trajectory) {
      trk_.flags |= track::KINK;
    }
    return;
  }
 
} // namespace lttc 
