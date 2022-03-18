// \file lttc_algo_circle.cc

// Ourselves:
#include <lttc/lttc_algo_circle.hh>

// Standard library:
#include <fstream>

// Third party:
#include <mygsl/mean.h>

// This project:
#include <lttc/lttc_algo.hh>
#include <lttc/point.hh>
#include <lttc/tangent_circle_3x1.hh>

namespace lttc {
  
  void lttc_algo_circle::hit_triplet_circle_type::print(std::ostream & out_) const
  {
    out_ << "|-- Triplet ID = " << triplet_id << '\n';
    out_ << "|-- Contacts : ["
         << contacts.p1 << ','
         << contacts.p2 << ','
         << contacts.p3 << "]\n";
    out_ << "`-- Circle data : {center=" << circle_data.center << ",radius=" <<  circle_data.radius << "}\n";
    return;
  }
  
  void lttc_algo_circle::prepare()
  {
    DT_LOG_DEBUG(lttc->cfg.logging, "Entering...");
    const lttc_algo::input_data & indata = *lttc->indata;
    const auto * sntracker = lttc->sntracker;   
    for (uint32_t ihit = 0; ihit < indata.hits.size(); ihit++) {
      const tracker_hit & hit1 = indata.hits[ihit];
      cell_id cid1(hit1.side_id, hit1.layer_id, hit1.row_id);
      if (sntracker->has_tracker_conditions()) {
        // This hit is invalidated because the hit cell is considered dead:
        if (sntracker->get_tracker_conditions().has_dead_cell(cid1)) {
          continue;
        }
      }
      for (uint32_t jhit = ihit+1; jhit < indata.hits.size(); jhit++) {
        const tracker_hit & hit2 = indata.hits[jhit];
        cell_id cid2(hit2.side_id, hit2.layer_id, hit2.row_id);
        if (sntracker->has_tracker_conditions()) {
          if (sntracker->get_tracker_conditions().has_dead_cell(cid2)) {
            continue;
          }
        }
        if (hit2.side_id != hit1.side_id) {
          continue;
        }
        for (uint32_t khit = jhit+1; khit < indata.hits.size(); khit++) {
          const tracker_hit & hit3 = indata.hits[khit];
          cell_id cid3(hit3.side_id, hit3.layer_id, hit3.row_id);
          if (sntracker->has_tracker_conditions()) {
            if (sntracker->get_tracker_conditions().has_dead_cell(cid3)) {
              continue;
            }
          }
          if (hit3.side_id != hit1.side_id) {
            continue;
          }
          // DT_LOG_DEBUG(lttc->cfg.logging, "Hit triplet = (" << ihit << ',' << jhit << ',' << khit << ')');
          hit_triplet_type ht;
          ht.i = ihit;
          ht.j = jhit;
          ht.k = khit;
          hit_triplets.push_back(ht);
        }
      }
    }
    DT_LOG_DEBUG(lttc->cfg.logging, "Number of hit triplets = " << hit_triplets.size());
    DT_LOG_DEBUG(lttc->cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo_circle::terminate()
  {
    DT_LOG_DEBUG(lttc->cfg.logging, "Entering...");
    hit_triplet_circles.clear();
    hit_triplets.clear();
    DT_LOG_DEBUG(lttc->cfg.logging, "Exiting.");
    return;
  }
  
  void lttc_algo_circle::do_steps()
  {
    DT_LOG_DEBUG(lttc->cfg.logging, "Entering...");

    do_step_1();
    do_step_2();
    
    DT_LOG_DEBUG(lttc->cfg.logging, "Exiting.");   
    return;
  }
  
  void lttc_algo_circle::do_step_1()
  {
    DT_LOG_DEBUG(lttc->cfg.logging, "Entering...");
    const lttc_algo::input_data & indata = *lttc->indata;
    const auto * sntracker = lttc->sntracker;   

    tangent_circle_3x1::config_type config;
    config.debug   = false; // datatools::logger::is_debug(lttc->cfg.logging);
    config.epsilon = lttc->cfg.circ_cfg.epsilon;

    DT_LOG_DEBUG(lttc->cfg.logging, "About to process " << hit_triplets.size() << " triplets");
    int trCount = 0;
    int tcCount = 0;
    DT_LOG_DEBUG(lttc->cfg.logging, "Reserving " << 8 * hit_triplets.size() << " triplet circles..");
    hit_triplet_circles.reserve(hit_triplets.size() * 8);
    for (const hit_triplet_type & ht : hit_triplets) {
      DT_LOG_DEBUG(lttc->cfg.logging, "Processing triplet #" << trCount << " : "
                   << "(" << ht.i << ',' << ht.j << ',' << ht.k << ')');
      gergonne::circle c0;
      gergonne::circle c1;
      gergonne::circle c2;
      const tracker_hit & hit0 = indata.hits[ht.i];
      const tracker_hit & hit1 = indata.hits[ht.j];
      const tracker_hit & hit2 = indata.hits[ht.k];
      cell_id cid0(hit0.side_id, hit0.layer_id, hit0.row_id);
      cell_id cid1(hit1.side_id, hit1.layer_id, hit1.row_id);
      cell_id cid2(hit2.side_id, hit2.layer_id, hit2.row_id);
      DT_LOG_DEBUG(lttc->cfg.logging, "  cell IDs :  cid0=" << cid0 << " , cid1=" << cid1 << " , cid2=" << cid2);
      int d01 = cid0.distance(cid1);
      int d02 = cid0.distance(cid2);
      int d12 = cid1.distance(cid2);
      DT_LOG_DEBUG(lttc->cfg.logging, "  d01=" << d01 << " , d02=" << d02 << " , d12=" << d12);
      if (d01 > lttc->cfg.circ_cfg.max_cell_dist) {
        DT_LOG_DEBUG(lttc->cfg.logging, "  >>>>>>>> d01>MAX");
        continue;
      }
      if (d02 > lttc->cfg.circ_cfg.max_cell_dist) {
        DT_LOG_DEBUG(lttc->cfg.logging, "  >>>>>>>> d02>MAX");
        continue;
      }
      if (d12 > lttc->cfg.circ_cfg.max_cell_dist) {
        DT_LOG_DEBUG(lttc->cfg.logging, "  >>>>>>>> d12>MAX");
        continue;
      }
      double r0 = hit0.drift_radius;
      double r1 = hit1.drift_radius;
      double r2 = hit2.drift_radius;
      point2 pos0 = sntracker->cell_position(cid0);
      point2 pos1 = sntracker->cell_position(cid1);
      point2 pos2 = sntracker->cell_position(cid2);
      c0.p.x = pos0.x();
      c0.p.y = pos0.y();
      c0.r   = r0;
      c1.p.x = pos1.x();
      c1.p.y = pos1.y();
      c1.r   = r1;
      c2.p.x = pos2.x();
      c2.p.y = pos2.y();
      c2.r   = r2;
      tangent_circle_3x1::input_data iData;
      iData.hit_indexes[0] = ht.i;
      iData.hit_indexes[1] = ht.j;
      iData.hit_indexes[2] = ht.k;
      iData.circles[0] = &c0;
      iData.circles[1] = &c1;
      iData.circles[2] = &c2;
      tangent_circle_3x1::output_data oData;
      config.debug   = false;
      if (trCount<10) config.debug = true;
      tangent_circle_3x1 tc3x1(config);
      tc3x1.process(iData, oData);
      DT_LOG_DEBUG(lttc->cfg.logging, "  -> Number of tangent circles = " << oData.circles.size());   
      tcCount += oData.circles.size();
      for (const auto & tCirc : oData.circles) {
        if (tCirc.tangent_circle.r >= lttc->cfg.circ_cfg.min_radius) {
          hit_triplet_circle_type htc;
          htc.triplet_id = trCount;
          htc.contacts.p1.set(tCirc.contacts[0].x, tCirc.contacts[0].y);
          htc.contacts.p2.set(tCirc.contacts[1].x, tCirc.contacts[1].y);
          htc.contacts.p3.set(tCirc.contacts[2].x, tCirc.contacts[2].y);
          htc.circle_data.center.set(tCirc.tangent_circle.p.x, tCirc.tangent_circle.p.y);
          htc.circle_data.radius = tCirc.tangent_circle.r;
          hit_triplet_circles.push_back(htc);
        }
      }
      trCount++;
    }
    DT_LOG_DEBUG(lttc->cfg.logging, "  -> Total number of tangent circles = " << tcCount);   
    DT_LOG_DEBUG(lttc->cfg.logging, "  -> Total number of selected circles = " << hit_triplet_circles.size());   
       
    DT_LOG_DEBUG(lttc->cfg.logging, "Exiting.");   
    return;
  }
 
  void lttc_algo_circle::do_step_2()
  {
    DT_LOG_DEBUG(lttc->cfg.logging, "Entering...");
    const lttc_algo::input_data & indata = *lttc->indata;
    const auto * sntracker = lttc->sntracker;
    int htcCount = 0;
    for (const auto & htc : hit_triplet_circles) {
      DT_LOG_DEBUG(lttc->cfg.logging, "Processing hit triplet circle #" << htcCount);   
      int trId = htc.triplet_id;
      hit_triplet_type ht = hit_triplets[trId];
      const tracker_hit & hit1 = indata.hits[ht.i];
      const tracker_hit & hit2 = indata.hits[ht.j];
      const tracker_hit & hit3 = indata.hits[ht.k];
      cell_id cid1(hit1.side_id, hit1.layer_id, hit1.row_id);
      cell_id cid2(hit2.side_id, hit2.layer_id, hit2.row_id);
      cell_id cid3(hit3.side_id, hit3.layer_id, hit3.row_id);
      // double r1 = hit1.drift_radius;
      // double r2 = hit2.drift_radius;
      // double r3 = hit3.drift_radius;
      point2 pos1 = sntracker->cell_position(cid1);
      point2 pos2 = sntracker->cell_position(cid2);
      point2 pos3 = sntracker->cell_position(cid3);
      point2 center = htc.circle_data.center;
      double radius = htc.circle_data.radius;
      mygsl::arithmetic_mean meanR1MR2;
      mygsl::arithmetic_mean meanR1PR2X;
      mygsl::arithmetic_mean meanR1PR2Y;
      {
        legendre_transform lt(center.x(), center.y(), radius);
        ltr_type ltRes1 = lt(0.0);
        ltr_type ltRes2 = lt(M_PI * 0.5);
        double r1a = ltRes1.concave;
        double r2a = ltRes1.convex;
        double r1b = ltRes2.concave;
        double r2b = ltRes2.convex;
        double r1m2  = r1a - r2a;
        double r1p2a = r1a + r2a;
        double r1p2b = r1b + r2b;
        meanR1MR2.add(r1m2);
        meanR1PR2X.add(r1p2a);
        meanR1PR2Y.add(r1p2b);
      }
      vector2 dir1 = (pos1 - center).unit();
      vector2 dir2 = (pos2 - center).unit();
      vector2 dir3 = (pos3 - center).unit();
      mygsl::arithmetic_mean meanX0;
      mygsl::arithmetic_mean meanY0;
      mygsl::arithmetic_mean meanR0;
      meanX0.add(center.x());
      meanY0.add(center.y());
      meanR0.add(radius);
      for (unsigned int iBlur = 0; iBlur < 8; iBlur++) {
        double r1_shift = hit1.drift_radius_err;
        double r2_shift = hit2.drift_radius_err;
        double r3_shift = hit3.drift_radius_err;
        if (iBlur & datatools::bit_mask::bit00) {
          r1_shift *= -1;
        }
        if (iBlur & datatools::bit_mask::bit01) {
          r2_shift *= -1;
        }
        if (iBlur & datatools::bit_mask::bit02) {
          r3_shift *= -1;
        }
        point2 blurPos1 = htc.contacts.p1 + r1_shift * dir1;
        point2 blurPos2 = htc.contacts.p2 + r2_shift * dir2; 
        point2 blurPos3 = htc.contacts.p3 + r3_shift * dir3;
        gergonne::point p1 = {blurPos1.x(), blurPos1.y()};
        gergonne::point p2 = {blurPos2.x(), blurPos2.y()};
        gergonne::point p3 = {blurPos3.x(), blurPos3.y()};
        double eps = lttc->cfg.circ_cfg.epsilon;
        gergonne::circle blurCircle = gergonne::make_circle_from_three_points(p1, p2, p3, eps, false);
        if (blurCircle.is_valid()) {
          DT_LOG_DEBUG(lttc->cfg.logging, "  Valid blur circle");   
          meanX0.add(blurCircle.p.x);
          meanY0.add(blurCircle.p.y);
          meanR0.add(blurCircle.r);
          // XXX
          legendre_transform lt(blurCircle.p.x,
                                blurCircle.p.y,
                                blurCircle.r);
          ltr_type ltRes1 = lt(0.0);
          ltr_type ltRes2 = lt(M_PI * 0.5);
          double r1a = ltRes1.concave;
          double r2a = ltRes1.convex;
          double r1b = ltRes2.concave;
          double r2b = ltRes2.convex;
          double r1m2  = r1a - r2a;
          double r1p2a = r1a + r2a;
          double r1p2b = r1b + r2b;
          DT_LOG_DEBUG(lttc->cfg.logging, "    r1m2  =" << r1m2);   
          DT_LOG_DEBUG(lttc->cfg.logging, "    r1p2a =" << r1p2a);   
          DT_LOG_DEBUG(lttc->cfg.logging, "    r1p2b =" << r1p2b);
          meanR1MR2.add(r1m2);
          meanR1PR2X.add(r1p2a);
          meanR1PR2Y.add(r1p2b);
        } else {
          DT_LOG_DEBUG(lttc->cfg.logging, "    XXX Invalid blur circle");   
        }
      }
      // DT_LOG_DEBUG(lttc->cfg.logging, "  X0 mean : N= " << meanX0.get_n());   
      // DT_LOG_DEBUG(lttc->cfg.logging, "  Y0 mean : N= " << meanY0.get_n());   
      // DT_LOG_DEBUG(lttc->cfg.logging, "  R0 mean : N= " << meanR0.get_n());   
      // double x0Bar = meanX0.get_mean();
      // double x0Sigma = std::sqrt(meanX0.get_variance());
      // double y0Bar = meanY0.get_mean();
      // double y0Sigma = std::sqrt(meanY0.get_variance());
      // double r0Bar = meanR0.get_mean();
      // double r0Sigma = std::sqrt(meanR0.get_variance());
      // DT_LOG_DEBUG(lttc->cfg.logging, "  X0 = " << x0Bar << " +/- " << x0Sigma);   
      // DT_LOG_DEBUG(lttc->cfg.logging, "  Y0 = " << y0Bar << " +/- " << y0Sigma);   
      // DT_LOG_DEBUG(lttc->cfg.logging, "  R0 = " << r0Bar << " +/- " << r0Sigma);
      
      double rBar   = meanR1MR2.get_mean();
      double rSigma = std::sqrt(meanR1MR2.get_variance());
      double xBar   = meanR1PR2X.get_mean();
      double xSigma = std::sqrt(meanR1PR2X.get_variance());
      double yBar   = meanR1PR2Y.get_mean();
      double ySigma = std::sqrt(meanR1PR2Y.get_variance());
      DT_LOG_DEBUG(lttc->cfg.logging, "  r = " << rBar << " +/- " << rSigma);   
      DT_LOG_DEBUG(lttc->cfg.logging, "  x = " << xBar << " +/- " << xSigma);   
      DT_LOG_DEBUG(lttc->cfg.logging, "  y = " << yBar << " +/- " << ySigma);
      htcCount++;
    }   
    DT_LOG_DEBUG(lttc->cfg.logging, "Exiting.");   
    return;
  }

  void lttc_algo_circle::do_step_3()
  {
    DT_LOG_DEBUG(lttc->cfg.logging, "Entering...");
    hr.initialize(0.0 * CLHEP::mm, 5.0 * CLHEP::mm);
    hx.initialize(0.0 * CLHEP::mm, 5.0 * CLHEP::mm);
    hy.initialize(0.0 * CLHEP::mm, 5.0 * CLHEP::mm);
    int htcCount = 0;
    for (const auto & htc : hit_triplet_circles) {
      DT_LOG_DEBUG(lttc->cfg.logging, "Processing triplet #" << htcCount
                   << " from triplet #" << htc.triplet_id);
      legendre_transform lt(htc.circle_data.center.x(),
                            htc.circle_data.center.y(),
                            htc.circle_data.radius);
      ltr_type ltRes1 = lt(0.0);
      ltr_type ltRes2 = lt(M_PI * 0.5);
      double r1a = ltRes1.concave;
      double r2a = ltRes1.convex;
      double r1b = ltRes2.concave;
      double r2b = ltRes2.convex;
      double r1m2  = r1a - r2a;
      double r1p2a = r1a + r2a;
      double r1p2b = r1b + r2b;
      DT_LOG_DEBUG(lttc->cfg.logging, "  r1m2  =" << r1m2);   
      DT_LOG_DEBUG(lttc->cfg.logging, "  r1p2a =" << r1p2a);   
      DT_LOG_DEBUG(lttc->cfg.logging, "  r1p2b =" << r1p2b);
      hr.fill(r1m2);
      hx.fill(r1p2a);
      hy.fill(r1p2b);
      htcCount++;
    }
    
    {
      std::ofstream fout(lttc->cfg.draw_prefix +"circle_hist.data");
      fout << "#@r1-r2" << '\n';
      hr.store(fout);
      fout << '\n';
      
      fout << "#@r1+r2x" << '\n';
      hx.store(fout);
      fout << '\n';

      fout << "#@r1+r2y" << '\n';
      hy.store(fout);
      fout << '\n';

      fout.close();
    }
    
    DT_LOG_DEBUG(lttc->cfg.logging, "Exiting.");   
    return;
  }
 
} // namespace lttc 
