// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>

// Third party:
// - Bayeux :
#include <datatools/clhep_units.h>

// This project:
#include <lttc/line.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
 
    unsigned seed = 314159;
    std::default_random_engine generator(seed);
     
    {
      double x0    = 50.0;
      double y0    = 20.0;
      double theta = M_PI / 6.0;
      double ux    = std::cos(theta);
      double uy    = std::sin(theta);
      lttc::point   l1start(x0, y0);
      lttc::vector2 l1dir(ux, uy);
      lttc::line    l1(l1start, l1dir);
      std::clog << "Line-1:\n";
      l1.print(std::clog);
    
      lttc::polyline l1samples;
      double t     = 100;
      l1.generate_samples(0.0, t, l1samples);
      std::cout << "#@L1\n";
      lttc::draw_polyline(std::cout, l1samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::point M(55.0, 90.0);
      lttc::fitted_point fM;
      fM.x = M.x();
      fM.y = M.y();
      fM.x_err = 2.0;
      fM.y_err = 2.0;
      std::cout << "#@fM\n";
      fM.draw(std::cout, 1);
      std::cout << '\n';
      std::cout << '\n';

      lttc::vector2 H = l1.orthogonal_projection(M);
      lttc::fitted_point fH;
      fH.x = H.x();
      fH.y = H.y();
      fH.x_err = 2.0;
      fH.y_err = 2.0;
      std::cout << "#@fH\n";
      fH.draw(std::cout, 1);
      std::cout << '\n';

      lttc::line l2 = lttc::line::make_from_start_stop(M, H);
      lttc::polyline l2samples;
      t = 100;
      l2.generate_samples(0.0, t, l2samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l2samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::fitted_line fl;
      fl.theta     = M_PI / 6;
      fl.r         = 200.0;
      fl.r         = -100.0;
      fl.r         = +50.0;
      fl.theta_err = M_PI / 120;
      fl.r_err     = 3.0;
      lttc::fitted_point fpM;
      lttc::orthogonal_projection_to_fitted_line(fl, M, fpM);
      fpM.draw(std::cout, 1);
      for (int i = 0; i < 4; i++) {
        double ti = fl.theta;
        double ri = fl.r;
        if (i == 0) {
          ti += fl.theta_err;
          ri += fl.r_err;
        }
        if (i == 1) {
          ti -= fl.theta_err;
          ri += fl.r_err;
        }
        if (i == 2) {
          ti -= fl.theta_err;
          ri -= fl.r_err;
        }
        if (i == 3) {
          ti += fl.theta_err;
          ri -= fl.r_err;
        }
        lttc::line aLine = lttc::line::make_from_theta_r(ti, ri);
        lttc::polyline aLineSamples;
        double dt = 300;
        aLine.generate_samples(-dt, dt, aLineSamples);
        std::cout << "#@fitted-line#" << i << "\n";
        lttc::draw_polyline(std::cout, aLineSamples);
        std::cout << '\n';
      }


      int ncount=0;
      double xN = 400.0;
      xN = 100.0;
      xN = -400.0;
      xN = -100.0;
      double ymin = 0.0;
      ymin = -200.0;
      double ymax = ymin + 500.0;
      for (double y = ymin; y < ymax; y += 100.0) {
        ncount++;
        std::cerr << "[devel] count=" << ncount << "\n";
        lttc::point N(xN, y);
        lttc::fitted_point fN;
        fN.x = N.x();
        fN.y = N.y();
        fN.x_err = 2.0;
        fN.y_err = 2.0;
        std::cout << "#@fN-" << ncount << "\n";
        fN.draw(std::cout, 1);
        std::cout << '\n';
        std::cout << '\n';
        lttc::fitted_point fpN;
        lttc::orthogonal_projection_to_fitted_line(fl, N, fpN);
        std::cout << "#@fpN-" << ncount << "\n";
        fpN.draw(std::cout, 1);
        std::cout << '\n';
        int n = 1000;
        std::normal_distribution<double> ranTheta(fl.theta, fl.theta_err);
        std::normal_distribution<double> ranR(fl.r, fl.r_err);
        std::cout << "#@random-projection-" << ncount << "\n";
        for (int i = 0; i < n; i++) {
          double ti = ranTheta(generator);
          double ri = ranR(generator);
          lttc::line aLine = lttc::line::make_from_theta_r(ti, ri);
          lttc::point pi;
          pi = aLine.orthogonal_projection(N);
          lttc::draw_point(std::cout, pi, 2);
        }
        std::cout << '\n';
        std::cout << '\n';
      }
      
    }
     
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
