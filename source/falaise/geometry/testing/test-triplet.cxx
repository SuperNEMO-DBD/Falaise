// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <falaise/geometry/triplet2.hh>
#include <falaise/geometry/circle.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-triplet.data");

    double lmax = 300.0;
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed = 314159;
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    falaise::geometry::point2 p1(lmax * ud01(generator), lmax * ud01(generator));
    falaise::geometry::point2 p2(lmax * ud01(generator), lmax * ud01(generator));
    falaise::geometry::point2 p3(lmax * ud01(generator), lmax * ud01(generator));
    falaise::geometry::triplet2 t1(p1, p2, p3);
    falaise::geometry::draw_triplet(fout, t1, 1);
    fout << '\n';
    fout << '\n';
    
    falaise::geometry::circle c1 = falaise::geometry::circle::make_circle(t1);
    falaise::geometry::polyline2 c1_pl;
    c1.generate_samples(0.0, 2 * M_PI * c1.r(), c1_pl);
    falaise::geometry::draw_polyline(fout, c1_pl, 2);
    fout << '\n';
   
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
