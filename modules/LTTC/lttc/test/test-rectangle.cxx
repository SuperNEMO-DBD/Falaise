// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/rectangle.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-rectangle.data");

    lttc::rectangle r1(lttc::point2(1.0, 0.5), lttc::point2(3.0, 1.25));
    r1.draw(fout);
    fout << '\n';

    lttc::rectangle r2(lttc::point2(2.5, 1.5), 0.8, 1.8);
    r2.draw(fout);
    fout << '\n';
 
    lttc::rectangle r3(lttc::point2(2.0, 1.0), 5.8, 3.8);
    r3.draw(fout);
    fout << '\n';
 
    lttc::rectangle r4(lttc::point2(3.0, 3.5), 2.8, 0.25);
    r4.draw(fout);
    fout << '\n';

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
    
    r1.draw_vertexes(fout);
    fout << '\n';
   
    r4.draw_vertexes(fout);
    fout << '\n';
  
    if (r3.contains(r1, 0.01)) {
      std::clog << "R3 contains R1" << std::endl;
    }

    {
      lttc::point2 from(1.0, 1.5);
      lttc::vector2 direction(1.0, 0.25);
      lttc::point2 impact;
      if (r2.unique_intersection(from, direction, impact, 0.01)) {
        std::clog << "Found intersection with R2" << std::endl;
        lttc::draw_point(fout, from, 10);
        lttc::draw_point(fout, impact, 10);
        fout << '\n';
        fout << '\n';
      } else {
        std::clog << "No intersection with R2" << std::endl;
      }
    }
    

    {
      lttc::point2 from(1.0, 4.5);
      lttc::vector2 direction(1.0, 0.25);
      lttc::point2 impact;
      if (r2.unique_intersection(from, direction, impact, 0.01)) {
        std::clog << "Found intersection with R2" << std::endl;
        lttc::draw_point(fout, from, 10);
        lttc::draw_point(fout, impact, 10);
        fout << '\n';
        fout << '\n';
      } else {
        std::clog << "No intersection with R2" << std::endl;
      }
    }
    

    fout.close();
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
