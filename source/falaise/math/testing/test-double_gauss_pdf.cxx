// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

#include <mygsl/rng.h>

// This project:
#include <falaise/math/double_gauss_pdf.hpp>


int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    double xmax = 2.0;
    double sigl = 0.1;
    double sigr = 0.2;
  
    falaise::math::double_gauss_pdf dg(xmax, sigl, sigr);

    {
      double s = 0.0;
      std::ofstream fout("test-double_gauss_pdf.data");
      double dx = 0.001;
      for (double x = 0.0; x < 5.0; x += dx) {
	double fx = dg(x);
	fout << x << ' ' << fx << '\n';
	s += fx;
      }
      fout << '\n';
      fout.close();
      s *= dx;
      std::clog << "I = " << s << std::endl;
    }
  
    {
      falaise::math::double_gauss_generator dgGen(dg);

      std::string rng_id = mygsl::rng::DEFAULT_RNG_ID;
      int32_t seed = 12345;
      mygsl::rng random;
      random.initialize(rng_id, seed);

      std::ofstream fout("test-double_gauss_random.data");
      auto n = 100000u;
      for (auto i = 0u; i < n; i++) {
	double x = dgGen.shoot(random);
	fout << x << '\n';	
      }
      fout << '\n';
      fout.close();
     
    }
   
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
