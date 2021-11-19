// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/point.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    //  double lmax = 300.0;
    
    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // std::default_random_engine generator(seed);
    // std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::point p1(0.0, 0.0);
    lttc::point p2(0.5, 0.5);
    lttc::point p3(1.0, 1.0);
    double tolerance = 0.01;
    bool aligned = lttc::are_aligned(p1, p2, p3, tolerance);
    if (aligned) {
      std::clog << "p1,p2,p3 are aligned" << '\n';
    } else {
      std::clog << "p1,p2,p3 are not aligned" << '\n';
    }

    lttc::point p4(1.0 + 10 * tolerance, 1.0);
    aligned = lttc::are_aligned(p1, p2, p4, tolerance);
    if (aligned) {
      std::clog << "p1,p2,p4 are aligned" << '\n';
    } else {
      std::clog << "p1,p2,p4 are not aligned" << '\n';
    }

    lttc::point p5(1.0 + 0.1 * tolerance, 1.0);
    aligned = lttc::are_aligned(p1, p2, p5, tolerance);
    if (aligned) {
      std::clog << "p1,p2,p5 are aligned" << '\n';
    } else {
      std::clog << "p1,p2,p5 are not aligned" << '\n';
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
