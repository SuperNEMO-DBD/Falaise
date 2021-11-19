// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/circle.hh>
#include <lttc/translated_curve.hh>
#include <lttc/rotated_curve.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
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
    clast.setY(clast.y() + y0min + (y0max-y0min) * ud01(generator));
    
    lttc::vector2 trans(-clast.x(), -clast.y());
    lttc::translated_curve tc(c, trans);

    lttc::point origin(0.0, 0.0);
    double angle = (M_PI / 3) * (-1 + 2 * ud01(generator));
    lttc::rotated_curve rtc(tc, origin, angle);
 
    lttc::polyline rtcsamples;
    rtc.generate_samples(t1, t2, rtcsamples);
    std::cout << "#@curve=circle-translated-rotated\n";
    lttc::draw_polyline(std::cout, rtcsamples);
    std::cout << '\n';
    std::cout << '\n';
 
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
