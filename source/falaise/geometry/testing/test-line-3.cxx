// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>

// Third party:
// - Bayeux :
#include <datatools/clhep_units.h>

// This project:
#include <falaise/geometry/line2.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-line-3.data");

    falaise::geometry::line2 l1(falaise::geometry::point2(-5.0, 0.0), falaise::geometry::vector2(-2.0, 1.0));
    l1.print(std::clog);
    
    falaise::geometry::polyline2 l1samples;
    double t     = 10;
    l1.generate_samples(-t, t, l1samples);
    fout << "#@L1\n";
    falaise::geometry::draw_polyline(fout, l1samples);
    fout << '\n';
    fout << '\n';
      
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
