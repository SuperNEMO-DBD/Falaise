// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/triplet.hh>
#include <lttc/circle.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    double lmax = 300.0;
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // seed = 314159;
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::point p1(lmax * ud01(generator), lmax * ud01(generator));
    lttc::point p2(lmax * ud01(generator), lmax * ud01(generator));
    lttc::point p3(lmax * ud01(generator), lmax * ud01(generator));
    lttc::triplet t1(p1, p2, p3);
    lttc::draw_triplet(std::cout, t1, 1);
    std::cout << '\n';
    std::cout << '\n';
    
    lttc::circle c1 = lttc::circle::make_circle(t1);
    lttc::polyline c1_pl;
    c1.generate_samples(0.0, 1.0, c1_pl);
    lttc::draw_polyline(std::cout, c1_pl, 2);
    std::cout << '\n';
   
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
