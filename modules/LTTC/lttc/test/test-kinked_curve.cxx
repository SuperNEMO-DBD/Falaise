// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

// This project:
#include <lttc/circle.hh>
#include <lttc/kinked_curve2.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    double x0 = 100.0;
    double y0 = -300.0;
    double r = 400.0;
    lttc::point2  ccenter(x0, y0);
    lttc::circle c(ccenter, r);
    lttc::polyline2 csamples;
    double t1 = 50.0;
    double t2 = 750.0;

    std::ofstream fout("test-kinked_curve.data");
    c.generate_samples(t1, t2, csamples);
    fout << "#@curve=circle\n";
    lttc::draw_polyline(fout, csamples);
    fout << '\n';
    fout << '\n';

    double tPivot = t1 + 0.5 * (t2-t1);
    // tPivot = t1 + 0.99 * (t2-t1);
    // tPivot = t2 - 0.01;
    double kinkAngle = -M_PI/6;
    // kinkAngle = -M_PI/10;
    // kinkAngle = 0;
    lttc::kinked_curve2 kc(c, tPivot, kinkAngle);
    std::clog << "kc.pivot=" << kc.get_pivot() << '\n';
    lttc::polyline2 kcsamples;
    kc.generate_samples(t1, t2,kcsamples);
    fout << "#@curve=circle\n";
    lttc::draw_polyline(fout, kcsamples);
    fout << '\n';
    fout << '\n';
 
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
