// Standard library:
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

// Third party:
// - Bayeux :
#include <datatools/clhep_units.h>

// This project:
#include <lttc/line.hh>
#include <lttc/sngeometry.hh>

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    lttc::tracker sntracker;
    lttc::tracker_drawer(sntracker).draw(std::cout);
    std::cout << '\n';
    std::cout << '\n';

    {
      // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      // seed = 314159;
      // std::default_random_engine generator(seed);
      // std::uniform_real_distribution<double> ud01(0.0, 1.0);

      double x0    = 200.0;
      double y0    = 50.0;
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
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l1samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::line l2(2.0, 4.0, 200.0);
      // lttc::line l2(0.5, -0.866025, -56.6987);
      std::clog << "Line2:\n";
      l2.print(std::clog);
      // std::cout << "#@curve=start\n";
      // std::cout << l2.start << ' ' ;
      // std::cout << l2.direction << '\n';
      // std::cout << '\n';
      // std::cout << '\n';
    
      lttc::polyline l2samples;
      double t2a = 0.0;
      double t2b = +200.0;
      l2.generate_samples(t2a, t2b, l2samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l2samples);
      std::cout << '\n';
      std::cout << '\n';

 
      lttc::line l3 = lttc::line::make_from_theta_r(M_PI /3, -150.0);
      std::clog << "Line3:\n";
      l3.print(std::clog);
      lttc::polyline l3samples;
      double t3a = -100.0;
      double t3b = +100.0;
      l3.generate_samples(t3a, t3b, l3samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l3samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::point A(200.0, 0.0);
      lttc::point B(0.0, 125.0);
      lttc::line l4(A, B);
      std::clog << "Line4:\n";
      l4.print(std::clog);
      lttc::polyline l4samples;
      double t4a = 0.0;
      double t4b = +100.0;
      l4.generate_samples(t4a, t4b, l4samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l4samples);
      std::cout << '\n';
      std::cout << '\n';
    
      lttc::point O(0.0, 0.0);
      std::clog << "OR4 = " << l3.distance(O) << '\n';


      lttc::line pl1 = l1.make_perp(lttc::point(250.0, 75.0));
      lttc::polyline pl1samples;
      double pt1a = 0.0;
      double pt1b = +100.0;
      pl1.generate_samples(pt1a, pt1b, pl1samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, pl1samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::line pl2 = l2.make_perp(lttc::point(-100.0, 0.0));
      lttc::polyline pl2samples;
      double pt2a = -100.0;
      double pt2b = +100.0;
      pl2.generate_samples(pt2a, pt2b, pl2samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, pl2samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::point P(200.0, 0.0);
      lttc::point Q(200.0, 100.0);
      lttc::line l5(P, Q);
      std::clog << "Line5:\n";
      l5.print(std::clog);
      lttc::polyline l5samples;
      double t5a = 0.0;
      double t5b = +100.0;
      l5.generate_samples(t5a, t5b, l5samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l5samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::point R(0.0, 300.0);
      lttc::point S(100.0, 300.0);
      lttc::line l6(R, S);
      std::clog << "Line6:\n";
      l6.print(std::clog);
      lttc::polyline l6samples;
      double t6a = 0.0;
      double t6b = +100.0;
      l6.generate_samples(t6a, t6b, l6samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, l6samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::line pl5 = l5.make_perp(lttc::point(0.0, 25.0));
      lttc::polyline pl5samples;
      double pt5a = -100.0;
      double pt5b = +100.0;
      pl5.generate_samples(pt5a, pt5b, pl5samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, pl5samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::line pl6 = l6.make_perp(lttc::point(50.0, 0.0));
      lttc::polyline pl6samples;
      double pt6a = -100.0;
      double pt6b = +100.0;
      pl6.generate_samples(pt6a, pt6b, pl6samples);
      std::cout << "#@curve=line\n";
      lttc::draw_polyline(std::cout, pl6samples);
      std::cout << '\n';
      std::cout << '\n';
    }
  
    {
      double theta1 = 3 * M_PI/4;
      double p1     = -100 * std::cos(45.0 * CLHEP::degree);
      lttc::line D1 = lttc::line::make_from_theta_r(theta1, p1);
      double theta2 = std::atan2(2.0, 1.0); //60.0 * CLHEP::degree;
      double p2     = 200 * std::cos(M_PI/2 - theta2);
      lttc::line D2 = lttc::line::make_from_theta_r(theta2, p2);
      std::clog << "D1:\n";
      D1.print(std::clog);
      std::clog << "D2:\n";
      D2.print(std::clog);
    
      lttc::point interceptD1D2;
      D1.unique_intersection(D2, interceptD1D2);
      std::clog << " intercept D1xD2:" << interceptD1D2 << "\n";
 
      lttc::polyline D1samples;
      lttc::polyline D2samples;
      D1.generate_samples(-300.0, 300.0, D1samples);
      D2.generate_samples(-300.0, 300.0, D2samples);
      std::cout << "#@curve=D1\n";
      lttc::draw_polyline(std::cout, D1samples);
      std::cout << '\n';
      std::cout << '\n';
      std::cout << "#@curve=D2\n";
      lttc::draw_polyline(std::cout, D2samples);
      std::cout << '\n';
      std::cout << '\n';
    }
    {
      lttc::point A(200.0, 100.0);
      lttc::point B(100.0, 150.0);
      lttc::point C(100.0, 100.0);
      lttc::point D(160.0, 20.0);
      lttc::point E(300.0, 200.0);
      lttc::line AB = lttc::line::make_from_start_stop(A,B);
      lttc::polyline ABsamples;
      AB.generate_samples(-300.0, 300.0, ABsamples);
      std::cout << "#@curve=AB\n";
      lttc::draw_polyline(std::cout, ABsamples);
      std::cout << '\n';
      std::cout << '\n';
      
      lttc::line perp1 = AB.make_perp(C);
      lttc::polyline perp1samples;
      perp1.generate_samples(-300.0, 300.0, perp1samples);
      std::cout << "#@curve=perp1\n";
      lttc::draw_polyline(std::cout, perp1samples);
      std::cout << '\n';
      std::cout << '\n';
      
      // lttc::line BC = lttc::line::make_from_start_stop(B,C);
      // lttc::polyline BCsamples;
      // BC.generate_samples(-300.0, 300.0, BCsamples);
      // std::cout << "#@curve=BC\n";
      // lttc::draw_polyline(std::cout, BCsamples);
      // std::cout << '\n';
      // std::cout << '\n';

      lttc::line perp2 = perp1.make_perp(D);
      lttc::polyline perp2samples;
      perp2.generate_samples(-300.0, 300.0, perp2samples);
      std::cout << "#@curve=perp2\n";
      lttc::draw_polyline(std::cout, perp2samples);
      std::cout << '\n';
      std::cout << '\n';

      lttc::line perp3 = perp2.make_perp(E);
      lttc::polyline perp3samples;
      perp3.generate_samples(-300.0, 300.0, perp3samples);
      std::cout << "#@curve=perp3\n";
      lttc::draw_polyline(std::cout, perp3samples);
      std::cout << '\n';
      std::cout << '\n';
    }
    
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}
