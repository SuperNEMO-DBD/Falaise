// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <falaise/geometry/circle.hh>
#include <falaise/geometry/translated_curve2.hh>
#include <falaise/geometry/rotated_curve2.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed = 3141592;
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);
    
    std::ofstream fout("test-circle.data");
    double x0min =    0.0;
    double x0max =  100.0;
    double y0min = -100.0;
    double y0max =  100.0;
    double rmin  =  500.0;
    double rmax  = 2000.0;
    double t1min =    0.0;
    double t1max =   1000.0;
    double dtmin =   1500.0;
    double dtmax =   2500.0;

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

    falaise::geometry::point2  ccenter(x0, y0);
    falaise::geometry::circle c(ccenter, r);
    falaise::geometry::point2  cfirst;
    c.compute(t1, cfirst); 
    falaise::geometry::point2  clast;
    c.compute(t2, clast); 
    clast.setY(clast.y() + y0min + (y0max-y0min) * ud01(generator));
    falaise::geometry::polyline2 csamples;
    c.generate_samples(t1, t2, csamples);
    fout << "#@curve=circle\n";
    falaise::geometry::draw_polyline(fout, csamples);
    fout << '\n';
    fout << '\n';
   
    falaise::geometry::vector2 trans(-clast.x(), -clast.y());
    falaise::geometry::translated_curve2 tc(c, trans);
    falaise::geometry::point2 origin(0.0, 0.0);
    double angle = (M_PI) * (-1 + 2 * ud01(generator));
    falaise::geometry::rotated_curve2 rtc(tc, origin, angle);
 
    falaise::geometry::polyline2 rtcsamples;
    rtc.generate_samples(t1, t2, rtcsamples);
    fout << "#@curve=circle-translated-rotated\n";
    falaise::geometry::draw_polyline(fout, rtcsamples);
    fout << '\n';
    fout << '\n';
 
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
