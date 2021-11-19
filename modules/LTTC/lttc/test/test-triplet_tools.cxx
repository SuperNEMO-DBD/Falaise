// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <cassert>

// This project:
#include <lttcc/circle.hpp>
#include <lttcc/tracker.hpp>
#include <lttcc/track.hpp>
#include <lttcc/triplet_tools.hpp>
#include <lttcc/translated_curve.hpp>
#include <lttcc/rotated_curve.hpp>
#include <lttcc/hit.hpp>
#include <lttcc/hit_simulator.hpp>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    lttcc::hits_ijk hijk0;
    lttcc::hits_ijk hijk1(1, 2, 3);
    std::clog << "hijk1=" << hijk1 << '\n';
    lttcc::hits_ijk hijk2(1, 3, 2);
    std::clog << "hijk2=" << hijk2 << '\n';
    lttcc::hits_ijk hijk3(5, 3, 1);
    std::clog << "hijk3=" << hijk3 << '\n';

    assert(! hijk0.is_valid()); 
    assert(hijk1 == hijk2); 
    assert(hijk1 < hijk3); 

    std::clog << '\n';
    lttcc::tracker sntracker;
    sntracker.draw(std::cout);
    std::cout << '\n';
    std::cout << '\n';

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);
    
    double x0min =    0.0;
    double x0max =  100.0;
    double y0min = -100.0;
    double y0max =  100.0;
    double rmin  =  500.0;
    double rmax  = 2000.0;
    double t1min =   0.10;
    double t1max =   0.15;
    double dtmin =   0.10;
    double dtmax =   0.25;

    double x0    = x0min + (x0max-x0min) * ud01(generator);
    double y0    = y0min + (y0max-y0min) * ud01(generator);
    double r     = rmin  + (rmax-rmin)   * ud01(generator);
    y0 -= r;
    double t1    = t1min  + (t1max-t1min)   * ud01(generator);
    double dt    = dtmin  + (dtmax-dtmin)   * ud01(generator);
    double t2    = t1 + dt;
    if (t2 > 0.25) {
      t2 = 0.25;
    }

    lttcc::point  ccenter(x0, y0);
    lttcc::circle c(ccenter, r);
    lttcc::point  cfirst;
    c.compute(t1, cfirst); 
    lttcc::point  clast;
    c.compute(t2, clast); 
    clast.y += y0min + (y0max-y0min) * ud01(generator);
    
    lttcc::vec2 trans(-clast.x, -clast.y);
    lttcc::translated_curve tc(c, trans);

    lttcc::point origin(0.0, 0.0);
    double angle = (M_PI / 3) * (-1 + 2 * ud01(generator));
    angle = 0.0;
    lttcc::rotated_curve rc(tc, origin, angle);
 
    lttcc::polyline rcsamples;
    rc.generate_samples(t1, t2, rcsamples);
    std::cout << "#@curve=circle-translated-rotated\n";
    std::cout << rcsamples;
    std::cout << '\n';
    std::cout << '\n';

    lttcc::track track;
    for (const auto & p : rcsamples) {
      if (sntracker.contains(p)) {
        track.pl.push_back(p);
        track.flags |= lttcc::track::CIRCLE;
      }
    }
    std::cout << "#@track\n";
    std::cout << track.pl;
    std::cout << '\n';
    std::cout << '\n';
    
    lttcc::hit_simulator::config hitSimConfig;
    hitSimConfig.debug = true;
    hitSimConfig.drift_radius_err = 0.15;
    hitSimConfig.add_noisy_hits = true;
    hitSimConfig.nb_noisy_hits = 5;
    lttcc::hit_simulator hitSim(hitSimConfig);
    lttcc::hit_collection hits;
    hitSim.generate_hits(generator, track, hits);
    std::clog << "Number of hits: " << hits.size() << '\n';
    int ihit = 0;
    for (const auto & h : hits) {
      std::cout << "#@hit-" << ihit << '\n';
      h.draw(std::cout);
      ihit++;
    }
    std::cout << '\n';

    std::vector<int> removed_hits;
    removed_hits.assign(hits.size(), -1);
    lttcc::triplet_tools tt(sntracker);
    lttcc::triplet_data td;
    tt.make_from_hits(hits, removed_hits, td);
    std::clog << "Number of triplets: " << td.triplets.size() << '\n';
    std::cout << "#nb_triplets=" << td.triplets.size() <<'\n';
    for (int it = 0; it < (int) td.triplets.size(); it++) {
      std::cout << "#triplet-" << it <<'\n';
      td.triplets[it].draw(std::cout, it);
      // std::cout << '\n';
      // if (it > 4) break;
    }
    std::cout << "#nb_circles=" << td.circles.size() <<'\n';
    for (int ic = 0; ic < (int) td.circles.size(); ic++) {
      std::cout << "#circle-" << ic <<'\n';
      const lttcc::circle & c = td.circles[ic];
      lttcc::polyline c_pl;
      c.generate_samples(0.0, 1.0, c_pl);
      lttcc::draw_polyline(std::cout, c_pl, ic);
      // std::cout << '\n';   
      // if (ic > 4) break;
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
