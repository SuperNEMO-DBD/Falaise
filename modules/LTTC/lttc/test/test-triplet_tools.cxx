// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>
#include <cassert>

// This project:
#include <lttc/circle.hh>
#include <lttc/tracker.hh>
#include <lttc/track.hh>
#include <lttc/triplet_tools.hh>
#include <lttc/translated_curve.hh>
#include <lttc/rotated_curve.hh>
#include <lttc/hit.hh>
#include <lttc/hit_simulator.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-triplet_tools.data");

    lttc::hits_ijk hijk0;
    lttc::hits_ijk hijk1(1, 2, 3);
    std::clog << "hijk1=" << hijk1 << '\n';
    lttc::hits_ijk hijk2(1, 3, 2);
    std::clog << "hijk2=" << hijk2 << '\n';
    lttc::hits_ijk hijk3(5, 3, 1);
    std::clog << "hijk3=" << hijk3 << '\n';

    assert(! hijk0.is_valid()); 
    assert(hijk1 == hijk2); 
    assert(hijk1 < hijk3); 

    std::clog << '\n';
    lttc::tracker sntracker;
    sntracker.draw(fout);
    fout << '\n';
    fout << '\n';

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

    lttc::point  ccenter(x0, y0);
    lttc::circle c(ccenter, r);
    lttc::point  cfirst;
    c.compute(t1, cfirst); 
    lttc::point  clast;
    c.compute(t2, clast); 
    clast.y += y0min + (y0max-y0min) * ud01(generator);
    
    lttc::vec2 trans(-clast.x, -clast.y);
    lttc::translated_curve tc(c, trans);

    lttc::point origin(0.0, 0.0);
    double angle = (M_PI / 3) * (-1 + 2 * ud01(generator));
    angle = 0.0;
    lttc::rotated_curve rc(tc, origin, angle);
 
    lttc::polyline rcsamples;
    rc.generate_samples(t1, t2, rcsamples);
    fout << "#@curve=circle-translated-rotated\n";
    fout << rcsamples;
    fout << '\n';
    fout << '\n';

    lttc::track track;
    for (const auto & p : rcsamples) {
      if (sntracker.contains(p)) {
        track.pl.push_back(p);
        track.flags |= lttc::track::CIRCLE;
      }
    }
    fout << "#@track\n";
    fout << track.pl;
    fout << '\n';
    fout << '\n';
    
    lttc::hit_simulator::config hitSimConfig;
    hitSimConfig.debug = true;
    hitSimConfig.drift_radius_err = 0.15;
    hitSimConfig.add_noisy_hits = true;
    hitSimConfig.nb_noisy_hits = 5;
    lttc::hit_simulator hitSim(hitSimConfig);
    lttc::hit_collection hits;
    hitSim.generate_hits(generator, track, hits);
    std::clog << "Number of hits: " << hits.size() << '\n';
    int ihit = 0;
    for (const auto & h : hits) {
      fout << "#@hit-" << ihit << '\n';
      h.draw(fout);
      ihit++;
    }
    fout << '\n';

    std::vector<int> removed_hits;
    removed_hits.assign(hits.size(), -1);
    lttc::triplet_tools tt(sntracker);
    lttc::triplet_data td;
    tt.make_from_hits(hits, removed_hits, td);
    std::clog << "Number of triplets: " << td.triplets.size() << '\n';
    fout << "#nb_triplets=" << td.triplets.size() <<'\n';
    for (int it = 0; it < (int) td.triplets.size(); it++) {
      fout << "#triplet-" << it <<'\n';
      td.triplets[it].draw(fout, it);
      // fout << '\n';
      // if (it > 4) break;
    }
    fout << "#nb_circles=" << td.circles.size() <<'\n';
    for (int ic = 0; ic < (int) td.circles.size(); ic++) {
      fout << "#circle-" << ic <<'\n';
      const lttc::circle & c = td.circles[ic];
      lttc::polyline c_pl;
      c.generate_samples(0.0, 1.0, c_pl);
      lttc::draw_polyline(fout, c_pl, ic);
      // fout << '\n';   
      // if (ic > 4) break;
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
