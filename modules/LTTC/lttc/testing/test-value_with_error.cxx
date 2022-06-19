// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// This project:
#include <lttc/value_with_error.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-value_with_error.data");
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed = 314159;
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> ud01(0.0, 1.0);

    lttc::value_with_error v(1.1, 0.04); 
    std::clog << "v = " << v << '\n';

    auto v2 = pow2(v);
    std::clog << "v2 = " << v2 << '\n';

    lttc::value_with_error v3(2.3, 0.03);
    std::clog << "v3 = " << v3 << '\n';

    auto v4 = v + v3;
    std::clog << "v4 = " << v4 << '\n';

    auto v5 = v * v3;
    std::clog << "v5 = " << v5 << '\n';

    auto v6 = sqrt(v3);
    std::clog << "v6 = " << v6 << '\n';
    
    fout.close();
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
