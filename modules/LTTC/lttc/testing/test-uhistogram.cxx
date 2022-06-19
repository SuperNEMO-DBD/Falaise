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
#include <lttc/uhistogram.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {

    unsigned seed = 314519;
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> uniformGen(0.0, 1.0);
    std::normal_distribution<> gaussianGen1{5.0 * CLHEP::mm, 2.0 * CLHEP::mm};
    std::normal_distribution<> gaussianGen2{-4.0 * CLHEP::mm, 4.0 * CLHEP::mm};
    std::normal_distribution<> gaussianGen3{15.0 * CLHEP::mm, 0.5 * CLHEP::mm};

    std::ofstream fout("test-uhistogram.data");

    lttc::uhistogram h1(0.0 * CLHEP::mm, 0.05 * CLHEP::mm);

    for (int i = 0; i < 1000000; i++) {
      double x;
      double p = uniformGen(generator);
      if (p < 0.9) {
        x = gaussianGen1(generator);
      } else if (p < 0.98) {
        x = gaussianGen2(generator);
      } else {
        x = gaussianGen3(generator);
      }
      h1.fill(x);
    }
   
    h1.store(fout);
    fout << '\n';

    fout.close();
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
