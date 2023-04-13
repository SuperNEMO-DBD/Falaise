// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// Third party:
// - Bayeux :
#include <datatools/clhep_units.h>

// This project:
#include <falaise/geometry/line2.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    std::ofstream fout("test-line.data");
    
    fout << 0 << ' ' << 0 << ' ' << 0 << '\n'; 
    fout << 0 << ' ' << 0 << ' ' << 0 << '\n'; 
    fout << '\n';
    fout << '\n';

    {
      double x0    = 200.0;
      double y0    = 50.0;
      double theta = M_PI / 6.0;
      double ux    = std::cos(theta);
      double uy    = std::sin(theta);
      falaise::geometry::point2   l1start(x0, y0);
      falaise::geometry::vector2 l1dir(ux, uy);
      falaise::geometry::line2   l1(l1start, l1dir);
      std::clog << "Line-1:\n";
      l1.print(std::clog);
    
      falaise::geometry::polyline2 l1samples;
      double t     = 100;
      l1.generate_samples(0.0, t, l1samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l1samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::line2 l2(2.0, 4.0, 200.0);
      // falaise::geometry::line l2(0.5, -0.866025, -56.6987);
      std::clog << "Line2:\n";
      l2.print(std::clog);
      // fout << "#@curve=start\n";
      // fout << l2.start << ' ' ;
      // fout << l2.direction << '\n';
      // fout << '\n';
      // fout << '\n';
    
      falaise::geometry::polyline2 l2samples;
      double t2a = 0.0;
      double t2b = +200.0;
      l2.generate_samples(t2a, t2b, l2samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l2samples);
      fout << '\n';
      fout << '\n';

 
      falaise::geometry::line2 l3 = falaise::geometry::line2::make_from_theta_r(M_PI /3, -150.0);
      std::clog << "Line3:\n";
      l3.print(std::clog);
      falaise::geometry::polyline2 l3samples;
      double t3a = -100.0;
      double t3b = +100.0;
      l3.generate_samples(t3a, t3b, l3samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l3samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::point2 A(200.0, 0.0);
      falaise::geometry::point2 B(0.0, 125.0);
      falaise::geometry::line2 l4(A, B);
      std::clog << "Line4:\n";
      l4.print(std::clog);
      falaise::geometry::polyline2 l4samples;
      double t4a = 0.0;
      double t4b = +100.0;
      l4.generate_samples(t4a, t4b, l4samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l4samples);
      fout << '\n';
      fout << '\n';
    
      falaise::geometry::point2 O(0.0, 0.0);
      std::clog << "OR4 = " << l3.distance(O) << '\n';


      falaise::geometry::line2 pl1 = l1.make_perp(falaise::geometry::point2(250.0, 75.0));
      falaise::geometry::polyline2 pl1samples;
      double pt1a = 0.0;
      double pt1b = +100.0;
      pl1.generate_samples(pt1a, pt1b, pl1samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, pl1samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::line2 pl2 = l2.make_perp(falaise::geometry::point2(-100.0, 0.0));
      falaise::geometry::polyline2 pl2samples;
      double pt2a = -100.0;
      double pt2b = +100.0;
      pl2.generate_samples(pt2a, pt2b, pl2samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, pl2samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::point2 P(200.0, 0.0);
      falaise::geometry::point2 Q(200.0, 100.0);
      falaise::geometry::line2 l5(P, Q);
      std::clog << "Line5:\n";
      l5.print(std::clog);
      falaise::geometry::polyline2 l5samples;
      double t5a = 0.0;
      double t5b = +100.0;
      l5.generate_samples(t5a, t5b, l5samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l5samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::point2 R(0.0, 300.0);
      falaise::geometry::point2 S(100.0, 300.0);
      falaise::geometry::line2 l6(R, S);
      std::clog << "Line6:\n";
      l6.print(std::clog);
      falaise::geometry::polyline2 l6samples;
      double t6a = 0.0;
      double t6b = +100.0;
      l6.generate_samples(t6a, t6b, l6samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, l6samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::line2 pl5 = l5.make_perp(falaise::geometry::point2(0.0, 25.0));
      falaise::geometry::polyline2 pl5samples;
      double pt5a = -100.0;
      double pt5b = +100.0;
      pl5.generate_samples(pt5a, pt5b, pl5samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, pl5samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::line2 pl6 = l6.make_perp(falaise::geometry::point2(50.0, 0.0));
      falaise::geometry::polyline2 pl6samples;
      double pt6a = -100.0;
      double pt6b = +100.0;
      pl6.generate_samples(pt6a, pt6b, pl6samples);
      fout << "#@curve=line\n";
      falaise::geometry::draw_polyline(fout, pl6samples);
      fout << '\n';
      fout << '\n';
    }
  
    {
      double theta1 = 3 * M_PI/4;
      double p1     = -100 * std::cos(45.0 * CLHEP::degree);
      falaise::geometry::line2 D1 = falaise::geometry::line2::make_from_theta_r(theta1, p1);
      double theta2 = std::atan2(2.0, 1.0); //60.0 * CLHEP::degree;
      double p2     = 200 * std::cos(M_PI/2 - theta2);
      falaise::geometry::line2 D2 = falaise::geometry::line2::make_from_theta_r(theta2, p2);
      std::clog << "D1:\n";
      D1.print(std::clog);
      std::clog << "D2:\n";
      D2.print(std::clog);
    
      falaise::geometry::point2 interceptD1D2;
      D1.unique_intersection(D2, interceptD1D2);
      std::clog << " intercept D1xD2:" << interceptD1D2 << "\n";
 
      falaise::geometry::polyline2 D1samples;
      falaise::geometry::polyline2 D2samples;
      D1.generate_samples(-300.0, 300.0, D1samples);
      D2.generate_samples(-300.0, 300.0, D2samples);
      fout << "#@curve=D1\n";
      falaise::geometry::draw_polyline(fout, D1samples);
      fout << '\n';
      fout << '\n';
      fout << "#@curve=D2\n";
      falaise::geometry::draw_polyline(fout, D2samples);
      fout << '\n';
      fout << '\n';
    }
    {
      falaise::geometry::point2 A(200.0, 100.0);
      falaise::geometry::point2 B(100.0, 150.0);
      falaise::geometry::point2 C(100.0, 100.0);
      falaise::geometry::point2 D(160.0, 20.0);
      falaise::geometry::point2 E(300.0, 200.0);
      falaise::geometry::line2 AB = falaise::geometry::line2::make_from_start_stop(A,B);
      falaise::geometry::polyline2 ABsamples;
      AB.generate_samples(-300.0, 300.0, ABsamples);
      fout << "#@curve=AB\n";
      falaise::geometry::draw_polyline(fout, ABsamples);
      fout << '\n';
      fout << '\n';
      
      falaise::geometry::line2 perp1 = AB.make_perp(C);
      falaise::geometry::polyline2 perp1samples;
      perp1.generate_samples(-300.0, 300.0, perp1samples);
      fout << "#@curve=perp1\n";
      falaise::geometry::draw_polyline(fout, perp1samples);
      fout << '\n';
      fout << '\n';
      
      // falaise::geometry::line2 BC = falaise::geometry::line2::make_from_start_stop(B,C);
      // falaise::geometry::polyline2 BCsamples;
      // BC.generate_samples(-300.0, 300.0, BCsamples);
      // fout << "#@curve=BC\n";
      // falaise::geometry::draw_polyline(fout, BCsamples);
      // fout << '\n';
      // fout << '\n';

      falaise::geometry::line2 perp2 = perp1.make_perp(D);
      falaise::geometry::polyline2 perp2samples;
      perp2.generate_samples(-300.0, 300.0, perp2samples);
      fout << "#@curve=perp2\n";
      falaise::geometry::draw_polyline(fout, perp2samples);
      fout << '\n';
      fout << '\n';

      falaise::geometry::line2 perp3 = perp2.make_perp(E);
      falaise::geometry::polyline2 perp3samples;
      perp3.generate_samples(-300.0, 300.0, perp3samples);
      fout << "#@curve=perp3\n";
      falaise::geometry::draw_polyline(fout, perp3samples);
      fout << '\n';
      fout << '\n';
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
