#include <iostream>
#include <gsl/gsl_fit.h>

int run0(void)
{
  double Es[2] = { 0.5 , 1.0 };
  double Qs[2] = { 2.6 , 5.1 };
  // double errQs[2] = { 0.1 , 0.15 };
  double errQs[2] = { 0.05 , 0.07 };
  double wQs[2];
  wQs[0] = 1.0 / errQs[0] / errQs[0];
  wQs[1] = 1.0 / errQs[1] / errQs[1];
  double c0, c1, cov00, cov01, cov11, chisq;
  int err = gsl_fit_wlinear(Es, 1u, wQs, 1u, Qs, 1u, 2u,
			    &c0, &c1, &cov00, &cov01, &cov11, &chisq);
  if (err != 0) {
    std::cerr << "[error] GSL linear fit failed\n";
    return 1;
  }
  std::clog << "c0    = " << c0 << '\n';
  std::clog << "c1    = " << c1 << '\n';
  std::clog << "cov00 = " << cov00 << '\n';
  std::clog << "cov01 = " << cov01 << '\n';
  std::clog << "cov11 = " << cov11 << '\n';
  std::clog << "chisq = " << chisq << '\n';

  double Estep = 0.1;
  for (double E = 0.05; E < 1.2 + 1e-3 * Estep; E += Estep) {
    double Q, Qerr;
    gsl_fit_linear_est(E, c0, c1, cov00, cov01, cov11, &Q, &Qerr);
    std::cout << E << ' ' << Q << ' ' << Qerr << '\n';
  }
  std::cout << "\n\n";
  return 0;
}

int run1(void)
{
  static const size_t NPARAMS = 2u;
  static const size_t NDATA = 3u;
  double Es[NDATA] = { 0.5 , 0.6, 1.0 };
  double Qs[NDATA] = { 2.6 , 3.1, 5.1 };
  // double errQs[NDATA] = { 0.1 , 0.13, 0.15 };
  double errQs[NDATA] = { 0.05 , 0.06, 0.07 };
  double wQs[NDATA];
  wQs[0] = 1.0 / errQs[0] / errQs[0];
  wQs[1] = 1.0 / errQs[1] / errQs[1];
  wQs[2] = 1.0 / errQs[2] / errQs[2];
  double c0, c1, cov00, cov01, cov11, chisq;
  int err = gsl_fit_wlinear(Es, 1u, wQs, 1u, Qs, 1u, NDATA,
			    &c0, &c1, &cov00, &cov01, &cov11, &chisq);
  if (err != 0) {
    std::cerr << "[error] GSL linear fit failed\n";
    return 1;
  }
  std::clog << "c0    = " << c0 << '\n';
  std::clog << "c1    = " << c1 << '\n';
  std::clog << "cov00 = " << cov00 << '\n';
  std::clog << "cov01 = " << cov01 << '\n';
  std::clog << "cov11 = " << cov11 << '\n';
  std::clog << "chisq = " << chisq << '\n';

  double Estep = 0.1;
  for (double E = 0.05; E < 1.2 + 1e-3 * Estep; E += Estep) {
    double Q, Qerr;
    gsl_fit_linear_est(E, c0, c1, cov00, cov01, cov11, &Q, &Qerr);
    std::cout << E << ' ' << Q << ' ' << Qerr << '\n';
  }
  std::cout << "\n\n";

  return 0;
}

int main(void)
{
  run0();
  run1();
  return 0;
}
