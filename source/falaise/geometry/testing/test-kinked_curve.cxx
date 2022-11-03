// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

// This project:
#include <falaise/geometry/circle.hh>
#include <falaise/geometry/kinked_curve2.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    double x0 = 100.0;
    double y0 = -300.0;
    double r = 400.0;
    falaise::geometry::point2  ccenter(x0, y0);
    falaise::geometry::circle c(ccenter, r);
    falaise::geometry::polyline2 csamples;
    double t1 = 50.0;
    double t2 = 750.0;

    std::ofstream fout("test-kinked_curve.data");
    c.generate_samples(t1, t2, csamples);
    fout << "#@curve=circle\n";
    falaise::geometry::draw_polyline(fout, csamples);
    fout << '\n';
    fout << '\n';

    double tPivot = t1 + 0.5 * (t2-t1);
    // tPivot = t1 + 0.99 * (t2-t1);
    // tPivot = t2 - 0.01;
    double kinkAngle = -M_PI/6;
    // kinkAngle = -M_PI/10;
    // kinkAngle = 0;
    falaise::geometry::kinked_curve2 kc(c, tPivot, kinkAngle);
    std::clog << "kc.pivot=" << kc.get_pivot() << '\n';
    falaise::geometry::polyline2 kcsamples;
    kc.generate_samples(t1, t2,kcsamples);
    fout << "#@curve=circle\n";
    falaise::geometry::draw_polyline(fout, kcsamples);
    fout << '\n';
    fout << '\n';
 
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
