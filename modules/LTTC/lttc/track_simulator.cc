#include <lttc/track_simulator.hh>

// Third party:
// - Bayeux/datatools :
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

// This project:
#include <lttc/point.hh>
#include <lttc/circle.hh>
#include <lttc/line2.hh>
#include <lttc/translated_curve2.hh>
#include <lttc/rotated_curve2.hh>
#include <lttc/kinked_curve2.hh>

namespace lttc {

  track_simulator::track_simulator(const tracker & sntracker_, const config & cfg_)
    : _sntracker_(&sntracker_)
    , _cfg_(cfg_)
  {
    return;
  }

  void track_simulator::shoot(std::default_random_engine & generator_, track2 & trk_)
  {
    DT_LOG_DEBUG(_cfg_.logging, "Entering...");
    std::uniform_real_distribution<double> ud01(0.0, 1.0);
    
    double sampleStep = 2.5 * CLHEP::mm;
    double sStart = datatools::invalid_real();
    double sKink = datatools::invalid_real();
    double sStop = datatools::invalid_real();
    polyline2 trajectory;
    if (_cfg_.track_shape == TRACK_SHAPE_LINE) {
      DT_LOG_DEBUG(_cfg_.logging, "Shooting a LINE...");
      /*** LINE ***/
      double x0min =    0.0;
      double x0max =   50.0 * CLHEP::mm;
      double y0min = -200.0 * CLHEP::mm;
      double y0max =  200.0 * CLHEP::mm;
      double tmin =  300.0 * CLHEP::mm;
      double tmax =  +600.0 * CLHEP::mm;
      
      double x0    = x0min + (x0max-x0min) * ud01(generator_);
      double y0    = y0min + (y0max-y0min) * ud01(generator_);
      double theta = M_PI / 2.2 * (-1.0 + 2 * ud01(generator_));
      double deltaT = (tmax-tmin); // * ud01(generator_);
      deltaT /= std::cos(theta);
      double s1    = 0.0;
      double s2    = -(tmin + deltaT * ud01(generator_));
      if (s2 < s1) {
        std::swap(s1, s2);
      }
      DT_LOG_DEBUG(_cfg_.logging, "s1 = " << s1);
      DT_LOG_DEBUG(_cfg_.logging, "s2 = " << s2);
      sStart = s1;
      sStop  = s2;
 
      /*   y ^ 
       *     :
       *     :
       *     | 
       *     | 
       *     |    
       *     |     ux
       *     +- - -:- - - - - - -> x
       *     |\    :
       *     | \   :
       *     |  \  :
       *     |   \ :
       *     |    \:
       *  uy |.....\
       *     |
       *     |
       */
      double ux = std::cos(theta);
      double uy = std::sin(theta);
      point2  lstart(x0, y0);
      vector2 ldir(ux, uy);
      line2 l(lstart, ldir);
      polyline2 lsamples;
      if (_cfg_.kinked_trajectory && (ud01(generator_) < _cfg_.kinked_prob)) {
        DT_LOG_DEBUG(_cfg_.logging, "Adding a kink...");
        double kinkAngle = (5.0 + 30.0 * ud01(generator_)) * CLHEP::degree;
        if (ud01(generator_) < 0.5) kinkAngle *= -1;
        DT_LOG_DEBUG(_cfg_.logging, "  kinkAngle = " << kinkAngle);
        DT_LOG_DEBUG(_cfg_.logging, "  sStart = " << sStart);
        DT_LOG_DEBUG(_cfg_.logging, "  sStop  = " << sStop);
        sKink = sStart + (0.2 + 0.6 * ud01(generator_)) * (sStop - sStart);
        DT_LOG_DEBUG(_cfg_.logging, "  sKink  = " << sKink);
        kinked_curve2 kc(l, sKink, kinkAngle);
        kc.generate_samples_step(sStart, sStop, lsamples, sampleStep);
        trk_.s_kink = sKink;
        trk_.kink = kc.get_pivot();
        DT_LOG_DEBUG(_cfg_.logging, "  trk_.s_kink = " << trk_.s_kink);
        DT_LOG_DEBUG(_cfg_.logging, "  trk_.kink   = " << trk_.kink);
        trk_.flags |= track::KINK;
      } else {
        l.generate_samples_step(sStart, sStop, lsamples, sampleStep);
      }
      for (int i = 0; i < (int) lsamples.size(); i++) {
        const point2 & pi = lsamples[i];
        double si = lsamples.s[i];
        if (_sntracker_->contains(pi)) {
          trajectory.push_back(pi);
          trajectory.s.push_back(si);
        }
      }
      trk_.flags |= track::LINE;
      trk_.s_start = sStart;
      trk_.s_stop = sStop;
      trk_.pl = trajectory;
    } else {
      DT_LOG_DEBUG(_cfg_.logging, "Shooting a CIRCLE...");
      /*** CIRCLE ***/
      double x0min =    0.0;
      double x0max =  300.0 * CLHEP::mm;
      double y0min = -100.0 * CLHEP::mm;
      double y0max =  100.0 * CLHEP::mm;
      double rmin  =  500.0 * CLHEP::mm;
      double rmax  = 2000.0 * CLHEP::mm;
      double theta1Min = M_PI / 4;
      double theta1Max = M_PI / 2;
      double dsmin =  300.0 * CLHEP::mm;
      double dsmax = 2000.0 * CLHEP::mm;

      double x0    = x0min + (x0max-x0min) * ud01(generator_);
      double y0    = y0min + (y0max-y0min) * ud01(generator_);
      double r     = rmin  + (rmax-rmin)   * ud01(generator_);
      double theta1 = theta1Min + (theta1Max-theta1Min) * ud01(generator_);
      double s1    = r * theta1;
      double ds    = dsmin + (dsmax-dsmin) * ud01(generator_);

      // x0 = 250.0 * CLHEP::mm; 
      // x0 = 0.0 * CLHEP::mm; 
      // y0 = 0.0 * CLHEP::mm; 
      // r  = 1000.0 * CLHEP::mm; 
      // s1 = r * M_PI / 4;
      // s1 = 0.0;
      // ds = r * M_PI / 2;
      
      y0 -= r;
      if (ds > 2.*M_PI*r) ds = 2. * M_PI * r;
      double s2    = s1 + ds;
      sStart = s1;
      sStop = s2;
      DT_LOG_DEBUG(_cfg_.logging, "x0 = " << x0);
      DT_LOG_DEBUG(_cfg_.logging, "y0 = " << y0);
      DT_LOG_DEBUG(_cfg_.logging, "r  = " << r);
      DT_LOG_DEBUG(_cfg_.logging, "s1 = " << s1);
      DT_LOG_DEBUG(_cfg_.logging, "ds = " << ds);
      DT_LOG_DEBUG(_cfg_.logging, "s2 = " << s2);

      point2 ccenter(x0, y0);
      circle c(ccenter, r);
      point2 cfirst;
      c.compute(s1, cfirst); 
      point2 clast;
      c.compute(s2, clast);
      DT_LOG_DEBUG(_cfg_.logging, "cfirst = " << cfirst);
      DT_LOG_DEBUG(_cfg_.logging, "clast  = " << clast);
      
      // double ytmp = clast.y();
      // ytmp += y0min + (y0max-y0min) * ud01(generator_);
      // clast.setY(ytmp);

      // vector2 trans(-clast.x(), -clast.y());
      // translated_curve tc(c, trans);

      point2 origin(0.0, 0.0);
      double angle = (M_PI / 6) * (-1 + 2 * ud01(generator_));
      if (std::abs(cfirst.x()) < std::abs(clast.x())) {
        origin = cfirst;
      } else {
        origin = clast;
      }
      // angle=0.0;
      DT_LOG_DEBUG(_cfg_.logging, "angle=" << angle);
      //  rotated_curve rc(tc, origin, angle);
      rotated_curve2 rc(c, origin, angle);

      trk_.flags |= track::CIRCLE;
      trk_.s_start = sStart;
      trk_.s_stop = sStop;
 
      i_curve2 * crv = &rc;
      // crv = &c;
      polyline2 rcsamples;
      if (_cfg_.kinked_trajectory && ud01(generator_) < _cfg_.kinked_prob) {
        DT_LOG_DEBUG(_cfg_.logging, "Adding a kink...");
        double kinkAngle = (5.0 + 30.0 * ud01(generator_)) * CLHEP::degree;
        if (ud01(generator_) < 0.5) kinkAngle *= -1;
        sKink = s1 + (0.2 + 0.6 * ud01(generator_)) * (s2 - s1);
        kinked_curve2 kc(*crv, sKink, kinkAngle);
        kc.generate_samples_step(sStart, sStop, rcsamples, sampleStep);
        trk_.kink = kc.get_pivot();
        trk_.s_kink = sKink;
        trk_.flags |= track::KINK;
        DT_LOG_DEBUG(_cfg_.logging, "  trk_.s_kink = " << trk_.s_kink);
        DT_LOG_DEBUG(_cfg_.logging, "  trk_.kink   = " << trk_.kink);
      } else {
        crv->generate_samples_step(sStart, sStop, rcsamples, sampleStep);
      }
      // bool continuousTrack = false;
      // int count = 0;
      for (int i = 0; i < (int) rcsamples.size(); i++) {
        const point2 & pi = rcsamples[i];
        double si = rcsamples.s[i];
        trajectory.push_back(pi);
        trajectory.s.push_back(si);
        // if (_sntracker_->contains(pi)) {
        //   // if (not continuousTrack) {
        //   //   continuousTrack = true;
        //   // }
        //   trajectory.push_back(pi);
        //   trajectory.t.push_back(ti);
        //   // count++;
        // } else {
        //   break;
        //   // if (continuousTrack) {
        //   //   trk_.cuts.insert(count);
        //   //   continuousTrack = false;
        //   // }
        // }
      }
      DT_LOG_DEBUG(_cfg_.logging, "trajectory.size = " << trajectory.size());
      trk_.pl = trajectory;
    } // END CIRCLE
    DT_LOG_DEBUG(_cfg_.logging, "Exiting.");
    return;
  }

  void track_simulator::shoot3(std::default_random_engine & generator_,
                               const track2 & trk2_,
                               track3 & trk3_)
  {
    DT_LOG_DEBUG(_cfg_.logging, "Entering...");
    std::uniform_real_distribution<double> zStartRan(_cfg_.z0_min, _cfg_.z0_max);
    std::uniform_real_distribution<double> alphaRan(_cfg_.alpha_min, _cfg_.alpha_max);
    double alpha = alphaRan(generator_);
    double z0 = zStartRan(generator_);
    DT_LOG_DEBUG(_cfg_.logging, "alpha = " << alpha);
    DT_LOG_DEBUG(_cfg_.logging, "z0    = " << z0);
    track3::make_from_xy_track(trk2_, trk3_, z0, alpha);
    DT_LOG_DEBUG(_cfg_.logging, "Exiting.");
    return;
  }
 
} // namespace lttc 
