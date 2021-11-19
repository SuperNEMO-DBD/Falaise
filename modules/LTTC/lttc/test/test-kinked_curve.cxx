// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>

// This project:
#include <lttc/circle.hh>
#include <lttc/kinked_curve.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    double x0 = 100.0;
    double y0 = -300.0;
    double r = 400.0;
    lttc::point  ccenter(x0, y0);
    lttc::circle c(ccenter, r);
    lttc::polyline csamples;
    double t1 = 0.15;
    double t2 = 0.25;
    c.generate_samples(t1, t2, csamples);
    std::cout << "#@curve=circle\n";
    lttc::draw_polyline(std::cout, csamples);
    std::cout << '\n';
    std::cout << '\n';

    double tPivot = t1 + 0.5 * (t2-t1);
    // tPivot = t1 + 0.99 * (t2-t1);
    // tPivot = t2 - 0.01;
    double kinkAngle = -M_PI/6;
    // kinkAngle = -M_PI/10;
    // kinkAngle = 0;
    lttc::kinked_curve kc(c, tPivot, kinkAngle);
    std::clog << "kc.pivot=" << kc.get_pivot() << '\n';
    lttc::polyline kcsamples;
    kc.generate_samples(t1, t2,kcsamples);
    std::cout << "#@curve=circle\n";
    lttc::draw_polyline(std::cout, kcsamples);
    std::cout << '\n';
    std::cout << '\n';
 
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
