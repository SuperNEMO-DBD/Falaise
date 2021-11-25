// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>

// Third party:
// - Bayeux :
#include <datatools/clhep_units.h>

// This project:
#include <lttc/line.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    lttc::line l1(lttc::point(-5.0, 0.0), lttc::vector2(-2.0, 1.0));
    l1.print(std::clog);
    
    lttc::polyline l1samples;
    double t     = 10;
    l1.generate_samples(-t, t, l1samples);
    std::cout << "#@L1\n";
    lttc::draw_polyline(std::cout, l1samples);
    std::cout << '\n';
    std::cout << '\n';
      
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
