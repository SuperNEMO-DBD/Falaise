// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttcc/rectangle.hpp>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    lttcc::rectangle r1(lttcc::point(1.0, 0.5), lttcc::point(3.0, 1.25));
    r1.draw(std::cout);
    std::cout << '\n';

    lttcc::rectangle r2(lttcc::point(2.5, 1.5), 0.8, 1.8);
    r2.draw(std::cout);
    std::cout << '\n';
 
    lttcc::rectangle r3(lttcc::point(2.0, 1.0), 5.8, 3.8);
    r3.draw(std::cout);
    std::cout << '\n';
 
    lttcc::rectangle r4(lttcc::point(3.0, 3.5), 2.8, 0.25);
    r4.draw(std::cout);
    std::cout << '\n';

    if (r1.overlap(r2, 0.01)) {
      std::clog << "R1 overlaps with R2" << std::endl;
    }
 
    if (r1.overlap(r3, 0.01)) {
      std::clog << "R1 overlaps with R3" << std::endl;
    }
 
    if (r1.overlap(r4, 0.01)) {
      std::clog << "R1 overlaps with R4" << std::endl;
    }

    if (r2.overlap(r1, 0.01)) {
      std::clog << "R2 overlaps with R1" << std::endl;
    }
 
    if (r2.overlap(r3, 0.01)) {
      std::clog << "R2 overlaps with R3" << std::endl;
    }
 
    if (r2.overlap(r4, 0.01)) {
      std::clog << "R2 overlaps with R4" << std::endl;
    }

    if (r3.overlap(r1, 0.01)) {
      std::clog << "R3 overlaps with R1" << std::endl;
    }
 
    if (r3.overlap(r2, 0.01)) {
      std::clog << "R3 overlaps with R2" << std::endl;
    }
 
    if (r3.overlap(r4, 0.01)) {
      std::clog << "R3 overlaps with R4" << std::endl;
    }
 
    if (r4.overlap(r1, 0.01)) {
      std::clog << "R4 overlaps with R1" << std::endl;
    }
 
    if (r4.overlap(r2, 0.01)) {
      std::clog << "R4 overlaps with R2" << std::endl;
    }
 
    if (r4.overlap(r3, 0.01)) {
      std::clog << "R4 overlaps with R3" << std::endl;
    }
    
    r1.draw_vertexes(std::cout);
    std::cout << '\n';
   
    r4.draw_vertexes(std::cout);
    std::cout << '\n';
  
    if (r3.contains(r1, 0.01)) {
      std::clog << "R3 contains R1" << std::endl;
    }
 
  
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
