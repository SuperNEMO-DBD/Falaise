// Standard library:
#include <cstdlib>
#include <iostream>
#include <random>

// This project:
#include <lttc/point.hh>

int main(void)
{
  int code = EXIT_SUCCESS;
  try {

    unsigned seed = 314159;
    std::default_random_engine generator(seed);

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

    lttc::fitted_point fp1;
    fp1.x = 2.4;
    fp1.y = 1.3;
    fp1.x_err = 0.8;
    fp1.y_err = 0.3;
    fp1.angle = -M_PI/6;
    fp1.draw(std::cout, 1);
    std::cout << '\n';
    std::cout << '\n';
    
    {
      int n = 40000;
      std::uniform_real_distribution<double> ranxy(0.0, 4.0);
      for (int i = 0; i < n; i++) {
        double x = ranxy(generator);
        double y = ranxy(generator);
        lttc::point P(x, y);
        if (fp1.inside(P, 1.75)) {
          lttc::draw_point(std::cout, P, 1, true);
        }
      } 
      std::cout << '\n';
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
