// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// This project:
#include <fecom/calo_constants.hpp>

int main(int /*argc_*/, char ** /*argv_*/)
{
  std::clog << "ADC dynamics    : " << fecom::calo_constants::adc_dynamics() / CLHEP::volt << " [V]" << std::endl;
  std::clog << "ADC unit        : " << fecom::calo_constants::adc_unit() / (1e-3 * CLHEP::volt) << " [mV]" << std::endl;
  std::clog << "ADC offset unit : " << fecom::calo_constants::adc_offset_unit() / (1e-6 * CLHEP::volt) << " [uV]" << std::endl;
  std::clog << "Sampling freq.  : " << fecom::calo_constants::default_sampling_frequency() / (1e9 * CLHEP::hertz) << " [Ghz]" << std::endl;
  std::clog << "Sample duration : " << fecom::calo_constants::time_sample_duration() / CLHEP::nanosecond << " [ns]" << std::endl;
  std::clog << "Time offset un. : " << fecom::calo_constants::time_offset_unit() / CLHEP::picosecond << " [ps]" << std::endl;


  return EXIT_SUCCESS;
}
