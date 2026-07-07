// test_snemo_processing_calo_signal_baseline.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <cmath>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>

// This project:
#include <falaise/snemo/processing/calo_signal_baseline.h>

void test0();
void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::processing::calo_signal_baseline'!" << std::endl;
    test0();
    test1();
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  std::clog << "The end." << std::endl;
  return (error_code);
}

void test0()
{
  std::clog << "\ntest1" << std::endl;
  namespace snproc = snemo::processing;

  snproc::calo_signal_waveform_type uddCaloSignal;

  long rngSeed = 31415;
  std::default_random_engine rng(rngSeed);

  double meanAdc = 2048.0;
  double noiseAmplitude = 1.5;
  std::normal_distribution adcNoiseDraw(0.0, noiseAmplitude);

  auto nbSamples = 1024u;
  for (auto iSample = 0u; iSample < nbSamples; iSample++) {
    double adc = meanAdc + adcNoiseDraw(rng);
    std::int32_t digiAdc = (std::int32_t) adc;
    uddCaloSignal.push_back(digiAdc);
  }

  // // Simulate a signal with a log-normal shape:
  // double lnMu = 0.0;
  // double lnSigma = 1.7;
  // double lnAmplitude = 400.0;
  // auto sampleSignalStart = 250u;
  // for (auto iSample = sampleSignalStart; iSample < nbSamples; iSample++) {
  //   auto di = iSample - sampleSignalStart + 1;
  //   double x = di / 200.;
  //   double signalAmp = lnAmplitude * std::exp(-0.5 * std::pow((std::log(x) - lnMu)/lnSigma,2)) / x / lnSigma / std::sqrt(2 * M_PI);
  //   double adc = meanAdc - signalAmp + adcNoiseDraw(rng);
  //   std::int32_t digiAdc = (std::int32_t) adc;
  //   uddCaloSignal[iSample] = digiAdc;
  // } 

  {
    std::ofstream fDigi("test_processing_calo_signal_baseline-1.data");
    for (auto iSample = 0u; iSample < nbSamples; iSample++) {
      auto digiAdc = uddCaloSignal[iSample];
      fDigi << iSample << ' ' << digiAdc << '\n';
    }
    fDigi.close();
  }

  snproc::calo_signal_baseline_calculator sigBaselineCalc;
  sigBaselineCalc.verbosity = datatools::logger::PRIO_DEBUG;
  snproc::calo_signal_baseline_measurement baselineMeas = sigBaselineCalc(uddCaloSignal);
  
  std::cout << baselineMeas << '\n';
  
  return;
}

void test1()
{
  std::clog << "\ntest1" << std::endl;
  namespace snproc = snemo::processing;

  snproc::calo_signal_waveform_type uddCaloSignal;

  long rngSeed = 31416;
  std::default_random_engine rng(rngSeed);

  double meanAdc = 2048.0;
  double noiseAmplitude = 1.5;
  std::normal_distribution adcNoiseDraw(0.0, noiseAmplitude);

  auto nbSamples = 1024u;
  for (auto iSample = 0u; iSample < nbSamples; iSample++) {
    double adc = meanAdc + adcNoiseDraw(rng);
    std::int32_t digiAdc = (std::int32_t) adc;
    uddCaloSignal.push_back(digiAdc);
  }

  // Simulate a signal with a log-normal shape:
  double lnMu = 0.0;
  double lnSigma = 1.7;
  double lnAmplitude = 400.0;
  auto sampleSignalStart = 250u;
  for (auto iSample = sampleSignalStart; iSample < nbSamples; iSample++) {
    auto di = iSample - sampleSignalStart + 1;
    double x = di / 200.;
    double signalAmp = lnAmplitude * std::exp(-0.5 * std::pow((std::log(x) - lnMu)/lnSigma,2)) / x / lnSigma / std::sqrt(2 * M_PI);
    double adc = meanAdc - signalAmp + adcNoiseDraw(rng);
    std::int32_t digiAdc = (std::int32_t) adc;
    uddCaloSignal[iSample] = digiAdc;
  } 

  {
    std::ofstream fDigi("test_processing_calo_signal_baseline-1.data");
    for (auto iSample = 0u; iSample < nbSamples; iSample++) {
      auto digiAdc = uddCaloSignal[iSample];
      fDigi << iSample << ' ' << digiAdc << '\n';
    }
    fDigi.close();
  }

  snproc::calo_signal_baseline_calculator sigBaselineCalc;
  sigBaselineCalc.verbosity = datatools::logger::PRIO_DEBUG;
  snproc::calo_signal_baseline_measurement baselineMeas = sigBaselineCalc(uddCaloSignal);
  
  std::cout << baselineMeas << '\n';
  
  return;
}
