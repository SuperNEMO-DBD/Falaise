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
#include <falaise/geometry/line2.hh>
#include <falaise/geometry/fitted_point2.hh>
#include <falaise/geometry/fitted_line2.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-line-2.data");
 
    unsigned seed = 314159;
    std::default_random_engine generator(seed);
     
    {
      double x0    = 50.0;
      double y0    = 20.0;
      double theta = M_PI / 6.0;
      double ux    = std::cos(theta);
      double uy    = std::sin(theta);
      falaise::geometry::point2   l1start(x0, y0);
      falaise::geometry::vector2 l1dir(ux, uy);
      falaise::geometry::line2    l1(l1start, l1dir);
      std::clog << "Line-1:\n";
      l1.print(std::clog);
    
      falaise::geometry::polyline2 l1samples;
      double t     = 100;
      l1.generate_samples(0.0, t, l1samples);
      fout << "#@L1\n";
      falaise::geometry::draw_polyline(fout, l1samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::point2 M(55.0, 90.0);
      falaise::geometry::fitted_point2 fM;
      fM.x = M.x();
      fM.y = M.y();
      fM.x_err = 2.0;
      fM.y_err = 2.0;
      fout << "#@fM\n";
      fM.draw(fout, 1);
      fout << '\n';
      fout << '\n';

      falaise::geometry::vector2 H = l1.orthogonal_projection(M);
      falaise::geometry::fitted_point2 fH;
      fH.x = H.x();
      fH.y = H.y();
      fH.x_err = 2.0;
      fH.y_err = 2.0;
      fout << "#@fH\n";
      fH.draw(fout, 1);
      fout << '\n';

      falaise::geometry::line2 l2 = falaise::geometry::line2::make_from_start_stop(M, H);
      falaise::geometry::polyline2 l2samples;
      t = 100;
      l2.generate_samples(0.0, t, l2samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l2samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::fitted_line2 fl;
      fl.theta     = M_PI / 6;
      fl.r         = 200.0;
      fl.r         = -100.0;
      fl.r         = +50.0;
      fl.theta_err = M_PI / 120;
      fl.r_err     = 3.0;
      falaise::geometry::fitted_point2 fpM;
      falaise::geometry::orthogonal_projection_to_fitted_line(fl, M, fpM);
      fpM.draw(fout, 1);
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
        falaise::geometry::line2 aLine = falaise::geometry::line2::make_from_theta_r(ti, ri);
        falaise::geometry::polyline2 aLineSamples;
        double dt = 300;
        aLine.generate_samples(-dt, dt, aLineSamples);
        fout << "#@fitted-line#" << i << "\n";
        falaise::geometry::draw_polyline(fout, aLineSamples);
        fout << '\n';
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
        falaise::geometry::point2 N(xN, y);
        falaise::geometry::fitted_point2 fN;
        fN.x = N.x();
        fN.y = N.y();
        fN.x_err = 2.0;
        fN.y_err = 2.0;
        fout << "#@fN-" << ncount << "\n";
        fN.draw(fout, 1);
        fout << '\n';
        fout << '\n';
        falaise::geometry::fitted_point2 fpN;
        falaise::geometry::orthogonal_projection_to_fitted_line(fl, N, fpN);
        fout << "#@fpN-" << ncount << "\n";
        fpN.draw(fout, 1);
        fout << '\n';
        int n = 1000;
        std::normal_distribution<double> ranTheta(fl.theta, fl.theta_err);
        std::normal_distribution<double> ranR(fl.r, fl.r_err);
        fout << "#@random-projection-" << ncount << "\n";
        for (int i = 0; i < n; i++) {
          double ti = ranTheta(generator);
          double ri = ranR(generator);
          falaise::geometry::line2 aLine = falaise::geometry::line2::make_from_theta_r(ti, ri);
          falaise::geometry::point2 pi;
          pi = aLine.orthogonal_projection(N);
          falaise::geometry::draw_point(fout, pi, 2);
        }
        fout << '\n';
        fout << '\n';
      }
      
    }
     
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
